#ifndef ZMOVEPOINT_H
#define ZMOVEPOINT_H

#include "xMovePoint.h"

struct zMovePoint : xMovePoint
{
};

struct zScene;

zMovePoint* zMovePoint_GetMemPool(int32 cnt);
void zMovePointInit(zMovePoint* m, xMovePointAsset* asset);
zMovePoint* zMovePoint_GetInst(int32 n);
void zMovePointSetup(zMovePoint* mvpt, zScene* scn);

#endif