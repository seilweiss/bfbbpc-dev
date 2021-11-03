#ifndef IFOG_H
#define IFOG_H

#include <types.h>
#include <rwcore.h>

struct iFogParams
{
	RwFogType type;
	float32 start;
	float32 stop;
	float32 density;
	RwRGBA fogcolor;
	RwRGBA bgcolor;
	uint8* table;
};

#endif