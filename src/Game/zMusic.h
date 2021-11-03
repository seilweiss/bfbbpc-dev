#ifndef ZMUSIC_H
#define ZMUSIC_H

#include <types.h>

#define ZMUSIC_SITUATION_0 0

void zMusicInit();
void zMusicNotify(int32 situation);
void zMusicUpdate(float32 dt);
void zMusicKill();

#endif