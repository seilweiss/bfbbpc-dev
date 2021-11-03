#ifndef ZVOLUME_H
#define ZVOLUME_H

#include "xVolume.h"

struct zVolume : xVolume
{
};

extern int32 gOccludeCount;

void zVolumeInit();
void zVolumeSetup();
zVolume* zVolumeGetVolume(uint16 n);

#endif