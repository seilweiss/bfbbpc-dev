#include "zDispatcher.h"

#include <types.h>

void zDispatcher_Startup()
{
}

void zDispatcher_Shutdown()
{
}

STUB void zDispatcher_scenePrepare()
{

}

STUB void zDispatcher_sceneFinish()
{

}

STUB ZDISPATCH_DATA* zDispatcher_memPool(int32 cnt)
{
	return NULL;
}

STUB ZDISPATCH_DATA* zDispatcher_getInst(ZDISPATCH_DATA* pool, int32 idx)
{
	return NULL;
}

STUB void zDispatcher_Init(ZDISPATCH_DATA* dspdata, xBaseAsset* bass)
{

}

STUB void zDispatcher_InitDep(ZDISPATCH_DATA* dspdata, zScene*)
{

}