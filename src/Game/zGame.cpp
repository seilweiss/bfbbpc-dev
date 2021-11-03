#include "zGame.h"

#include "iSystem.h"
#include "xEnt.h"
#include "xMarkerAsset.h"
#include "xstransvc.h"
#include "zBase.h"
#include "zMain.h"
#include "zCamera.h"

uint32 gGameSfxReport = 0;
uint32 startPressed = 0xFFFFFFFF;

static uint32 sPlayerMarkerStartID = 0;
static uint32 sPlayerMarkerStartCamID = 0;
static float32 sPlayerStartAngle = 0.0f;
static bool32 sPortalling = FALSE;

uint32 gSoak = 0;
bool sHackSmoothedUpdate = false;
eGameWhereAmI gGameWhereAmI = eGameWhere_NA;

STUB void zGameInit(uint32 theSceneID)
{

}

STUB void zGameExit()
{

}

STUB void zGameSetup()
{

}

STUB void zGameLoop()
{
	iSystemUpdate();
}

STUB void zGameScreenTransitionBegin()
{

}

STUB void zGameScreenTransitionUpdate(float32 percentComplete, char* msg)
{

}

STUB void zGameScreenTransitionUpdate(float32 percentComplete, char* msg, uint8* rgba)
{

}

STUB void zGameScreenTransitionEnd()
{

}

UNCHECKED void zGameSetupPlayer()
{
	xEntAsset* asset;
	int32 playerCount = xSTAssetCountByType('PLYR');

	asset = (xEntAsset*)xSTFindAssetByType('PLYR', playerCount - 1);
	asset->baseType = eBaseTypePlayer;

	if (sPortalling)
	{
		if (sPlayerStartAngle > -100000000.0f)
		{
			asset->ang.x = DEG2RAD(sPlayerStartAngle);
		}

		sPortalling = FALSE;
	}

	asset->ang.y = 0.0f;
	asset->ang.z = 0.0f;

	gGameWhereAmI = eGameWhere_SetupPlayerInit;

	zEntPlayer_Init(&globals.player.ent, asset);

	if (sPlayerMarkerStartID != 0)
	{
		uint32 size;
		xMarkerAsset* m = (xMarkerAsset*)xSTFindAsset(sPlayerMarkerStartID, &size);

		if (m)
		{
			xVec3Copy(&globals.player.ent.frame->mat.pos, &m->pos);
			xVec3Copy(&globals.player.ent.frame->mat.pos, &m->pos);
			xVec3Copy((xVec3*)&globals.player.ent.model->Mat->pos, &m->pos);

			xCameraSetTargetMatrix(&globals.camera, xEntGetFrame(&globals.player.ent));
		}

		sPlayerMarkerStartID = 0;
	}

	gGameWhereAmI = eGameWhere_SetupPlayerCamera;

	zCameraReset(&globals.camera);
	zEntPlayer_StoreCheckPoint(&globals.player.ent.frame->mat.pos, globals.player.ent.frame->rot.angle, globals.camera.id);

	gGameWhereAmI = eGameWhere_SetupPlayerEnd;
}