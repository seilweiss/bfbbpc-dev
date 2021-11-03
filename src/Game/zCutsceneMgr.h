#ifndef ZCUTSCENEMGR_H
#define ZCUTSCENEMGR_H

#include "xBase.h"
#include "xCutscene.h"

struct xCutsceneMgrAsset : xBaseAsset
{
	uint32 cutsceneAssetID;
	uint32 flags;
	float32 interpSpeed;
	float32 startTime[15];
	float32 endTime[15];
	uint32 emitID[15];
};

struct xCutsceneMgr : xBase
{
	xCutsceneMgrAsset* tasset;
	xCutscene* csn;
	uint32 stop;
	xCutsceneZbufferHack* zhack;
	float32 oldfov;
};

struct zCutsceneMgr : xCutsceneMgr
{
};

void zCutsceneMgrInit(void* b, void* tasset);
void zCutSceneNamesTable_clearAll();

#endif