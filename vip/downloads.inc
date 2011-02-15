#pragma semicolon 1

public FireUpConfigFiles()
{
	decl String:SMPath[256];
	decl String:buffer[1024];
	
	BuildPath(Path_SM, SMPath, sizeof(SMPath), "");
	FormatEx(buffer, sizeof(buffer), "%sconfigs/vip/Models.cfg", SMPath);
	switch(FileExists(buffer))
	{
		case 0: // Legacy Mode
		{
			new Size = sizeof(g_sSmall1);
			for(new i; i < Size; i++)
			{
				AddFileToDownloadsTable(g_sSmall1[i]);
				PrecacheModel(g_sSmall1[i]);
			}
			
			strcopy(g_sVIPModelPath, sizeof(g_sVIPModelPath), "models/player/vip/small2/vip.mdl");
		}
		
		case 1: // New Mode
		{
			new Handle:f = OpenFile(buffer, "r");
			while (!IsEndOfFile(f) && ReadFileLine(f, buffer, sizeof(buffer)))
			{
				TrimString(buffer);
				if(FileExists(buffer))
				{
					AddFileToDownloadsTable(buffer);
					PrecacheModel(buffer);
				}
			}
			
			CloseHandle(f);
		}
	}

	FormatEx(buffer, sizeof(buffer), "%sconfigs/vip/Sounds.cfg", SMPath);
	if(FileExists(buffer))
	{
		new Handle:f = OpenFile(buffer, "r");
		while (!IsEndOfFile(f) && ReadFileLine(f, buffer, sizeof(buffer)))
		{
			TrimString(buffer);
			if(FileExists(buffer))
			{
				AddFileToDownloadsTable(buffer);
				PrecacheSound(buffer);
			}
		}
		
		CloseHandle(f);
	}

	FormatEx(buffer, sizeof(buffer), "%sconfigs/vip/Decals.cfg", SMPath);
	if(FileExists(buffer))
	{
		new Handle:f = OpenFile(buffer, "r");
		while (!IsEndOfFile(f) && ReadFileLine(f, buffer, sizeof(buffer)))
		{
			TrimString(buffer);
			if(FileExists(buffer))
			{
				AddFileToDownloadsTable(buffer);
				PrecacheDecal(buffer);
			}
		}
		
		CloseHandle(f);
	}
}

/* Better/Global Method. However, requires 9+ native calls on a good day :|.
{
	while (!IsEndOfFile(f) && ReadFileLine(f, buffer, sizeof(buffer))) // 9 Native Calls... If you know a better (more reliable, and effecient) method. Please contact the author!
	{
		TrimString(buffer);
		if(FileExists(buffer))
		{
			AddFileToDownloadsTable(buffer);
			if((StrContains(buffer, ".mdl") != -1 || StrContains(buffer, ".vmt") != -1 || StrContains(buffer, ".vtf") != -1 || StrContains(buffer, ".phy") != -1) || StrContains(buffer, ".vtx") != -1 || StrContains(buffer, ".vvd") != -1)
			{
				PrecacheModel(buffer);
				continue;
			}
			
			if((StrContains(buffer, ".mp3") != -1 || StrContains(buffer, ".wav") != -1))
			{
				PrecacheSound(buffer);
				continue;
			}
			
			if(StrContains(buffer, "decal") != -1)
			{
				PrecacheDecal(buffer);
			}

			PrecacheGeneric(buffer);
		}
	}
}*/