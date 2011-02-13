#pragma semicolon 1

/* Core */
new bool:g_bCoordsFound;
new bool:g_bHasBeenLoaded;
new bool:g_bModIsEnabled;
new bool:g_bRoundEnd;

new g_iMoneyPropInfo;
new g_iLastVIPUserId, g_iVIPIndex;


new String:g_sInitialRadio[][] = { "radio/letsgo.wav", "radio/moveout.wav", "radio/go.wav", "radio/locknload.wav" };
new String:g_sSmall1[][] = {"models/player/vip/small2/vip.dx80.vtx", "models/player/vip/small2/vip.dx90.vtx", "models/player/vip/small2/vip.mdl", "models/player/vip/small2/vip.phy", "models/player/vip/small2/vip.sw.vtx", "models/player/vip/small2/vip.vvd", "materials/models/player/vip/small1/erdim_cylmap.vmt", "materials/models/player/vip/small1/erdim_cylmap.vtf", "materials/models/player/vip/small1/erdim_facemap.vmt", "materials/models/player/vip/small1/eyeball_l.vmt", "materials/models/player/vip/small1/eyeball_l.vtf", "materials/models/player/vip/small1/eyeball_r.vmt", "materials/models/player/vip/small1/eyeball_r.vtf", "materials/models/player/vip/small1/UrbanTemp.vmt", "materials/models/player/vip/small1/UrbanTemp.vtf"};

new Handle:g_hGameConfiguration		=	INVALID_HANDLE;
new Handle:g_hMPFreezeTime			= 	INVALID_HANDLE;
new Handle:g_hRoundTime				=	INVALID_HANDLE;
new Handle:g_hRoundTimer			=	INVALID_HANDLE;
new Handle:g_hTerminateRound		=	INVALID_HANDLE;
new Handle:g_hVIPTimerHandle		=	INVALID_HANDLE;

new Float:g_fVIPResQ1Min[3], Float:g_fVIPResQ1Max[3];
new Float:g_fVIPResQ2Min[3], Float:g_fVIPResQ2Max[3];
new Float:g_fVIPResQdTele[3];

/* CVAR Values*/

enum CvarsList
{
    Handle:CVAR_FORCELOAD,
	Handle:CVAR_GAME_DESCRIPTION,
	Handle:CVAR_VIPVOCAL,
	Handle:CVAR_VIPMODEL_PATH,
	Handle:CVAR_VIPTELEPORT,
	Handle:CVAR_VIPWEAPON,
	Handle:CVAR_VIPZONE1,
	Handle:CVAR_VIPZONE2
}

new Handle:g_hConVar[CvarsList];

new bool:g_bChangeGameDesc;
new bool:g_bForceLoad;

new Float:g_fRoundEndFloat;
new Float:g_fRoundEndDuration;

new bool:g_bVocalize;

new String:g_sVIPVersionString[64];

new String:g_sVIPModelPath[256];
new String:g_sVIPWeaponName[32];