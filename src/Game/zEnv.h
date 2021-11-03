#ifndef ZENV_H
#define ZENV_H

#include "xBase.h"

struct xEnvAsset : xBaseAsset
{
	uint32 bspAssetID;
	uint32 startCameraAssetID;
	uint32 climateFlags;
	float32 climateStrengthMin;
	float32 climateStrengthMax;
	uint32 bspLightKit;
	uint32 objectLightKit;
	float32 padF1;
	uint32 bspCollisionAssetID;
	uint32 bspFXAssetID;
	uint32 bspCameraAssetID;
	uint32 bspMapperID;
	uint32 bspMapperCollisionID;
	uint32 bspMapperFXID;
	float32 loldHeight;
};

typedef struct _zEnv : xBase
{
	xEnvAsset* easset;
} zEnv;

void zEnvInit(void* env, void* easset);
void zEnvInit(zEnv* env, xEnvAsset* easset);
void zEnvSetup(zEnv* env);
void zEnvStartingCamera(zEnv* env);
bool32 zEnvEventCB(xBase*, xBase* to, uint32 toEvent, const float32* toParam, xBase*);

#endif