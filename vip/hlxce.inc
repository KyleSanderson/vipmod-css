#pragma semicolon 1
/* HLX:CE Support - Thanks Psychonic! */
new String:g_team_list[4][64]; // HLX:CE Support

stock GetTeams()
{
	new max_teams_count = GetTeamCount();
	decl String: team_name[64];
	for (new team_index = 0; (team_index < max_teams_count); team_index++)
	{
		GetTeamName(team_index, team_name, sizeof(team_name));
		if (strcmp(team_name, "") != 0)
		{
			g_team_list[team_index] = team_name;
		}
	}
}

stock LogPlayerEvent(client, const String:verb[], const String:event[], bool:display_location = false, const String:properties[] = "")
{
	if (IsValidPlayer(client))
	{
		decl String:player_authid[32];
		if (!GetClientAuthString(client, player_authid, sizeof(player_authid)))
		{
			strcopy(player_authid, sizeof(player_authid), "UNKNOWN");
		}

		if (display_location)
		{
			decl Float:player_origin[3];
			GetClientAbsOrigin(client, player_origin);
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\"%s (position \"%d %d %d\")", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, properties, RoundFloat(player_origin[0]), RoundFloat(player_origin[1]), RoundFloat(player_origin[2]));
		}
		else
		{
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\"%s", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, properties); 
		}
	}
}

stock LogPlyrPlyrEvent(client, victim, const String:verb[], const String:event[], bool:display_location = false, const String:properties[] = "")
{
	if (IsValidPlayer(client) && IsValidPlayer(victim))
	{
		decl String:player_authid[32];
		if (!GetClientAuthString(client, player_authid, sizeof(player_authid)))
		{
			strcopy(player_authid, sizeof(player_authid), "UNKNOWN");
		}
		decl String:victim_authid[32];
		if (!GetClientAuthString(victim, victim_authid, sizeof(victim_authid)))
		{
			strcopy(victim_authid, sizeof(victim_authid), "UNKNOWN");
		}
		
		if (display_location)
		{
			decl Float:player_origin[3];
			GetClientAbsOrigin(client, player_origin);
			
			decl Float:victim_origin[3];
			GetClientAbsOrigin(victim, victim_origin);
			
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\" against \"%N<%d><%s><%s>\"%s (position \"%d %d %d\") (victim_position \"%d %d %d\")", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, victim, GetClientUserId(victim), victim_authid, g_team_list[GetClientTeam(victim)], properties, RoundFloat(player_origin[0]), RoundFloat(player_origin[1]), RoundFloat(player_origin[2]), RoundFloat(victim_origin[0]), RoundFloat(victim_origin[1]), RoundFloat(victim_origin[2])); 
		}
		else
		{
			LogToGame("\"%N<%d><%s><%s>\" %s \"%s\" against \"%N<%d><%s><%s>\"%s", client, GetClientUserId(client), player_authid, g_team_list[GetClientTeam(client)], verb, event, victim, GetClientUserId(victim), victim_authid, g_team_list[GetClientTeam(victim)], properties); 
		}
	}
}

stock IsValidPlayer(client)
{
	if (client > 0 && client <= MaxClients && IsClientInGame(client))
	{
		return true;
	}
	return false;
}