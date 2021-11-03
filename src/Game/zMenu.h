#ifndef ZMENU_H
#define ZMENU_H

#include <types.h>

extern bool menu_fmv_played;

void zMenuInit(uint32 theSceneID);
void zMenuExit();
void zMenuSetup();
uint32 zMenuLoop();
bool32 zMenuIsFirstBoot();
void zMenuFMVPlay(char* filename, uint32 buttons, float32 time, bool skippable, bool lockController);
void zMenuFirstBootSet(bool32 firstBoot);

#endif