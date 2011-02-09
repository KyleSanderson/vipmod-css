#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

/* Defines */
#define PLUGIN_VERSION	"1.0g"
#define DESCRIPTION		"Creates VIP style gameplay on AS_ maps. Protect the VIP, Team :3"

/* Globals */
new g_VIPIndex = -1;
new g_VIPChanceArray[MAXPLAYERS+1];
new g_VIPLastChosen;
new g_MoneyInt;

new String:g_team_list[16][64]; // HLX:CE Support
new String:Small1[][] = {"models/player/vip/small2/vip.dx80.vtx", "models/player/vip/small2/vip.dx90.vtx", "models/player/vip/small2/vip.mdl", "models/player/vip/small2/vip.phy", "models/player/vip/small2/vip.sw.vtx", "models/player/vip/small2/vip.vvd", "materials/models/player/vip/small1/erdim_cylmap.vmt", "materials/models/player/vip/small1/erdim_cylmap.vtf", "materials/models/player/vip/small1/erdim_facemap.vmt", "materials/models/player/vip/small1/eyeball_l.vmt", "materials/models/player/vip/small1/eyeball_l.vtf", "materials/models/player/vip/small1/eyeball_r.vmt", "materials/models/player/vip/small1/eyeball_r.vtf", "materials/models/player/vip/small1/UrbanTemp.vmt", "materials/models/player/vip/small1/UrbanTemp.vtf"};
new String:StaticVIPModel[] = {"models/player/vip/small2/vip.mdl"};

new bool:g_bTimeChanged;
new bool:g_bGotResQCoords;
new bool:g_bRoundEnd;
new bool:g_bIsModEnabled;
new bool:g_bInitialized;

new Float:g_fAddedTime;
new Float:g_fVIPResQ1Min[3], Float:g_fVIPResQ1Max[3];
new Float:g_fVIPResQ2Min[3], Float:g_fVIPResQ2Max[3];
new Float:g_fVIPResQdTele[3];

new Handle:g_hVIPCoords1		=	INVALID_HANDLE;
new Handle:g_hVIPCoords2		=	INVALID_HANDLE;
new Handle:g_hVIPCoordsTele		=	INVALID_HANDLE;
new Handle:g_hRoundTime			=	INVALID_HANDLE;
new Handle:g_hRoundTimer		=	INVALID_HANDLE;
new Handle:g_hFreezeTime		=	INVALID_HANDLE;
new Handle:g_hTerminateRound	=	INVALID_HANDLE;
new Handle:g_hGameConfiguration	=	INVALID_HANDLE;

public Plugin:myinfo =
{
    name 		=		"VIPMod",			// http://www.thesixtyone.com/s/KkzxBiqbs4C/
    author		=		"Kyle Sanderson", 
    description	=		DESCRIPTION, 
    version		=		PLUGIN_VERSION, 
    url			=		"http://SourceMod.net"
};

public OnConfigsExecuted()
{
	decl String:MapName[64];
	GetCurrentMap(MapName, sizeof(MapName));
	if(strncmp(MapName, "as_", 3) != 0)
	{
		return;
	}
	VIPMod_Start();
}

public OnClientDisconnect(client)
{
	if(client == g_VIPIndex)
	{
		PrintToChatAll("\x04[VIPMod]\x03 The VIP (\x04%N\x03) has disconnected.", g_VIPIndex);
		LogPlyrPlyrEvent(0, g_VIPIndex, "triggered", "VIP_Killed"); // HLX:CE Support
		FireRoundEnd(0);
	}
}

public VIPMod_Start()
{
	AddCommandListeners(1);
	EventHooks(1);
	GetTeams(); 	// HLX:CE Support
	SDKHooks(1);
	RoundTime(1);
	ModelsSounds(1);
	CreateTimer(0.4, CheckVIPPosition, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
	g_bIsModEnabled = true;
	if(!g_bInitialized)
	{
		InitialStart();
	}
}

public InitialStart()
{
	g_hVIPCoords1 = CreateConVar("vip_escapezone", "0", "VIP Rescue Zone", FCVAR_PLUGIN);
	g_hVIPCoords2 = CreateConVar("vip_escapezone2", "0", "VIP Rescue Zone2", FCVAR_PLUGIN);
	g_hVIPCoordsTele = CreateConVar("vip_teleport", "0", "VIP Teleport", FCVAR_PLUGIN);
	CreateConVar("vip_version", PLUGIN_VERSION, DESCRIPTION, FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY);
	g_hGameConfiguration = LoadGameConfigFile("plugin.VIPMod");
	
	// Prep the SDKCall for "TerminateRound".
	StartPrepSDKCall(SDKCall_GameRules);
	PrepSDKCall_SetFromConf(g_hGameConfiguration, SDKConf_Signature, "TerminateRound");
	PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	g_hTerminateRound = EndPrepSDKCall();
	g_MoneyInt = FindSendPropInfo("CCSPlayer", "m_iAccount");
	g_bInitialized = true;
}

public AddCommandListeners(Status)
{
	switch(Status)
	{
		case 0:
		{
			RemoveCommandListener(RestrictBuyCommand, "buy");
			RemoveCommandListener(RestrictBuyCommand, "autobuy");
			RemoveCommandListener(RestrictBuyCommand, "rebuy");
		}
		
		case 1:
		{
			AddCommandListener(RestrictBuyCommand, "buy");
			AddCommandListener(RestrictBuyCommand, "autobuy");
			AddCommandListener(RestrictBuyCommand, "rebuy");
		}
	}
}

public Action:RestrictBuyCommand(client, const String:command[], argc)
{
	if(client == g_VIPIndex)
	{
		PrintToChat(client, "\x04[VIPMod]\x03 You can't buy anything!");
		return Plugin_Handled;
	}
	return Plugin_Continue;
}

public OnMapEnd()
{
	if(!g_bIsModEnabled)
	{
		return;
	}
	
	AddCommandListeners(0);
	SDKHooks(0);
	EventHooks(0);
	RoundTime(0);
	VipCoordinates(0);
	VIPSelection(0);
	g_bRoundEnd = false;
	g_bIsModEnabled = false;
}

public SDKHooks(Status)
{
	switch(Status)
	{
		case 0:
		{
			SDKUnhook(FindEntityByClassname(0, "cs_player_manager"), SDKHook_ThinkPost, CSPlayerManagerThinkPost);
		}
		
		case 1:
		{
			SDKHook(FindEntityByClassname(0, "cs_player_manager"), SDKHook_ThinkPost, CSPlayerManagerThinkPost);
		}
	}
}

public ModelsSounds(Status)
{
	switch(Status)
	{
		case 0:
		{
			// Happy Dance!
		}
		case 1:
		{
			new Size = sizeof(Small1)-1;
			for(new i; i<=Size; i++)
			{
				AddFileToDownloadsTable(Small1[i]);
				PrecacheModel(Small1[i]);
			}
			PrecacheSound("radio/vip.wav");
		}
	}
}

public EventHooks(Status)
{
	switch(Status)
	{
		case 0:
		{
			UnhookEvent("player_death",	OnPlayerDeath,	EventHookMode_Pre);
			UnhookEvent("round_start",	OnRoundStart,	EventHookMode_PostNoCopy);
			UnhookEvent("round_end",	OnRoundEnd,		EventHookMode_Pre);
		}
		
		case 1:
		{
			HookEvent("player_death",	OnPlayerDeath,	EventHookMode_Pre);
			HookEvent("round_start",	OnRoundStart,	EventHookMode_PostNoCopy);
			HookEvent("round_end",		OnRoundEnd,		EventHookMode_Pre);
		}
	}
}

public Action:OnPlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
	new killed = GetClientOfUserId(GetEventInt(event, "userid"));
	if (killed == g_VIPIndex)
	{
		new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));
		PrintToChatAll("\x04[VIPMod] %N\x03 has Assassinated the \x04VIP\x03 (\x04%N\x03).", attacker, killed);
		LogPlyrPlyrEvent(attacker, g_VIPIndex, "triggered", "VIP_Killed"); // HLX:CE Logging Support
		FireRoundEnd(0);
	}
	return Plugin_Continue;
}

public FireRoundEnd(Status)
{
	switch (Status)
	{
		case 0:
		{
			SDKCall(g_hTerminateRound, 4.0, 2);		// The VIP has been assassinated!
			SetCash(2);
		}
		
		case 1:
		{
			SDKCall(g_hTerminateRound, 4.0, 1);		// The VIP has escaped!
			SetCash(3);
		}
		
		case 2:
		{
			SDKCall(g_hTerminateRound, 4.0, 14);	// The VIP has not escaped!
			SetCash(2);
		}
	}
}

public Action:OnRoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
	if(g_VIPIndex != -1)
	{
		SDKUnhook(g_VIPIndex, SDKHook_OnTakeDamage, OnClientTakeDamage); // http://www.thesixtyone.com/s/7kQaXbb9bLT/
		SDKUnhook(g_VIPIndex, SDKHook_WeaponCanUse, OnWeaponCanUse);
	}
	
	if(!g_bGotResQCoords)
	{
		VipCoordinates(1);
	}
	VIPSelection(0);
	VIPSelection(1);
	if(g_VIPIndex > 0)
	PrintCenterText(g_VIPIndex, "You are the VIP");
	CheckTime();
	RoundTime(2);
	g_bRoundEnd = false;
	return Plugin_Continue;
}

public SetCash(Status)
{
	new PlayerAlive;
	switch(Status)
	{
		case 2:
		{
			for(new i = 1; i <=MaxClients; i++)
			{
				if(IsClientConnected(i) && IsClientInGame(i) && GetClientTeam(i) == 2)
				{
					PlayerAlive = IsPlayerAlive(i);
					switch(PlayerAlive)
					{
						case 0:
						{
							SetEntData(i, g_MoneyInt, GetEntData(i, g_MoneyInt, 4)+1500, 4);
						}
						
						case 1:
						{
							SetEntData(i, g_MoneyInt, GetEntData(i, g_MoneyInt, 4)+3000, 4);
						}
					}
				}
			}
		}
		case 3:
		{
			for(new i = 1; i <=MaxClients; i++)
			{
				if(IsClientConnected(i) && IsClientInGame(i) && GetClientTeam(i) == 3)
				{
					PlayerAlive = IsPlayerAlive(i);
					switch(PlayerAlive)
					{
						case 0:
						{
							SetEntData(i, g_MoneyInt, GetEntData(i, g_MoneyInt, 4)+1500, 4);
						}
						
						case 1:
						{
							SetEntData(i, g_MoneyInt, GetEntData(i, g_MoneyInt, 4)+3000, 4);
						}
					}
				}
			}
		}
	}
}

public Action:OnRoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
	g_bRoundEnd = true;
	return Plugin_Continue;
}

public VIPSelection(Status)
{
	switch (Status)
	{
		case 0: 
		{
			g_VIPIndex = -1;
			for (new i = 0; i <= MaxClients; i++)
			{
				g_VIPChanceArray[i] = 0;
			}
		}
		
		case 1:
		{
			new PlayerCount = 0;
			if(GetRealClientCount(2) <= 1)
			{
				for (new i = 1; i<=MaxClients; i++)
				{
					if (IsClientConnected(i) && IsClientInGame(i) && IsPlayerAlive(i) && !IsFakeClient(i) && GetClientTeam(i) == 3)
					{
						g_VIPChanceArray[PlayerCount] = i;
						PlayerCount++;
					}
				}
				
				if (PlayerCount == 0)
				{
					g_VIPIndex = -1;
					g_VIPLastChosen = -1;
					return;
				}
				
				g_VIPIndex = g_VIPChanceArray[GetRandomInt(0, PlayerCount-1)];
				SetVIPPlayer(g_VIPIndex);
				g_VIPLastChosen = GetClientUserId(g_VIPIndex);
				return;
			}

			for (new i = 1; i<=MaxClients; i++)
			{
				if (IsClientConnected(i) && IsClientInGame(i) && IsPlayerAlive(i) && !IsFakeClient(i) && GetClientUserId(i) != g_VIPLastChosen && GetClientTeam(i) == 3)
				{
					g_VIPChanceArray[PlayerCount] = i;
					PlayerCount++;
				}
			}
			
			if (PlayerCount == 0)
			{
				g_VIPIndex = -1;
				g_VIPLastChosen = -1;
				return;
			}
			
			g_VIPIndex = g_VIPChanceArray[GetRandomInt(0, PlayerCount-1)];
			g_VIPLastChosen = GetClientUserId(g_VIPIndex);
			SetVIPPlayer(g_VIPIndex);
		}
	}
}

public SetVIPPlayer(client)
{
	SetEntProp(client, Prop_Send, "m_iHealth", 200);
	SetEntProp(client, Prop_Send, "m_bHasHelmet", 1);
	SetEntProp(client, Prop_Send, "m_ArmorValue", 50);
	SDKHook(client, SDKHook_WeaponCanUse, OnWeaponCanUse);
	if(!IsModelPrecached(StaticVIPModel))
	{
		ThrowError("You've forgotten to install the base archive!");
		return;
	}
	SetEntityModel(client, StaticVIPModel);
}

public Action:CheckVIPPosition(Handle:timer)
{
	if(g_VIPIndex <= 0)
	{
		return Plugin_Handled;
	}
	
	if(IsClientConnected(g_VIPIndex) && IsClientInGame(g_VIPIndex) && IsPlayerAlive(g_VIPIndex) && !g_bRoundEnd)
	{
		decl Float:VIPPos[3];
		GetClientAbsOrigin(g_VIPIndex, VIPPos);
		if(IsPointInLocation(VIPPos, g_fVIPResQ1Min, g_fVIPResQ1Max))
		{
			if(g_fVIPResQdTele[0] != 0 && g_fVIPResQdTele[1] != 0 && g_fVIPResQdTele[2] != 0)
			{
				TeleportEntity(g_VIPIndex, g_fVIPResQdTele, NULL_VECTOR, NULL_VECTOR);
			}
			SetEntProp(g_VIPIndex, Prop_Data, "m_MoveType", MOVETYPE_NONE);
			SDKHook(g_VIPIndex, SDKHook_OnTakeDamage, OnClientTakeDamage);
			LogPlayerEvent(g_VIPIndex, "triggered", "VIP_Escaped"); // HLX:CE Logging Support
			FireRoundEnd(1);
		}
		if(IsPointInLocation(VIPPos, g_fVIPResQ2Min, g_fVIPResQ2Max))
		{
			if(g_fVIPResQdTele[0] != 0 && g_fVIPResQdTele[1] != 0 && g_fVIPResQdTele[2] != 0)
			{
				TeleportEntity(g_VIPIndex, g_fVIPResQdTele, NULL_VECTOR, NULL_VECTOR);
			}
			SetEntProp(g_VIPIndex, Prop_Data, "m_MoveType", MOVETYPE_NONE);
			SDKHook(g_VIPIndex, SDKHook_OnTakeDamage, OnClientTakeDamage);
			LogPlayerEvent(g_VIPIndex, "triggered", "VIP_Escaped"); // HLX:CE Logging Support
			FireRoundEnd(1);
		}
	}
	return Plugin_Continue;
}

public CSPlayerManagerThinkPost(entity)
{
    if (g_VIPIndex != -1)
    {
		if(IsClientConnected(g_VIPIndex) && IsClientInGame(g_VIPIndex) && IsPlayerAlive(g_VIPIndex))
		{
			SetEntProp(entity, Prop_Send, "m_iPlayerVIP", g_VIPIndex);
		}
    }
}

public Action:OnWeaponCanUse(client, weapon)
{
	decl String:WeaponName[14];
	GetEdictClassname(weapon, WeaponName, sizeof(WeaponName));
	ReplaceString(WeaponName, sizeof(WeaponName), "weapon_", "", false);
	if(!StrEqual(WeaponName, "knife") && !StrEqual(WeaponName, "usp"))
	{
		return Plugin_Handled;
	}
	return Plugin_Continue;
}

public VipCoordinates(Status)
{
	switch (Status)
	{
		case 0:
		{
			SetConVarString(g_hVIPCoords1, "0");
			SetConVarString(g_hVIPCoords2, "0");
			for(new i; i <= 2; i++)
			{
				g_fVIPResQ1Min[i] = 0.0;
				g_fVIPResQ1Max[i] = 0.0;
				g_fVIPResQ2Min[i] = 0.0;
				g_fVIPResQ2Max[i] = 0.0;
			}
			g_bGotResQCoords = false;
		}
		
		case 1:
		{
			g_hVIPCoords1 = FindConVar("vip_escapezone");
			if(g_hVIPCoords1 != INVALID_HANDLE)
			{
				decl String:Coords1[256], String:VIPCoordsArray1[6][8];
				GetConVarString(g_hVIPCoords1, Coords1, sizeof(Coords1));
				ReplaceString(Coords1, sizeof(Coords1), "  ", " ");
				ExplodeString(Coords1, " ", VIPCoordsArray1, sizeof(VIPCoordsArray1), sizeof(VIPCoordsArray1[]));
				for(new i; i <= 5; i++)
				{
					switch(i)
					{
						case 0, 1, 2:
						{
							g_fVIPResQ1Min[i] = StringToFloat(VIPCoordsArray1[i]);
						}
				
						case 3, 4, 5:
						{
							g_fVIPResQ1Max[i-3] = StringToFloat(VIPCoordsArray1[i]);
						}
					}
				}
			}
	
			g_hVIPCoords2 = FindConVar("vip_escapezone2");
			if(g_hVIPCoords2 != INVALID_HANDLE)
			{
				decl String:Coords2[256], String:VIPCoordsArray2[6][8];
				GetConVarString(g_hVIPCoords2, Coords2, sizeof(Coords2));
				ReplaceString(Coords2, sizeof(Coords2), "  ", " ");
				ExplodeString(Coords2, " ", VIPCoordsArray2, sizeof(VIPCoordsArray2), sizeof(VIPCoordsArray2[]));
				for(new i; i <= 5; i++)
				{
					switch(i)
					{
						case 0, 1, 2:
						{
							g_fVIPResQ2Min[i] = StringToFloat(VIPCoordsArray2[i]);
						}
				
						case 3, 4, 5:
						{
							g_fVIPResQ2Max[i-3] = StringToFloat(VIPCoordsArray2[i]);
						}
					}
				}
			}
	
			g_hVIPCoordsTele = FindConVar("vip_teleport");
			if(g_hVIPCoordsTele != INVALID_HANDLE)
			{
				decl String:CoordsTele[256], String:VIPCoordsTeleArray[3][8];
				GetConVarString(g_hVIPCoordsTele, CoordsTele, sizeof(CoordsTele));
				ReplaceString(CoordsTele, sizeof(CoordsTele), "  ", " ");
				ExplodeString(CoordsTele, " ", VIPCoordsTeleArray, sizeof(VIPCoordsTeleArray), sizeof(VIPCoordsTeleArray[]));
				for(new i; i <= 2; i++)
				{
					g_fVIPResQdTele[i] = StringToFloat(VIPCoordsTeleArray[i]);
				}
			}
	
			if((g_fVIPResQ1Min[0] == 0) && (g_fVIPResQ2Min[0] == 0) && (g_fVIPResQ1Min[1] == 0) && (g_fVIPResQ1Max[2] == 0))
			{
				PrintToChatAll("\x04[VIPMod]\x03 Sorry, I was unable to get any rescue coordinates for this map. Bug?");
				PrintToServer("[VIPMod] Sorry, I was unable to get any rescue coordinates for this map. Bug?");
				return;
			}
			g_bGotResQCoords = true;
		}
	}
}

public RoundTime(Status)
{
	switch (Status)
	{
		case 0:
		{
			if(g_hRoundTimer != INVALID_HANDLE)
			{
				KillTimer(g_hRoundTimer);
				g_hRoundTimer = INVALID_HANDLE;
			}
			UnhookConVarChange(g_hRoundTime, OnTimeChange);
			UnhookConVarChange(g_hFreezeTime, OnTimeChange);
		}
		
		case 1:
		{
			g_hRoundTime = FindConVar("mp_roundtime");
			g_hFreezeTime = FindConVar("mp_freezetime");
			g_fAddedTime = (GetConVarFloat(g_hRoundTime)*60 + GetConVarFloat(g_hFreezeTime));
			HookConVarChange(g_hRoundTime, OnTimeChange);
			HookConVarChange(g_hFreezeTime, OnTimeChange);
		}
		
		case 2:
		{
			if (g_hRoundTimer != INVALID_HANDLE)
			{
				KillTimer(g_hRoundTimer);
				g_hRoundTimer = INVALID_HANDLE;
			}
			g_hRoundTimer = CreateTimer(g_fAddedTime, RoundTimeTimer, _, TIMER_FLAG_NO_MAPCHANGE);
		}
	}
}

public OnTimeChange(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
	g_bTimeChanged = true;
}

public CheckTime()
{
	if(g_bTimeChanged)
	{
		g_fAddedTime = (GetConVarFloat(g_hRoundTime)*60 + GetConVarFloat(g_hFreezeTime));
		g_bTimeChanged = false;
	}
}

public Action:RoundTimeTimer(Handle:timer)
{
	LogPlayerEvent(0, "triggered", "VIP_NotEscaped"); // HLX:CE Logging Support - There's no support for this from Source? o.O
	FireRoundEnd(2);
	g_hRoundTimer = INVALID_HANDLE;
}

public Action:OnClientTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
	return Plugin_Handled;
}

stock GetRealClientCount(Status)
{
	new Count;
	switch(Status)
	{
		case 2:
		{
			for(new i = 1; i <=MaxClients; i++)
			{
				if (IsClientConnected(i) && IsClientInGame(i) && !IsFakeClient(i) && GetClientTeam(i) == 2)
				{
					Count++;
				}
			}
		}
		
		case 3:
		{
			for(new i = 1; i <=MaxClients; i++)
			{
				if (IsClientConnected(i) && IsClientInGame(i) && !IsFakeClient(i) && GetClientTeam(i) == 3)
				{
					Count++;
				}
			}
		}
	}
	return Count;
}

/* Huge Thanks to Zombie:Reloaded and Richard Helgeby for explaining this to me ages ago */
bool:IsPointInLocation(Float:point[3], Float:min[3], Float:max[3])
{
	// Cache to avoid re-indexing arrays.
	new Float:posX = point[0];
	new Float:posY = point[1];
	new Float:posZ = point[2];
	// Check if within x boundaries.
	if ((posX >= min[0]) && (posX <= max[0]))
	{
		// Check if within y boundaries.
		if ((posY >= min[1]) && (posY <= max[1]))
		{
			// Check if within x boundaries.
			if ((posZ >= min[2]) && (posZ <= max[2]))
			{
				// The point is within the location boundaries.
				return true;
			}
		}
	}
	
	// The point is outside the location boundaries.
	return false;
}

/* HLX:CE Support - Thanks Psychonic! */
stock GetTeams(bool:insmod = false)
{
	if (!insmod)
	{
		new max_teams_count = GetTeamCount();
		for (new team_index = 0; (team_index < max_teams_count); team_index++)
		{
			decl String: team_name[64];
			GetTeamName(team_index, team_name, sizeof(team_name));

			if (strcmp(team_name, "") != 0)
			{
				g_team_list[team_index] = team_name;
			}
		}
	}
	else
	{
		// they really need to get their act together... GetTeamName() would be awesome since they can't even keep their team indexes consistent
		decl String:mapname[64];
		GetCurrentMap(mapname, sizeof(mapname));
		if (strcmp(mapname, "ins_karam") == 0 || strcmp(mapname, "ins_baghdad") == 0)
		{
			g_team_list[1] = "Iraqi Insurgents";
			g_team_list[2] = "U.S. Marines";
		}
		else
		{
			g_team_list[1] = "U.S. Marines";
			g_team_list[2] = "Iraqi Insurgents";
		}
		g_team_list[0] = "Unassigned";
		g_team_list[3] = "SPECTATOR";
	}
}

stock LogPlayerEvent(client, const String:verb[], const String:event[], bool:display_location = false, const String:properties[] = "")
{
	if (IsValidPlayer(client))
	{
		decl String:player_authid[32];
		if (!GetClientAuthString(client, player_authid, sizeof(player_authid)))
		{
			strcopy(player_authid, sizeof(player_authid), "UNKNOWN");
		}

		if (display_location)
		{
			decl Float:player_origin[3];
			GetClientAbsOrigin(client, player_origin);
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\"%s (position \"%d %d %d\")", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, properties, RoundFloat(player_origin[0]), RoundFloat(player_origin[1]), RoundFloat(player_origin[2])); 
		}
		else
		{
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\"%s", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, properties); 
		}
	}
}

stock LogPlyrPlyrEvent(client, victim, const String:verb[], const String:event[], bool:display_location = false, const String:properties[] = "")
{
	if (IsValidPlayer(client) && IsValidPlayer(victim))
	{
		decl String:player_authid[32];
		if (!GetClientAuthString(client, player_authid, sizeof(player_authid)))
		{
			strcopy(player_authid, sizeof(player_authid), "UNKNOWN");
		}
		decl String:victim_authid[32];
		if (!GetClientAuthString(victim, victim_authid, sizeof(victim_authid)))
		{
			strcopy(victim_authid, sizeof(victim_authid), "UNKNOWN");
		}
		
		if (display_location)
		{
			decl Float:player_origin[3];
			GetClientAbsOrigin(client, player_origin);
			
			decl Float:victim_origin[3];
			GetClientAbsOrigin(victim, victim_origin);
			
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\" against \"%N<%d><%s><%s>\"%s (position \"%d %d %d\") (victim_position \"%d %d %d\")", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, victim, GetClientUserId(victim), victim_authid, g_team_list[GetClientTeam(victim)], properties, RoundFloat(player_origin[0]), RoundFloat(player_origin[1]), RoundFloat(player_origin[2]), RoundFloat(victim_origin[0]), RoundFloat(victim_origin[1]), RoundFloat(victim_origin[2])); 
		}
		else
		{
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\" against \"%N<%d><%s><%s>\"%s", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, victim, GetClientUserId(victim), victim_authid, g_team_list[GetClientTeam(victim)], properties); 
		}
	}
}

stock IsValidPlayer(client)
{
	if (client > 0 && client <= MaxClients && IsClientInGame(client))
	{
		return true;
	}
	return false;
}