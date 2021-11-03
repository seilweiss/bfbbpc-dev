#ifndef ISND_H
#define ISND_H

#include <types.h>

void iSndSuspendCD(uint32);
void iSndSceneExit();
void iSndInitSceneLoaded();
void iSndWaitForDeadSounds();

#endif