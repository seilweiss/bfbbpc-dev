#include "xDebug.h"

#include <stdio.h>
#include <stdarg.h>

uint32 gFrameCount = 0;

void xprintf(const char* format, ...)
{
#ifdef DEBUG
    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
#endif
}

void xDebugInit()
{
}

void xDebugUpdate()
{
}

void xDebugExit()
{
}

void xDebugTimestampScreen()
{
}