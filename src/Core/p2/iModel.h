#ifndef IMODEL_H
#define IMODEL_H

#include <types.h>

#include <rwcore.h>
#include <rpworld.h>

struct xModelTag;

void iModelInit();
RpAtomic* iModelFileNew(void* buffer, uint32 size);
RpAtomic* iModelFile_RWMultiAtomic(RpAtomic* model);
uint32 iModelNumBones(RpAtomic* model);
uint32 iModelTagSetup(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z);

#endif