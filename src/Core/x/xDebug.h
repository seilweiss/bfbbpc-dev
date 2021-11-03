#ifndef XDEBUG_H
#define XDEBUG_H

#include <types.h>

extern uint32 gFrameCount;

void xprintf(const char* format, ...);
void xDebugInit();
void xDebugUpdate();
void xDebugExit();
void xDebugTimestampScreen();

#ifdef DEBUG
#define XPRINTF xprintf
#else
#define XPRINTF
#endif

#endif