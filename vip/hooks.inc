#pragma semicolon 1

public EventHooks(Status) // Forwards for these would be nice. They're not game specific either. *Shrugs*
{
	switch(Status)
	{
		case 0:
		{
			UnhookEvent("player_death",		Event_OnPlayerDeath,			EventHookMode_Pre);
			UnhookEvent("player_team",		Event_OnPlayerTeam,				EventHookMode_Post);
			UnhookEvent("round_start",		Event_OnRoundStart,				EventHookMode_PostNoCopy);
			UnhookEvent("round_end",		Event_OnRoundEnd,				EventHookMode_Pre);
			UnhookEvent("round_freeze_end",	Event_OnRoundStartFreezeEnd,	EventHookMode_PostNoCopy);
		}
		
		case 1:
		{
			HookEvent("player_death",		Event_OnPlayerDeath,			EventHookMode_Pre);
			HookEvent("player_team",		Event_OnPlayerTeam,				EventHookMode_Post);
			HookEvent("round_start",		Event_OnRoundStart,				EventHookMode_PostNoCopy);
			HookEvent("round_end",			Event_OnRoundEnd,				EventHookMode_Pre);
			HookEvent("round_freeze_end",	Event_OnRoundStartFreezeEnd,	EventHookMode_PostNoCopy);
		}
	}
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

public UserMessages(Status)
{
	switch(Status)
	{
		case 0:
		{
			
			g_hUMTimerHandle = INVALID_HANDLE;
			// UnhookUserMessage(GetUserMessageId("SendAudio"), SendAudioHook, true); Doesn't function? <javalia> DONT DO THAT
		}
	
		case 1:
		{
			HookUserMessage(GetUserMessageId("SendAudio"), SendAudioHook, true);
		}
	}
}

public Action:SendAudioHook(UserMsg:msg_id, Handle:bf, const players[], playersNum, bool:reliable, bool:init)
{
	if(g_bModIsEnabled && g_bUserMessageEnabled)
	{
		decl String:Buffer[256];
		BfReadString(bf, Buffer, sizeof(Buffer), true);
		if(StrEqual("radio.moveout", Buffer, false) || StrEqual("radio.letsgo", Buffer, false) || StrEqual("radio.locknload", Buffer, false) || StrEqual("radio.go", Buffer, false))
		{
			return Plugin_Handled;
		}
	}
	return Plugin_Continue;
}

public Event_OnPlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
	OnPlayerDeath(GetClientOfUserId(GetEventInt(event, "userid")), GetClientOfUserId(GetEventInt(event, "attacker")));
}

public Event_OnPlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
	OnPlayerSwapTeam(GetClientOfUserId(GetEventInt(event, "userid")), GetEventInt(event, "team"));
}

public Event_OnRoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
	OnRoundStart();
}

public Action:Event_OnRoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
	if(!g_bRoundEnd)
	{
		if(GetEventInt(event, "reason") == 7) // Thanks GoD-Tony!
		{
			g_bRoundEnd = true;
			return Plugin_Continue;
		}
		
		g_bRoundEnd = true;
		OnRoundEnd();
		return Plugin_Continue;
	}
	
	return Plugin_Handled;
}

public Event_OnRoundStartFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
	OnRoundStartFreezeEnd();
	if(g_hUMTimerHandle != INVALID_HANDLE)
	{
		KillTimer(g_hUMTimerHandle);
	}

	g_hUMTimerHandle = CreateTimer(2.0, UnhookUserMessageTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

public Action:RestrictBuyCommand(client, const String:command[], argc)
{
	if(client == g_iVIPIndex)
	{
		PrintToChat(client, "\x04[VIPMod]\x03 You can't buy anything as the VIP you silly goose.");
		return Plugin_Handled;
	}
	return Plugin_Continue;
}