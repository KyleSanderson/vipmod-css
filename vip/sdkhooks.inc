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
		
		case 2:
		{
			if(g_iVIPIndex > 0)
			SDKUnhook(g_iVIPIndex, SDKHook_WeaponCanUse, OnWeaponCanUse);
		}
		
		case 3:
		{
			if(g_iVIPIndex > 0)
			SDKHook(g_iVIPIndex, SDKHook_WeaponCanUse, OnWeaponCanUse);
		}
		
		case 4:
		{
			if(g_iVIPIndex > 0)
			SDKUnhook(g_iVIPIndex, SDKHook_OnTakeDamage, OnClientTakeDamage);
		}
		
		case 5:
		{
			if(g_iVIPIndex > 0)
			SDKHook(g_iVIPIndex, SDKHook_OnTakeDamage, OnClientTakeDamage);
		}
	}
}

public Action:OnWeaponCanUse(client, weapon)
{
	decl String:WeaponName[14];
	GetEdictClassname(weapon, WeaponName, sizeof(WeaponName));
	if(!StrEqual(WeaponName, "weapon_knife") && !StrEqual(WeaponName, g_sVIPWeaponName, false))
	{
		return Plugin_Handled;
	}
	return Plugin_Continue;
}

public CSPlayerManagerThinkPost(entity)
{
	if(g_iVIPIndex > 0)
	{
		SetEntProp(entity, Prop_Send, "m_iPlayerVIP", g_iVIPIndex);
	}
}

public Action:OnGetGameDescription(String:gameDesc[64])
{
	if(g_bChangeGameDesc && g_bModIsEnabled)
	{
		strcopy(gameDesc, sizeof(gameDesc), g_sVIPVersionString);
		return Plugin_Changed;
	}
	
	return Plugin_Continue;
}

public Action:OnClientTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype)
{
	return Plugin_Handled;
}