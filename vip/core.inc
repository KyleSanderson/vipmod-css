#pragma semicolon 1

public ClearVIPCoordinates()
{
	SetConVarString(FindConVar("vip_escapezone"),		"0 0 0 0 0 0");
	SetConVarString(FindConVar("vip_escapezone2"),		"0 0 0 0 0 0");
	SetConVarString(FindConVar("vip_teleport"),			"0 0 0");
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
			if(!g_bEscZone1Coords && StrContains(g_sVIPEscapeZone1, "0 0 0", false) == -1 && StrContains(g_sVIPEscapeZone1, ",", false) == -1)
			{
				decl String:VIPCoordsArray1[6][8];
				ReplaceString(g_sVIPEscapeZone1, sizeof(g_sVIPEscapeZone1), "  ", " ");
				ExplodeString(g_sVIPEscapeZone1, " ", VIPCoordsArray1, sizeof(VIPCoordsArray1), sizeof(VIPCoordsArray1[]));
				for(new i; i < 6; i++)
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
				g_bEscZone1Coords = true;
			}
		}
		
		case 1:
		{
			decl String:VIPCoordsArray2[6][8];
			if(!g_bEscZone2Coords && StrContains(g_sVIPEscapeZone2, "0 0 0", false) == -1 && StrContains(g_sVIPEscapeZone2, ",", false) == -1)
			{
				ReplaceString(g_sVIPEscapeZone2, sizeof(g_sVIPEscapeZone2), "  ", " ");
				ExplodeString(g_sVIPEscapeZone2, " ", VIPCoordsArray2, sizeof(VIPCoordsArray2), sizeof(VIPCoordsArray2[]));
				for(new i; i < 6; i++)
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
				g_bEscZone2Coords = true;
			}
		}
		
		case 2:
		{
			if(StrContains(g_sVIPTeleportZone, "0 0 0", false) == -1 && StrContains(g_sVIPTeleportZone, ",", false) == -1)
			{
				decl String:VIPCoordsTeleArray[3][8];
				ReplaceString(g_sVIPTeleportZone, sizeof(g_sVIPTeleportZone), "  ", " ");
				ExplodeString(g_sVIPTeleportZone, " ", VIPCoordsTeleArray, sizeof(VIPCoordsTeleArray), sizeof(VIPCoordsTeleArray[]));
				for(new i; i < 3; i++)
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
	g_bRoundEnd = true; // Thanks GoD-Tony!
	switch(Status)
	{
		case 0: // Disconnection / Team Change.
		{
			SDKCall(g_hTerminateRound, g_fRoundEndDelay, 2);		// The VIP has been assassinated!
			LogPlayerEvent(g_iVIPIndex, "triggered", "VIP_Killed"); // HLX:CE Support
			SetTeamScore(2, GetTeamScore(2)+1);
			SetScore(2);
			PlayTeamSound(2);
			SetCash(2);
		}
		
		case 1:
		{
			SDKCall(g_hTerminateRound, g_fRoundEndDelay, 1);		// The VIP has escaped!
			LogPlayerEvent(g_iVIPIndex, "triggered", "VIP_Escaped"); // HLX:CE Logging Support
			SetEntProp(g_iVIPIndex, Prop_Data, "m_MoveType", MOVETYPE_NONE);
			SetTeamScore(3, GetTeamScore(3)+1);
			SetScore(3);
			PlayTeamSound(3);
			SetCash(3);
		}
		
		case 2:
		{
			SDKCall(g_hTerminateRound, g_fRoundEndDelay, 14);	// The VIP has not escaped!
			LogPlayerEvent(0, "triggered", "VIP_NotEscaped"); // HLX:CE Logging Support - There's no support for this from Source? o.O
			SetTeamScore(2, GetTeamScore(2)+1);
			SetScore(2);
			PlayTeamSound(2);
			SetCash(2);
		}
		
		case 3:
		{
			SDKCall(g_hTerminateRound, g_fRoundEndDelay, 2);		// The VIP has been assassinated!
			LogPlyrPlyrEvent(Attacker, g_iVIPIndex, "triggered", "VIP_Killed"); // HLX:CE Logging Support
			SetTeamScore(2, GetTeamScore(2)+1);
			SetScore(2);
			PlayTeamSound(2);
			SetCash(2);
		}
	}
}

public bool:FindNewVIP(Status)
{
	switch(Status)
	{
		case 0: 
		{
			if(g_iVIPIndex > 0 && IsValidClient(g_iVIPIndex, 0))
			{
				FixOldVIPArmour();
				SDKHooks(2);
				SDKHooks(4);
			}

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
							PrintToChatAll("\x04[VIPMod]\x03 Come on guys, one of you needs to be on the \x04Counter-Terrorist\x03 Team.");
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
						if(i != LastVIP && IsValidClient(i, 0) && GetClientTeam(i) == 3)
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
			g_hUMTimerHandle	= INVALID_HANDLE;
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
			
			g_hRoundTimer = CreateTimer(g_fRoundDuration, TimerFireRoundEnd, _, TIMER_FLAG_NO_MAPCHANGE);
		}
	}
}

public Action:TimerFireRoundEnd(Handle:Timer)
{
	g_hRoundTimer = INVALID_HANDLE;
	if(!g_bRoundEnd && 0 < g_iVIPIndex)
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
		if((g_bEscZone1Coords && IsPointInLocation(VIPPos, g_fVIPResQ1Min, g_fVIPResQ1Max)) || (g_bEscZone2Coords && IsPointInLocation(VIPPos, g_fVIPResQ2Min, g_fVIPResQ2Max)))
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