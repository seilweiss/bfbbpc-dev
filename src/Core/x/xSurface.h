#ifndef XSURFACE_H
#define XSURFACE_H

#include "xEnt.h"

struct xSurface : xBase
{
	uint32 idx;
	uint32 type;
	union
	{
		uint32 mat_idx;
		xEnt* ent;
		void* obj;
	};
	float32 friction;
	uint8 state;
	uint8 pad[3];
	void* moprops;
};

xSurface* xSurfaceGetByIdx(uint16 n);

#endif