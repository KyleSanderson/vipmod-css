#pragma semicolon 1

stock bool:CanLoad()
{
	decl String:MapName[64];
	GetCurrentMap(MapName, sizeof(MapName));
	if(strncmp(MapName, "as_", 3) != 0 && !g_bForceLoad)
	{
		return false;
	}
	
	return true;
}

stock bool:SetVIPIndex(Index)
{
	g_iVIPIndex = Index;
	if(Index > 0)
	{
		g_iLastVIPUserId = GetClientUserId(Index);
		return true;
	}
	
	return false;
}

stock bool:IsNotLastVIP(Index)
{
	if(GetClientUserId(Index) == g_iLastVIPUserId)
	{
		return false;
	}
	
	return true;
}

stock GetActualTeamCount(Team)
{
	new Count;
	for(new i = 1; i <= MaxClients; i++)
	{
		if(IsValidClient(i, 0) && GetClientTeam(i) == Team) // See how easy this is with that stock?
		{
			Count++;
		}
	}
	
	return Count;
}

stock bool:IsValidClient(i, Status)
{
	switch(Status)
	{
		case 0:
		{
			if(i > 0 && IsClientConnected(i) && IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
			{
				return true;
			}
		}
		
		case 1:
		{
			if(i > 0 && IsClientConnected(i) && IsClientInGame(i) && !IsFakeClient(i))
			{
				return true;
			}
		}
	}
	
	return false;
}

stock SetupTerminateRound()
{
	g_hGameConfiguration = LoadGameConfigFile("plugin.VIPMod");
	StartPrepSDKCall(SDKCall_GameRules);
	PrepSDKCall_SetFromConf(g_hGameConfiguration, SDKConf_Signature, "TerminateRound");
	PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	g_hTerminateRound = EndPrepSDKCall();
}

stock SetupMoney()
{
	g_iMoneyPropInfo = FindSendPropInfo("CCSPlayer", "m_iAccount");
}

stock FixOldVIPArmour()
{
	if(g_iVIPIndex > 0 && IsValidClient(g_iVIPIndex, 0))
	{
		SetEntProp(g_iVIPIndex, Prop_Send, "m_ArmorValue", 100);
	}
}

/* Start of Score Work Around Shit.
stock WorkAroundSetScore(Team) // If SetTeamScore is fixed, remove and use that, duh!
{
	if(g_bScoreNotChanged)
	{
		return;
	}
	
	g_bScoreNotChanged = false;
	SetTeamScore(Team, GetTeamScore(Team)+1);
	switch (Team)
	{
		case 2:
		{
			ChangeEdictState(g_iTTeamManIndex, GetEntSendPropOffs(g_iTTeamManIndex, "m_iScore"));
		}
		
		case 3:
		{
			ChangeEdictState(g_iCTTeamManIndex, GetEntSendPropOffs(g_iCTTeamManIndex, "m_iScore"));
		}
	}
}

stock BerniScoreStock()
{
	new MaxEntities		=	GetMaxEntities();
	g_iTTeamManIndex	=	Team_GetEdict(2, MaxEntities);
	g_iCTTeamManIndex	=	Team_GetEdict(3, MaxEntities);
}

stock Team_GetEdict(Team, MaxEntities)
{
	decl String:EntityClassName[64];
	for (new Index=MaxClients+1; Index < MaxEntities; Index++)
	{
		if (IsValidEntity(Index))
		{
			GetEdictClassname(Index, EntityClassName, sizeof(EntityClassName));
			if(StrContains(EntityClassName, "team_manager", false))
			{
				if (GetEntProp(Index, Prop_Send, "m_iTeamNum") == Team)
				{
					PrintToServer("Found Team: %i at Edict: %i", Team, Index);
					return Index;
				}
			}
		}
	}
	PrintToServer("Couldn't find shit for %i", Team);
	return -1;
}
End of Score Work Around Shit. */

stock SetCash(Team)
{
	for(new i = 1; i <=MaxClients; i++)
	{
		if(IsValidClient(i, 1) && GetClientTeam(i) == Team)
		{
			switch(IsPlayerAlive(i))
			{
				case 0:
				{
					SetEntData(i, g_iMoneyPropInfo, GetEntData(i, g_iMoneyPropInfo, 4)+1500, 4);
				}

				case 1:
				{
					SetEntData(i, g_iMoneyPropInfo, GetEntData(i, g_iMoneyPropInfo, 4)+3000, 4);
				}
			}
		}
	}
}

stock CreateVIPPlayer(client)
{
	SetEntProp(client, Prop_Send, "m_iHealth", 150);
	SetEntProp(client, Prop_Send, "m_ArmorValue", 125);
	SetEntProp(client, Prop_Send, "m_bHasHelmet", 1);
	
	if(!IsModelPrecached(g_sVIPModelPath))
	{
		if(!FileExists(g_sVIPModelPath))
		{
			ThrowError("The model you've selected (%s) doesn't exist.", g_sVIPModelPath);
		}
		ThrowError("Well this is weird (%s) hasn't been precached.", g_sVIPModelPath);
	}
	
	AM_RemoveAllWeapons(client); // Crash?
	if(g_bVocalize)
	{
		PrintCenterText(client, "You are the VIP");
	}
	SetEntityModel(client, g_sVIPModelPath);
}

stock AM_RemoveAllWeapons(client)
{
	decl String:WeaponName[32];
	new weaponIndex;
	for (new i = 0; i <= 5; i++)
	{
		weaponIndex = 0;
		while ((weaponIndex = GetPlayerWeaponSlot(client, i)) != -1)
		{
			GetEdictClassname(weaponIndex, WeaponName, sizeof(WeaponName));
			if(StrEqual(WeaponName, "weapon_knife"))
			{
				break;
			}
			
			RemovePlayerItem(client, weaponIndex);
			AcceptEntityInput(weaponIndex, "Kill");
		}
	}
	
	GivePlayerItem(client, g_sVIPWeaponName); // Work Around for Jesus Mode.
}

stock ProtectTheVIPTeamSound()
{
	decl String:InitialRadioSound[24];
	FormatEx(InitialRadioSound, sizeof(InitialRadioSound), g_sInitialRadio[GetRandomInt(0,3)]);
	for(new i = 1; i <= MaxClients; i++)
	{
		if(IsValidClient(i, 0))
		{
			switch(GetClientTeam(i))
			{
				case 2:
				{
					EmitSoundToClient(i, InitialRadioSound);
				}
				
				case 3:
				{
					EmitSoundToClient(i, "radio/vip.wav");
				}
			}
			
		}
		
	}
}

stock String:GetProperTeamName(Team)
{
	decl String:TeamName[20];
	switch(Team)
	{
		case 0:
		{
			FormatEx(TeamName, sizeof(TeamName), "Unassigned");
			return TeamName;
		}
		case 1:
		{
			FormatEx(TeamName, sizeof(TeamName), "Spectator");
			return TeamName;
		}
		case 2:
		{
			FormatEx(TeamName, sizeof(TeamName), "Terrorist");
			return TeamName;
		}
		case 3:
		{
			FormatEx(TeamName, sizeof(TeamName), "Counter-Terrorist");
			return TeamName;
		}
	}
	return TeamName; // Cannot Happen.
}

/* Huge Thanks to Zombie:Reloaded and Richard Helgeby for explaining this to me ages ago */
stock bool:IsPointInLocation(Float:point[3], Float:min[3], Float:max[3])
{
	// Cache to avoid re-indexing arrays.
	new Float:posX = point[0];
	new Float:posY = point[1];
	new Float:posZ = point[2];
	// Check if within x boundaries.
	if((posX >= min[0]) && (posX <= max[0]) && (posY >= min[1]) && (posY <= max[1]) && (posZ >= min[2]) && (posZ <= max[2]))
	{
		// The point is within the location boundaries.
		return true;
	}
	
	// The point is outside the location boundaries.
	return false;
}