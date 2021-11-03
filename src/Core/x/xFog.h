#ifndef XFOG_H
#define XFOG_H

#include "xBase.h"

struct xFogAsset : xBaseAsset
{
	uint8 bkgndColor[4];
	uint8 fogColor[4];
	float32 fogDensity;
	float32 fogStart;
	float32 fogStop;
	float32 transitionTime;
	uint8 fogType;
	uint8 padFog[3];
};

typedef struct _xFog : xBase
{
	xFogAsset* tasset;
} xFog;

void xFogClearFog();
void xFogInit(void* b, void* tasset);

#endif