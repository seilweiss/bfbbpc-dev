#ifndef XGLOBALS_H
#define XGLOBALS_H

#include "xCamera.h"
#include "xPad.h"
#include "xUpdateCull.h"
#include "iFog.h"
#include "iTime.h"

#include <rwcore.h>
#include <rpworld.h>

#define PROF_FUNC_LEN 128
#define PROF_FUNC_TRIANGLE 0
#define PROF_FUNC_SQUARE 1
#define PROF_FUNC_LEFT 2
#define PROF_FUNC_RIGHT 3
#define PROF_FUNC_UP 4
#define PROF_FUNC_DOWN 5
#define PROF_FUNC_COUNT 6

struct xGlobals
{
	xCamera camera;
	xPad* pad0;
	xPad* pad1;
	xPad* pad2;
	xPad* pad3;
	int32 profile;
	char profFunc[PROF_FUNC_COUNT][PROF_FUNC_LEN];
	xUpdateCullMgr* updateMgr;
	bool32 sceneFirst;
	char sceneStart[32];
	RpWorld* currWorld;
	iFogParams fog;
	iFogParams fogA;
	iFogParams fogB;
	iTime fog_t0;
	iTime fog_t1;
	bool32 option_vibration;
	uint32 QuarterSpeed;
	float32 update_dt;
	bool32 useHIPHOP;
	bool NoMusic;
	int8 currentActivePad;
	bool firstStartPressed;
	uint32 minVSyncCnt;
	bool dontShowPadMessageDuringLoadingOrCutScene;
	bool autoSaveFeature;
};

#endif