#ifndef XBOUND_H
#define XBOUND_H

#include "xQuickCull.h"

struct xBound
{
	xQCData qcd;
	uint8 type;
	uint8 pad[3];
	union
	{
		xSphere sph;
		xBBox box;
		xCylinder cyl;
	};
	xMat4x3* mat;
};

#define XBOUND_TYPE_NA 0
#define XBOUND_TYPE_SPHERE 1
#define XBOUND_TYPE_BOX 2
#define XBOUND_TYPE_CYL 3
#define XBOUND_TYPE_OBB 4

#endif