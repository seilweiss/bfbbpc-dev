#ifndef XFX_H
#define XFX_H

#include <types.h>
#include <rwcore.h>
#include <rpworld.h>

void xFXInit();
void xFX_SceneEnter(RpWorld* world);
void xFXPreAllocMatFX(RpClump* clump);
uint32 xFXanimUVCreate();
void xFXFireworksInit(const char* fireworksTrailEmitter, const char* fireworksEmitter1, const char* fireworksEmitter2, const char* fireworksSound, const char* fireworksLaunchSound);
void xFXStreakInit();
void xFXShineInit();
void xFXRibbonSceneEnter();
void xFXAuraInit();
void xFXStartup();
void xFXShutdown();
void xFXSceneInit();
void xFXSceneSetup();
void xFXScenePrepare();
void xFXSceneFinish();

#endif