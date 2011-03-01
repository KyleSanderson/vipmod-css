#pragma semicolon 1

public RegisterCVars()
{
	new Handle:RoundEndDelayHandle	=	FindConVar("mp_round_restart_delay");
	new Handle:RoundTimeHandle 		=	FindConVar("mp_roundtime");
	
	CreateConVar("vip_gamemode_version", PLUGIN_VERSION, PLUGIN_DESCRIPTION, FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY); // Some Jerk took vip_version :< 

	HookConVarChange(CreateConVar("vip_forceload",			"0",								"Enables forced loading of VIPMod."),				ForceLoadChanged);
	HookConVarChange(CreateConVar("vip_gamedescription",	"1",								"Enables the changing of SRCDS's gamedescription."),GameDescChanged);
	HookConVarChange(CreateConVar("vip_teleport",			"0 0 0",							"VIP after Escape Teleport."),						TeleportChanged);
	HookConVarChange(CreateConVar("vip_escapezone",			"0 0 0 0 0 0",						"VIP Placement Zone."),								VIPZone1Changed);
	HookConVarChange(CreateConVar("vip_escapezone2",		"0 0 0 0 0 0",						"VIP Placement Zone 2."),							VIPZone2Changed);
	HookConVarChange(CreateConVar("vip_vipmodel",			"models/player/vip/small2/vip.mdl",	"VIP Model Path."),									ModelPathChanged);
	HookConVarChange(CreateConVar("vip_vocalize",			"1",								"Should the plugin be Verbal?"), 					VocalizeChanged);
	HookConVarChange(CreateConVar("vip_vipweapon",			"weapon_usp",						"Weapon given to the VIP"),							VIPWeaponChanged);
	
	HookConVarChange(RoundEndDelayHandle,														RoundEndDelayChanged);
	HookConVarChange(RoundTimeHandle,															RoundTimeChanged);
	
	g_fRoundEndDelay	=	GetConVarFloat(RoundEndDelayHandle);
	g_fRoundDuration	=	GetConVarFloat(RoundTimeHandle) * 60;
	CloseHandle(RoundEndDelayHandle);
	CloseHandle(RoundTimeHandle);
}

public ForceLoadChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_bForceLoad = GetConVarBool(convar);
}

public GameDescChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_bChangeGameDesc = GetConVarBool(convar);
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
	strcopy(g_sVIPTeleportZone, sizeof(g_sVIPTeleportZone), newValue);
	GetVIPCoords(2);
}

public VIPZone1Changed(Handle:convar, const String:oldValue[], const String:newValue[])
{
	strcopy(g_sVIPEscapeZone1, sizeof(g_sVIPEscapeZone1), newValue);
	g_bEscZone1Coords = false;
	GetVIPCoords(0);
	
	if(g_bModIsEnabled && !DoWeHaveTheCoords())
	{
		g_bCoordsFound = false;
	}
}

public VIPZone2Changed(Handle:convar, const String:oldValue[], const String:newValue[])
{
	strcopy(g_sVIPEscapeZone2, sizeof(g_sVIPEscapeZone2), newValue);
	g_bEscZone2Coords = false;
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

public RoundEndDelayChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_fRoundEndDelay = StringToFloat(newValue);
}

public RoundTimeChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_fRoundDuration = StringToFloat(newValue) * 60;
}

public VocalizeChanged(Handle:convar, const String:oldValue[], const String:newValue[])
{
	g_bVocalize = GetConVarBool(convar);
}