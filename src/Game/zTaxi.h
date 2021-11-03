#ifndef ZTAXI_H
#define ZTAXI_H

#include "zEnt.h"
#include "xDynAsset.h"

struct taxi_asset : xDynAsset
{
	uint32 marker;
	uint32 cameraID;
	uint32 portalID;
	uint32 talkBoxID;
	uint32 textID;
	uint32 taxiID;
	float32 invDelay;
	float32 portalDelay;
};

struct zTaxi : xBase
{
	taxi_asset* basset;
	zEnt* taxi;
	xVec3 pos;
	uint32 currState;
	uint32 prevState;
	float32 portalTimer;
	float32 invTimer;
	float32 radius;
};

void zTaxi_Init(xBase& data, xDynAsset& asset, ulong32);
void zTaxi_Setup(zTaxi* taxi);

#endif