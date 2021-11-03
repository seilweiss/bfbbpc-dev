#ifndef XTIMER_H
#define XTIMER_H

#include "xBase.h"

struct xTimerAsset : xBaseAsset
{
	float32 seconds;
	float32 randomRange;
};

struct xTimer : xBase
{
	xTimerAsset* tasset;
	uint8 state;
	uint8 runsInPause;
	uint16 flags;
	float32 secondsLeft;
};

void xTimerInit(void* b, void* tasset);

#endif