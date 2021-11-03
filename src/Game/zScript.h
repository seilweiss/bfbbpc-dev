#ifndef ZSCRIPT_H
#define ZSCRIPT_H

#include "xBase.h"

struct xScriptAsset : xBaseAsset
{
	float32 scriptStartTime;
	uint32 eventCount;
};

typedef struct _zScript : xBase
{
	xScriptAsset* tasset;
	uint8 state;
	uint8 more;
	uint8 pad[2];
	float32 time;
} zScript;

void zScriptInit(void* b, void* tasset);

#endif