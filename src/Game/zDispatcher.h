#ifndef ZDISPATCHER_H
#define ZDISPATCHER_H

#include "xBase.h"

typedef struct st_ZDISPATCH_DATA : xBase
{
	xBaseAsset* rawass;
	int32 placeholder;
} ZDISPATCH_DATA;

struct zScene;

void zDispatcher_Startup();
void zDispatcher_Shutdown();
void zDispatcher_scenePrepare();
void zDispatcher_sceneFinish();
ZDISPATCH_DATA* zDispatcher_memPool(int32 cnt);
ZDISPATCH_DATA* zDispatcher_getInst(ZDISPATCH_DATA* pool, int32 idx);
void zDispatcher_Init(ZDISPATCH_DATA* dspdata, xBaseAsset* bass);
void zDispatcher_InitDep(ZDISPATCH_DATA* dspdata, zScene*);

#endif