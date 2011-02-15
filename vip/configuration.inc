#pragma semicolon 1

public RegisterCVars()
{
	CreateConVar("vip_gamemode_version", PLUGIN_VERSION, PLUGIN_DESCRIPTION, FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY); // Some Jerk took vip_version :< 

	g_hConVar[CVAR_FORCELOAD]		=	CreateConVar("vip_forceload",		"0",								"Enables forced loading of VIPMod.");
	g_hConVar[CVAR_GAME_DESCRIPTION]=	CreateConVar("vip_gamedescription",	"1",								"Enables the changing of SRCDS's gamedescription.");
	g_hConVar[CVAR_VIPTELEPORT]		=	CreateConVar("vip_teleport",		"0, 0, 0, 0, 0, 0",					"VIP after Escape Teleport.");
	g_hConVar[CVAR_VIPZONE1]		=	CreateConVar("vip_escapezone",		"0, 0, 0, 0, 0, 0",					"VIP Placement Zone.");
	g_hConVar[CVAR_VIPZONE2]		=	CreateConVar("vip_escapezone2",		"0, 0, 0, 0, 0, 0",					"VIP Placement Zone 2.");
	g_hConVar[CVAR_VIPMODEL_PATH]	=	CreateConVar("vip_vipmodel",		"models/player/vip/small2/vip.mdl",	"VIP Model Path.");
	g_hConVar[CVAR_VIPVOCAL]		=	CreateConVar("vip_vocalize",		"1",								"Chooses if you want the plugin to be silent, or to be talkative");
	g_hConVar[CVAR_VIPWEAPON]		=	CreateConVar("vip_vipweapon",		"weapon_usp",						"Weapon given to the VIP");

	g_hMPFreezeTime		=	FindConVar("mp_round_restart_delay");
	g_fRoundEndFloat	=	GetConVarFloat(g_hMPFreezeTime);
	g_hRoundTime 		=	FindConVar("mp_roundtime");
	g_fRoundEndDuration =	GetConVarFloat(g_hRoundTime) * 60;
	
	GetConVarString(g_hConVar[CVAR_VIPMODEL_PATH], g_sVIPModelPath, sizeof(g_sVIPModelPath));
	GetConVarString(g_hConVar[CVAR_VIPWEAPON], g_sVIPWeaponName, sizeof(g_sVIPWeaponName));
	
	HookConVarChange(g_hConVar[CVAR_FORCELOAD],			ForceLoadChanged);
	HookConVarChange(g_hConVar[CVAR_GAME_DESCRIPTION],	GameDescChanged);
	HookConVarChange(g_hConVar[CVAR_VIPMODEL_PATH],		ModelPathChanged);
	HookConVarChange(g_hConVar[CVAR_VIPTELEPORT],		TeleportChanged);
	HookConVarChange(g_hConVar[CVAR_VIPZONE1],			VIPZone1Changed);
	HookConVarChange(g_hConVar[CVAR_VIPZONE2],			VIPZone2Changed);
	HookConVarChange(g_hConVar[CVAR_VIPWEAPON],			VIPWeaponChanged);
	HookConVarChange(g_hMPFreezeTime,					MPFreezeChanged);
	HookConVarChange(g_hRoundTime,						RoundTimeChanged);
}

public ForceLoadChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_bForceLoad = GetConVarBool(g_hConVar[CVAR_FORCELOAD]);
}

public GameDescChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_bChangeGameDesc = GetConVarBool(g_hConVar[CVAR_GAME_DESCRIPTION]);
}

public ModelPathChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	strcopy(g_sVIPModelPath, sizeof(g_sVIPModelPath), newValue);
	
	if(g_bModIsEnabled && g_bVocalize)
	{
		PrintToChatAll("\x04[VIPMod]\x03 VIP Model path changed, you need to change the map for it to take effect (properly).");
		PrintToServer("[VIPMod] VIP Model path changed, you need to change the map for it to take effect (properly).");
	}
}

public TeleportChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	GetVIPCoords(2);
}

public VIPZone1Changed(Handle:convar, const String:oldValue[], const String:newValue[])
{
	GetVIPCoords(0);
	
	if(g_bModIsEnabled && !DoWeHaveTheCoords())
	{
		g_bCoordsFound = false;
	}
}

public VIPZone2Changed(Handle:convar, const String:oldValue[], const String:newValue[])
{
	GetVIPCoords(1);

	if(g_bModIsEnabled && !DoWeHaveTheCoords())
	{
		g_bCoordsFound = false;
	}
}

public VIPWeaponChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	strcopy(g_sVIPWeaponName, sizeof(g_sVIPWeaponName), newValue);
}

public MPFreezeChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_fRoundEndFloat = StringToFloat(newValue);
}

public RoundTimeChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_fRoundEndDuration = StringToFloat(newValue) * 60;
}