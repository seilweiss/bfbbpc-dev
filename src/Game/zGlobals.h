#ifndef ZGLOBALS_H
#define ZGLOBALS_H

#include "xGlobals.h"
#include "zEntPlayer.h"
#include "zScene.h"

struct zAssetPickupTable;
struct zCutsceneMgr;

struct zGlobals : xGlobals
{
	zPlayerGlobals player;
	zAssetPickupTable* pickupTable;
	zCutsceneMgr* cmgr;
	zScene* sceneCur;
	zScene* scenePreload;
};

#endif