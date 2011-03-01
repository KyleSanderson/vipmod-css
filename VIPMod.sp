#pragma semicolon 1

/* Includes */
#include <sourcemod>
#include <sdkhooks>
#include <sdktools>

/* Defines */
#define PLUGIN_VERSION			"1.2.3"
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
	RegisterCVars(); 
	SetupMoney();
	SetupTerminateRound();
	SetupVersionString();
	AutoExecConfig(true, "vipmod"); // http://forums.alliedmods.net/showpost.php?p=1414583&postcount=17
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
	GetVIPCoords(0);
	GetVIPCoords(1);
	GetVIPCoords(2);
	AddCommandListeners(StartMeUp);
	EventHooks(StartMeUp);
	SDKHooks(StartMeUp);
	UserMessages(StartMeUp);
	FireUpConfigFiles();

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
	FindNewVIP(KillMeNow);
	
	/* Lets restart the process shall we? */
	if(FindNewVIP(StartMeUp))
	{
		SDKHooks(3);
		TimerStuff(StartMeUp);
	}
	
	g_bUserMessageEnabled = true;
	g_bRoundEnd = false;
}

public OnRoundStartFreezeEnd() // When FreezeTime is up, bad name eh? lol :p
{
	PlayTeamSound(StartMeUp);
	TimerStuff(2);
}

public OnPlayerDeath(Killed, Killer)
{
	if(Killed == g_iVIPIndex && !g_bRoundEnd)
	{
		FireRoundEnd(KillMeNow, Killer);
	}
}

public OnRoundEnd()
{
	FireRoundEnd(2, 0);
}

public OnVIPEscape()
{
	SDKHooks(5);
	FireRoundEnd(1, 0);
}

public OnPlayerSwapTeam(client, Team)
{
	if(client == g_iVIPIndex && !g_bRoundEnd)
	{
		if(g_bVocalize)
		{
			PrintToChatAll("\x04[VIPMod]\x03 The VIP (\x04%N\x03) decided to swap to the \x04%s\x03 Team.\nHow lame is that? \x04:/", client, GetProperTeamName(Team));
			PrintToServer("[VIPMod] The VIP (%N) decided to swap to the %s Team.\nHow lame is that? :/", client, GetProperTeamName(Team));
		}

		FireRoundEnd(KillMeNow, 0);
	}
}

public OnClientDisconnect(client)
{
	if(client == g_iVIPIndex && !g_bRoundEnd)
	{
		if(g_bVocalize)
		{
			PrintToChatAll("\x04[VIPMod]\x03 The VIP (\x04%N\x03) has disconnected.", client);
			PrintToServer("[VIPMod] The VIP (%N) has disconnected.", client);
		}
		
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

public Action:UnhookUserMessageTimer(Handle:Timer)
{
	UserMessages(KillMeNow);
	return Plugin_Handled; // This is stupid on a Timer.
}