#ifndef ZCAMERAFLY_H
#define ZCAMERAFLY_H

#include "xDynAsset.h"

struct CameraFly_asset : xDynAsset
{
	uint32 flyID;
};

struct zCameraFly : xBase
{
	CameraFly_asset* casset;
};

void zCameraFly_Init(xBase& data, xDynAsset& asset, ulong32);
void zCameraFly_Setup(zCameraFly* fly);

#endif