#ifndef XUTIL_H
#define XUTIL_H

#include <types.h>

int32 xUtilStartup();
int32 xUtilShutdown();
const char* xUtil_idtag2string(uint32 srctag, int32 bufidx = 0);

#endif