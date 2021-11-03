#ifndef ZGUST_H
#define ZGUST_H

#include "xBase.h"
#include "zVolume.h"
#include "xEnt.h"

struct zGustAsset : xBaseAsset
{
	uint32 flags;
	uint32 volumeID;
	uint32 effectID;
	xVec3 vel;
	float32 fade;
	float32 partMod;
};

struct zGust : xBase
{
	uint32 flags;
	zGustAsset* asset;
	zVolume* volume;
	zVolume* fx_volume;
	float32 debris_timer;
};

struct zGustData
{
	uint32 gust_on;
	zGust* g[4];
	float32 lerp[4];
};

void zGustInit();
void zGustSetup();
zGust* zGustGetGust(uint16 n);
void zGustUpdateEnt(xEnt* ent, xScene*, float32 dt, void* gdata);

#endif