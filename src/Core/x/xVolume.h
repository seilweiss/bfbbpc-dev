#ifndef XVOLUME_H
#define XVOLUME_H

#include "xBase.h"
#include "xBound.h"

struct xVolumeAsset : xBaseAsset
{
	uint32 flags;
	xBound bound;
	float32 rot;
	float32 xpivot;
	float32 zpivot;
};

struct xVolume : xBase
{
	xVolumeAsset* asset;
};

#endif