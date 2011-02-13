#pragma semicolon 1

public EventHooks(Status) // Forwards for these would be nice. They're not game specific either. *Shrugs*
{
	switch(Status)
	{
		case 0:
		{
			UnhookEvent("player_death",		Event_OnPlayerDeath,			EventHookMode_Pre);
			UnhookEvent("player_team",		Event_OnPlayerTeam,				EventHookMode_Pre);
			UnhookEvent("round_start",		Event_OnRoundStart,				EventHookMode_PostNoCopy);
			UnhookEvent("round_end",		Event_OnRoundEnd,				EventHookMode_Pre);
			UnhookEvent("round_freeze_end",	Event_OnRoundStartFreezeEnd,	EventHookMode_Pre);
		}
		
		case 1:
		{
			HookEvent("player_death",		Event_OnPlayerDeath,			EventHookMode_Pre);
			HookEvent("player_team",		Event_OnPlayerTeam,				EventHookMode_Pre);
			HookEvent("round_start",		Event_OnRoundStart,				EventHookMode_PostNoCopy);
			HookEvent("round_end",			Event_OnRoundEnd,				EventHookMode_Pre);
			HookEvent("round_freeze_end",	Event_OnRoundStartFreezeEnd,	EventHookMode_Pre);
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
			UnhookUserMessage(GetUserMessageId("SendAudio"), SendAudioHook, true);
		}
	
		case 1:
		{
			HookUserMessage(GetUserMessageId("SendAudio"), SendAudioHook, true);
		}
	}
}

public Action:SendAudioHook(UserMsg:msg_id, Handle:bf, const players[], playersNum, bool:reliable, bool:init)
{
	decl String:Buffer[256];
	BfReadString(bf, Buffer, sizeof(Buffer), true);
	if(StrEqual("radio.moveout", Buffer, false) || StrEqual("radio.letsgo", Buffer, false) || StrEqual("radio.locknload", Buffer, false) || StrEqual("radio.go", Buffer, false))
	{
		return Plugin_Handled;
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
		OnRoundEnd();
		return Plugin_Continue;
	}
	return Plugin_Handled;
}

public Action:Event_OnRoundStartFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
	OnRoundStartFreezeEnd();
	ProtectTheVIPTeamSound();
	return Plugin_Handled;
}

public Action:RestrictBuyCommand(client, const String:command[], argc)
{
	if (client == g_iVIPIndex)
	{
		PrintToChat(client, "\x04[VIPMod]\x03 You can't buy anything as the VIP you silly goose.");
		return Plugin_Handled;
	}
	return Plugin_Continue;
}