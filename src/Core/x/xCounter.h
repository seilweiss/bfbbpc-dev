#ifndef XCOUNTER_H
#define XCOUNTER_H

#include "xBase.h"

struct xCounterAsset : xBaseAsset
{
	int16 count;
};

typedef struct _xCounter : xBase
{
	xCounterAsset* asset;
	int16 count;
	uint8 state;
	uint8 counterFlags;
} xCounter;

void xCounterInit();
void xCounterInit(void* b, void* asset);

#endif