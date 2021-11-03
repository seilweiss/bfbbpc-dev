#ifndef ZCAMERATWEAK_H
#define ZCAMERATWEAK_H

#include "xDynAsset.h"

struct zCamTweak
{
	uint32 owner;
	float32 priority;
	float32 time;
	float32 pitch;
	float32 distMult;
};

struct zCamTweakLook
{
	float32 h;
	float32 dist;
	float32 pitch;
};

struct CameraTweak_asset : xDynAsset
{
	int32 priority;
	float32 time;
	float32 pitch_adjust;
	float32 dist_adjust;
};

struct zCameraTweak : xBase
{
	CameraTweak_asset* casset;
};

void zCameraTweakGlobal_Init();
void zCameraTweakGlobal_Reset();
void zCameraTweak_Init(xBase& data, xDynAsset& asset, ulong32);

#endif