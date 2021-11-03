#ifndef IMODEL_H
#define IMODEL_H

#include <types.h>

#include <rwcore.h>
#include <rpworld.h>

struct xModelTag;

RpAtomic* iModelFile_RWMultiAtomic(RpAtomic* model);
uint32 iModelNumBones(RpAtomic* model);
uint32 iModelTagSetup(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z);

#endif