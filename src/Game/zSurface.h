#ifndef ZSURFACE_H
#define ZSURFACE_H

#include "xSurface.h"

void zSurfaceInit();
void zSurfaceRegisterMapper(uint32 assetId);
void zSurfaceExit();
void zSurfaceSetup(xSurface* s);

#endif