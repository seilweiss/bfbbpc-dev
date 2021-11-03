#ifndef ZENT_H
#define ZENT_H

#include "xEnt.h"
#include "xAnim.h"

struct zEnt : xEnt
{
	xAnimTable* atbl;
};

struct zScene;

void zEntInit(zEnt* ent, xEntAsset* asset, uint32 type);
void zEntUpdate(zEnt* ent, zScene* scene, float32 elapsedSec);
void zEntEventAll(xBase* from, uint32 fromEvent, uint32 toEvent, float32* toParam);
void zEntEventAllOfType(uint32 toEvent, uint32 type);
xModelInstance* zEntRecurseModelInfo(void* info, xEnt* ent);
void zEntParseModelInfo(xEnt* ent, uint32 assetID);

#endif