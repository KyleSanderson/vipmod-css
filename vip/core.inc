#pragma semicolon 1

public ClearVIPCoordinates()
{
	SetConVarString(g_hConVar[CVAR_VIPZONE1],		"0");
	SetConVarString(g_hConVar[CVAR_VIPZONE2],		"0");
	SetConVarString(g_hConVar[CVAR_VIPTELEPORT],	"0");
	for(new i; i <= 2; i++)
	{
		g_fVIPResQ1Min[i]	= 0.0;
		g_fVIPResQ1Max[i]	= 0.0;
		g_fVIPResQ2Min[i]	= 0.0;
		g_fVIPResQ2Max[i]	= 0.0;
		g_fVIPResQdTele[i]	= 0.0;
	}
}

public GetVIPCoords(Status)
{
	switch(Status)
	{
		case 0:
		{
			g_hConVar[CVAR_VIPZONE1] = FindConVar("vip_escapezone");
			if(g_hConVar[CVAR_VIPZONE1] != INVALID_HANDLE)
			{
				decl String:Coords1[256], String:VIPCoordsArray1[6][8];
				GetConVarString(g_hConVar[CVAR_VIPZONE1], Coords1, sizeof(Coords1));
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
		}
		
		case 1:
		{
			g_hConVar[CVAR_VIPZONE2] = FindConVar("vip_escapezone2");
			if(g_hConVar[CVAR_VIPZONE2] != INVALID_HANDLE)
			{
				decl String:Coords2[256], String:VIPCoordsArray2[6][8];
				GetConVarString(g_hConVar[CVAR_VIPZONE2], Coords2, sizeof(Coords2));
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
		}
		
		case 2:
		{
			g_hConVar[CVAR_VIPTELEPORT] = FindConVar("vip_teleport");
			if(g_hConVar[CVAR_VIPTELEPORT] != INVALID_HANDLE)
			{
				decl String:CoordsTele[256], String:VIPCoordsTeleArray[3][8];
				GetConVarString(g_hConVar[CVAR_VIPTELEPORT], CoordsTele, sizeof(CoordsTele));
				ReplaceString(CoordsTele, sizeof(CoordsTele), "  ", " ");
				ExplodeString(CoordsTele, " ", VIPCoordsTeleArray, sizeof(VIPCoordsTeleArray), sizeof(VIPCoordsTeleArray[]));
				for(new i; i <= 2; i++)
				{
					g_fVIPResQdTele[i] = StringToFloat(VIPCoordsTeleArray[i]);
				}
			}
		}
	}
}

public bool:DoWeHaveTheCoords()
{
	if((g_fVIPResQ1Min[0] == 0) && (g_fVIPResQ2Min[0] == 0) && (g_fVIPResQ1Min[1] == 0) && (g_fVIPResQ1Max[2] == 0)) // Has the possibility of failing, but is pretty damn hard to.
	{
		if(g_bVocalize)
		{
			PrintToChatAll("\x04[VIPMod]\x03 Sorry, I was unable to get any rescue coordinates for this map. Bug?\nIf there's a new method of doing this, inform the author.");
			PrintToServer("[VIPMod] Sorry, I was unable to get any rescue coordinates for this map. Bug?\nIf there's a new method of doing this, inform the author.");
		}
		return false;
	}
	
	return true;
}

public FireRoundEnd(Status, Attacker)
{
	switch(Status)
	{
		case 0: // Disconnection / Team Change.
		{
			LogPlayerEvent(g_iVIPIndex, "triggered", "VIP_Killed"); // HLX:CE Support
			SDKCall(g_hTerminateRound, g_fRoundEndFloat, 2);		// The VIP has been assassinated!
			SetCash(2);
		}
		
		case 1:
		{
			SDKCall(g_hTerminateRound, g_fRoundEndFloat, 1);		// The VIP has escaped!
			LogPlayerEvent(g_iVIPIndex, "triggered", "VIP_Escaped"); // HLX:CE Logging Support
			SetEntProp(g_iVIPIndex, Prop_Data, "m_MoveType", MOVETYPE_NONE);
			SetCash(3);
		}
		
		case 2:
		{
			SDKCall(g_hTerminateRound, g_fRoundEndFloat, 14);	// The VIP has not escaped!
			LogPlayerEvent(0, "triggered", "VIP_NotEscaped"); // HLX:CE Logging Support - There's no support for this from Source? o.O
			SetCash(2);
		}
		
		case 3:
		{
			SDKCall(g_hTerminateRound, g_fRoundEndFloat, 2);		// The VIP has been assassinated!
			SetCash(2);
			LogPlyrPlyrEvent(Attacker, g_iVIPIndex, "triggered", "VIP_Killed"); // HLX:CE Logging Support
		}
	}
}

public bool:FindNewVIP(Status)
{
	switch(Status)
	{
		case 0: 
		{
			SDKHooks(0);
			SetVIPIndex(-1);
		}
		
		case 1:
		{
			switch(GetActualTeamCount(3))
			{
				case 0:
				{
					SetVIPIndex(-1);
					if(GetActualTeamCount(2) >= 2)
					{
						if(g_bVocalize)
						{
							PrintToChatAll("\x04[VIPMod]\x03 Come on guys, one of you needs to be on the Counter-Terrorist Team.");
							PrintToServer("VIPMod] Come on guys, one of you needs to be on the Counter-Terrorist Team.");
						}
					}
					return false;
				}
				
				case 1:
				{
					for (new i = 1; i <= MaxClients; i++)
					{
						if(IsValidClient(i, 0) && GetClientTeam(i) == 3)
						{
							SetVIPIndex(i);
							CreateVIPPlayer(g_iVIPIndex);
							break;
						}
					}
					return true;
				}
					
				default:
				{
					new VIPChanceArray[MAXPLAYERS], PlayerCount;
					new LastVIP = GetClientOfUserId(g_iLastVIPUserId);
					
					for (new i = 1; i <= MaxClients; i++)
					{
						if(i != LastVIP && IsValidClient(i, 0)  && GetClientTeam(i) == 3)
						{
							VIPChanceArray[PlayerCount] = i;
							PlayerCount++;
						}
					}
					
					SetVIPIndex(VIPChanceArray[GetRandomInt(0, PlayerCount-1)]);
					CreateVIPPlayer(g_iVIPIndex);
					return true;
				}
			}
		}
	}
	return false; // Cannot happen unless if someone edits the plugin, and does a silly job.
}

public SetupVersionString()
{
	FormatEx(g_sVIPVersionString, sizeof(g_sVIPVersionString), "VIPMod %s", PLUGIN_VERSION);
}

/* Timer Methodology. Psychonic said there's a better way with brushes? */
public TimerStuff(Status)
{
	switch(Status)
	{
		case 0:
		{
			g_hVIPTimerHandle	= INVALID_HANDLE;
			g_hRoundTimer		= INVALID_HANDLE;
		}
		
		case 1:
		{
			if(g_hVIPTimerHandle != INVALID_HANDLE)
			{
				KillTimer(g_hVIPTimerHandle);
			}
			
			g_hVIPTimerHandle = CreateTimer(0.4, CheckVIPPosition, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
		}
		
		case 2:
		{
			if(g_hRoundTimer != INVALID_HANDLE)
			{
				KillTimer(g_hRoundTimer);
			}
			
			g_hRoundTimer = CreateTimer(g_fRoundEndDuration, TimerFireRoundEnd, _, TIMER_FLAG_NO_MAPCHANGE);
		}
	}
}

public Action:TimerFireRoundEnd(Handle:Timer)
{
	g_hRoundTimer = INVALID_HANDLE;
	if(!g_bRoundEnd)
	{
		FireRoundEnd(2, 0);
	}
}

public Action:CheckVIPPosition(Handle:Timer)
{
	if(g_iVIPIndex < 1)
	{
		KillTimer(g_hVIPTimerHandle);
		g_hVIPTimerHandle = INVALID_HANDLE;
		return Plugin_Handled;
	}

	if(!g_bRoundEnd)
	{
		decl Float:VIPPos[3];
		GetClientAbsOrigin(g_iVIPIndex, VIPPos);
		if(IsPointInLocation(VIPPos, g_fVIPResQ1Min, g_fVIPResQ1Max) || IsPointInLocation(VIPPos, g_fVIPResQ2Min, g_fVIPResQ2Max))
		{
			if(g_fVIPResQdTele[0] != 0 && g_fVIPResQdTele[1] != 0 && g_fVIPResQdTele[2] != 0)
			{
				TeleportEntity(g_iVIPIndex, g_fVIPResQdTele, NULL_VECTOR, NULL_VECTOR);
			}
			
			OnVIPEscape();
		}
		return Plugin_Handled;
	}
		
	KillTimer(g_hVIPTimerHandle);
	g_hVIPTimerHandle = INVALID_HANDLE;
	
	return Plugin_Continue;
}