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
				switch(FileExists(g_sSmall1[i]))
				{
					case 0:
					{
						LogError("Unable to Find %s", g_sSmall1[i]);
					}
					
					case 1:
					{
						AddFileToDownloadsTable(g_sSmall1[i]);
						PrecacheModel(g_sSmall1[i]);
					}
				}
			}
			
			switch(FileExists(g_sSmall1[2]))
			{
				case 0:
				{
					LogError("Unable to set the VIP's model to %s", g_sSmall1[2]);
				}
				
				case 1:
				{
					strcopy(g_sVIPModelPath, sizeof(g_sVIPModelPath), g_sSmall1[2]);
				}
			}
		}
		
		case 1: // New Mode
		{
			new Handle:f = OpenFile(buffer, "r");
			while (!IsEndOfFile(f) && ReadFileLine(f, buffer, sizeof(buffer)))
			{
				TrimString(buffer);
				switch(FileExists(buffer))
				{
					case 0:
					{
						LogError("Unable to Find %s", buffer);
					}
					
					case 1:
					{
						AddFileToDownloadsTable(buffer);
						PrecacheModel(buffer);						
					}
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
			switch(FileExists(buffer))
			{
				case 0:
				{
					LogError("Unable to Find %s", buffer);
				}
				
				case 1:
				{
					AddFileToDownloadsTable(buffer);
					PrecacheSound(buffer);						
				}
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
			switch(FileExists(buffer))
			{
				case 0:
				{
					LogError("Unable to Find %s", buffer);
				}
				
				case 1:
				{
					AddFileToDownloadsTable(buffer);
					PrecacheDecal(buffer);						
				}
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