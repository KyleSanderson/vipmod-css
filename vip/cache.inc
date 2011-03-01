#pragma semicolon 1

/* Core */
new bool:g_bCoordsFound;
new bool:g_bModIsEnabled;
new bool:g_bRoundEnd;
new bool:g_bUserMessageEnabled; // Workaround for lack of Working Unhook?

new bool:g_bEscZone1Coords;
new bool:g_bEscZone2Coords;

new g_iMoneyPropInfo;
new g_iLastVIPUserId, g_iVIPIndex;

new String:g_sInitialRadio[][] = { "radio/letsgo.wav", "radio/moveout.wav", "radio/go.wav", "radio/locknload.wav" };
new String:g_sSmall1[][] = { "models/player/vip/small2/vip.dx80.vtx", "models/player/vip/small2/vip.dx90.vtx", "models/player/vip/small2/vip.mdl", "models/player/vip/small2/vip.phy", "models/player/vip/small2/vip.sw.vtx", "models/player/vip/small2/vip.vvd", "materials/models/player/vip/small1/erdim_cylmap.vmt", "materials/models/player/vip/small1/erdim_cylmap.vtf", "materials/models/player/vip/small1/erdim_facemap.vmt", "materials/models/player/vip/small1/eyeball_l.vmt", "materials/models/player/vip/small1/eyeball_l.vtf", "materials/models/player/vip/small1/eyeball_r.vmt", "materials/models/player/vip/small1/eyeball_r.vtf", "materials/models/player/vip/small1/UrbanTemp.vmt", "materials/models/player/vip/small1/UrbanTemp.vtf" };
new String:g_sVIPEscapeZone1[128];
new String:g_sVIPEscapeZone2[128];
new String:g_sVIPTeleportZone[128];

new Handle:g_hGameConfiguration		=	INVALID_HANDLE;
new Handle:g_hRoundTimer			=	INVALID_HANDLE;
new Handle:g_hTerminateRound		=	INVALID_HANDLE;
new Handle:g_hVIPTimerHandle		=	INVALID_HANDLE;
new Handle:g_hUMTimerHandle			=	INVALID_HANDLE;

new Float:g_fVIPResQ1Min[3], Float:g_fVIPResQ1Max[3];
new Float:g_fVIPResQ2Min[3], Float:g_fVIPResQ2Max[3];
new Float:g_fVIPResQdTele[3];

/* CVAR Values*/

new bool:g_bChangeGameDesc = true;
new bool:g_bForceLoad;
new bool:g_bVocalize = true;

new Float:g_fRoundEndDelay;
new Float:g_fRoundDuration;

new String:g_sVIPVersionString[64];

new String:g_sVIPModelPath[256] = { "models/player/vip/small2/vip.mdl" };
new String:g_sVIPWeaponName[32] = { "weapon_usp" };