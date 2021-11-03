#ifndef IENV_H
#define IENV_H

#include "xJSP.h"

struct iEnv
{
	RpWorld* world;
	RpWorld* collision;
	RpWorld* fx;
	RpWorld* camera;
	xJSPHeader* jsp;
	RpLight* light[2];
	RwFrame* light_frame[2];
	int32 memlvl;
};

#define IENV_TYPE_BSP 0
#define IENV_TYPE_COLLISION 1
#define IENV_TYPE_FX 2
#define IENV_TYPE_CAMERA 3

struct xEnvAsset;

void iEnvLoad(iEnv* env, const void* data, uint32, int32 dataType);
void iEnvDefaultLighting(iEnv*);
void iEnvLightingBasics(iEnv*, xEnvAsset*);

inline void iEnvStartup()
{
}

#endif