#ifndef XSHADOW_H
#define XSHADOW_H

#include <types.h>
#include <rwcore.h>
#include <rpworld.h>

extern RpLight* ShadowLight;
extern RwRGBAReal ShadowLightColor;
extern RwCamera* ShadowCamera;
extern RwRaster* ShadowCameraRaster;

void xShadowInit();
void xShadowSetWorld(RpWorld* world);
bool32 xShadowCameraCreate();
void xShadowManager_Init(int32 numEnts);

#endif