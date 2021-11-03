#ifndef ZPORTAL_H
#define ZPORTAL_H

#include "xBase.h"

struct xPortalAsset : xBaseAsset
{
	uint32 assetCameraID;
	uint32 assetMarkerID;
	float32 ang;
	uint32 sceneID;
};

typedef struct _zPortal : xBase
{
	xPortalAsset* passet;
} zPortal;

void zPortalInit(void* portal, void* passet);

#endif