#ifndef XENV_H
#define XENV_H

#include "iEnv.h"
#include "xLightKit.h"

struct xEnv
{
	iEnv* geom;
	iEnv ienv;
	xLightKit* lightKit;
};

#define XENV_TYPE_BSP 0
#define XENV_TYPE_COLLISION 1
#define XENV_TYPE_FX 2
#define XENV_TYPE_CAMERA 3

extern xEnv* gCurXEnv;

void xEnvLoadBsp(xEnv* env, const void* data, uint32 datasize, int32 dataType);
void xEnvSetup(xEnv* env);

#endif