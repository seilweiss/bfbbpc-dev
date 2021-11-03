#include "xUpdateCull.h"

STUB uint32 xUpdateCull_DistanceSquaredCB(void* ent, void* cbdata)
{
    return 0;
}

STUB xUpdateCullMgr* xUpdateCull_Init(void** ent, uint32 entCount, xGroup** group, uint32 groupCount)
{
    return NULL;
}

STUB void xUpdateCull_Update(xUpdateCullMgr* m, uint32 percent_update)
{

}

STUB void xUpdateCull_SetCB(xUpdateCullMgr* m, void* entity, xUpdateCullEntCallback cb, void* cbdata)
{

}