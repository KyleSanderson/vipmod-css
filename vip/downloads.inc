#pragma semicolon 1

public FireUpConfigFiles()
{
	decl String:buffer[1024];
	AutoExecConfig(true, "vipmod");
	BuildPath(Path_SM, buffer, sizeof(buffer), "configs/vip/downloads.cfg");
	
	if (!FileExists(buffer)) /* Legacy Mode */
	{
		new Size = sizeof(g_sSmall1);
		for(new i; i < Size; i++)
		{
			AddFileToDownloadsTable(g_sSmall1[i]);
			PrecacheModel(g_sSmall1[i]);
		}
		
		strcopy(g_sVIPModelPath, sizeof(g_sVIPModelPath), "models/player/vip/small2/vip.mdl");
		PrecacheSound("radio/vip.wav");
		for(new i; i <= 3; i++)
		{
			PrecacheSound(g_sInitialRadio[i]);
		}
		return;
	}
	
	/* New Mode - Sslice*/
	
	new Handle:f = OpenFile(buffer, "r");
	while (!IsEndOfFile(f) && ReadFileLine(f, buffer, sizeof(buffer)))
	{
		TrimString(buffer);
		if(FileExists(buffer))
		{
			AddFileToDownloadsTable(buffer);
			if((StrContains(buffer, ".mdl") != -1 || StrContains(buffer, ".vmt") != -1 || StrContains(buffer, ".vtf") != -1 || StrContains(buffer, ".mdl") != -1) && StrContains(buffer, "models/") != -1)
			{
				PrecacheModel(buffer);
				continue;
			}
			
			if((StrContains(buffer, ".mp3") != -1 || StrContains(buffer, ".wav") != -1) && StrContains(buffer, "sound") != -1)
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
	CloseHandle(f);
}