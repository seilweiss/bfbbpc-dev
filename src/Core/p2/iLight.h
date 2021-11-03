#ifndef ILIGHT_H
#define ILIGHT_H

#include "xMath3.h"
#include "xColor.h"

#include <rwcore.h>
#include <rpworld.h>

struct iLight
{
	uint32 type;
	RpLight* hw;
	xSphere sph;
	float32 radius_sq;
	xFColor color;
	xVec3 dir;
	float32 coneangle;
};

void iLightInit(RpWorld* world);

#endif