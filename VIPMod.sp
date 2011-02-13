#pragma semicolon 1

/* Includes */
#include <sourcemod>
#include <sdkhooks>
#include <sdktools>

/* Defines */
#define PLUGIN_VERSION			"1.2.0"
#define PLUGIN_DESCRIPTION		"Creates VIP style gameplay on AS_ maps. Protect the VIP, Team :3"
#define StartMeUp 1				/* Just to make it easier for people to read this */
#define KillMeNow 0				/* Just to make it easier for people to read this */

/* Core Additions (Required) */
#include "vip/cache"
#include "vip/core"
#include "vip/configuration"
#include "vip/downloads" // Huge thanks to the ever elusive, Error Throwing, WebShortcuts.
#include "vip/hlxce"
#include "vip/hooks"
#include "vip/sdkhooks"
#include "vip/stocks"

public Plugin:myinfo =
{
    name 		=		"VIPMod",			// ReWrite: http://www.youtube.com/watch?v=-qPPb5B5LWE&hd=1 || Original: http://www.thesixtyone.com/s/KkzxBiqbs4C/
    author		=		"Kyle Sanderson", 
    description	=		 PLUGIN_DESCRIPTION, 
    version		=		 PLUGIN_VERSION, 
    url			=		"http://SourceMod.net"
};

public OnPluginStart()
{
	CreateConVar("vip_gamemode_version", PLUGIN_VERSION, PLUGIN_DESCRIPTION, FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY); // Some Jerk took vip_version :< 
}

public OnConfigsExecuted()
{
	if(CanLoad())
	{
		VIPMod_Start();
	}
}

public VIPMod_Start() // Lets Fire it up.
{
	if(!g_bHasBeenLoaded)
	{
		RegisterCVars();
		SetupMoney();
		SetupTerminateRound();
		SetupVersionString();
		g_bHasBeenLoaded = true;
	}
	
	GetVIPCoords(0);
	GetVIPCoords(1);
	GetVIPCoords(2);
	AddCommandListeners(StartMeUp);
	EventHooks(StartMeUp);
	FireUpConfigFiles();
	UserMessages(StartMeUp);
	
	g_bModIsEnabled = true;
}

public OnRoundStart()
{
	if(!g_bCoordsFound)
	{
		GetVIPCoords(0);
		GetVIPCoords(1);
		GetVIPCoords(2);
		if(!DoWeHaveTheCoords())
		{
			return;
		}
		g_bCoordsFound = true;
	}
	
	/* Unload Proceedure */
	SDKHooks(2);
	SDKHooks(4);
	FindNewVIP(KillMeNow);
	
	/* Lets restart the process shall we? */
	if(FindNewVIP(StartMeUp))
	{
		SDKHooks(1);
		SDKHooks(3);
		TimerStuff(StartMeUp);
	}
		
	g_bRoundEnd = false;
}

public OnRoundStartFreezeEnd() // When FreezeTime is up, bad name eh? lol :p
{
	TimerStuff(2);
}

public OnPlayerDeath(Killed, Killer)
{
	if(Killed == g_iVIPIndex)
	{
		FireRoundEnd(KillMeNow, Killer);
	}
}

public OnRoundEnd()
{
	g_bRoundEnd = true;
	FireRoundEnd(2, 0);
}

public OnVIPEscape()
{
	SDKHooks(5);
	FireRoundEnd(1, 0);
}

public OnPlayerSwapTeam(client, Team)
{
	if(client == g_iVIPIndex)
	{
		decl String:TeamName[32];
		GetTeamName(Team, TeamName, sizeof(TeamName));
		PrintToChatAll("\x04[VIPMod]\x03 The VIP (\x04%N\x03) decided to swap to the \x04%s\x03 Team.\nHow lame is that? \x04:/", client, TeamName);
		PrintToServer("[VIPMod] The VIP (%N) decided to swap to the %s Team. How lame is that? :/", client, TeamName);
		FireRoundEnd(KillMeNow, 0);
	}
}

public OnClientDisconnect(client)
{
	if(client == g_iVIPIndex)
	{
		PrintToChatAll("\x04[VIPMod]\x03 The VIP (\x04%N\x03) has disconnected as the VIP.", client);
		PrintToServer("[VIPMod] The VIP (%N) has disconnected as the VIP.", client);
		FireRoundEnd(KillMeNow, 0);
	}
}

public OnMapEnd()
{
	if(g_bModIsEnabled)
	{
		ClearVIPCoordinates();
		AddCommandListeners(KillMeNow);
		EventHooks(KillMeNow);
		SDKHooks(KillMeNow);
		TimerStuff(KillMeNow);
		FindNewVIP(KillMeNow);
		UserMessages(KillMeNow);
		
		g_bCoordsFound = false;
		g_bModIsEnabled = false;
	}
}