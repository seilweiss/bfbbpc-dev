#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <types.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAMES_PER_SEC 60.0f
#define ONE_FRAME (1.0f/FRAMES_PER_SEC)

void iSystemInit(uint32 options);
void iSystemExit();
void iSystemUpdate();
bool iSystemShouldQuit();

#endif