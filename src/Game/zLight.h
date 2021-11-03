#ifndef ZLIGHT_H
#define ZLIGHT_H

#include "xBase.h"
#include "xMath3.h"
#include "iLight.h"
#include "zVolume.h"
#include "xEnv.h"

struct zLightAsset : xBaseAsset
{
	uint8 lightType;
	uint8 lightEffect;
	uint8 lightPad[2];
	uint32 lightFlags;
	float32 lightColor[4];
	xVec3 lightDir;
	float32 lightConeAngle;
	xSphere lightSphere;
	uint32 attachID;
};

typedef struct _zLight : xBase
{
	uint32 flags;
	zLightAsset* tasset;
	iLight light;
	xBase* attached_to;
	int32 true_idx;
	float32* reg;
	int32 effect_idx;
} zLight;

void zLightResetAll(xEnv* env);
void zLightInit(void* b, void* tasset);
void zLightResolveLinks();
void zLightDestroyAll();
void zLightSetVolume(zVolume* vol);

#endif