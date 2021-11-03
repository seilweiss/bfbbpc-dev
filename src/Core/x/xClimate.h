#ifndef XCLIMATE_H
#define XCLIMATE_H

#include "zParEmitter.h"

typedef struct _tagRain
{
	int32 rain;
	float32 strength;
	zParEmitter* rain_emitter;
	zParEmitter* snow_emitter;
} Rain;

typedef struct _tagWind
{
	float32 strength;
	float32 angle;
	xVec3 dir;
} Wind;

typedef struct _tagClimate
{
	Rain rain;
	Wind wind;
} Climate;

struct xEnvAsset;

void xClimateInit(Climate* climate);
void xClimateInitAsset(Climate* climate, xEnvAsset* easset);

#endif