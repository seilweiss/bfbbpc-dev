#ifndef ZBUSSTOP_H
#define ZBUSSTOP_H

#include "zEnt.h"
#include "xDynAsset.h"

struct busstop_asset : xDynAsset
{
	uint32 marker;
	uint32 character;
	uint32 cameraID;
	uint32 busID;
	float32 delay;
};

struct zBusStop : xBase
{
	busstop_asset* basset;
	zEnt* bus;
	xVec3 pos;
	uint32 currState;
	uint32 prevState;
	float32 switchTimer;
};

void zBusStop_Init(xBase& data, xDynAsset& asset, ulong32);
void zBusStop_Setup(zBusStop* bstop);

#endif