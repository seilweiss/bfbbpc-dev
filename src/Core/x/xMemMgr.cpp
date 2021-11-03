#include "xMemMgr.h"

#include "iMemMgr.h"

#include "xDebug.h"

#include <stdlib.h>
#include <string.h>

#include <rwcore.h>

xMemInfo gMemInfo;
uint32 gActiveHeap;
xMemHeap gxHeap[3];
xMemBaseNotifyCallback sMemBaseNotifyFunc;

void xMemInit()
{
	iMemInit();

	xMemInitHeap(&gxHeap[0], gMemInfo.DRAM.addr, gMemInfo.DRAM.size, 0x8826);
	xMemInitHeap(&gxHeap[1], gMemInfo.DRAM.addr + gMemInfo.DRAM.size, gMemInfo.DRAM.size, 0x8925);
	xMemInitHeap(&gxHeap[2], gMemInfo.DRAM.addr + gMemInfo.DRAM.size, gMemInfo.DRAM.size, 0x892A);

	gxHeap[0].opp_heap[0] = 1;
	gxHeap[0].opp_heap[1] = 2;
	gxHeap[1].opp_heap[0] = 0;
	gxHeap[1].opp_heap[1] = 2;
	gxHeap[2].opp_heap[0] = 0;
	gxHeap[2].opp_heap[1] = 1;

	gActiveHeap = 0;
}

void xMemExit()
{
	iMemExit();
}

void xMemInitHeap(xMemHeap* heap, uint32 base, uint32 size, uint32 flags)
{
    uint32 old_base = base;
    int32 align = 1 << XMEMHEAP_GETALIGN(flags);

    if (flags & XMEMHEAP_UNK0x100)
    {
        base &= -align;
        size -= old_base - base;
    }
    else
    {
        base = -align & (base + align - 1);
        size -= base - old_base;
    }

    heap->state_idx = 0;
    heap->hard_base = base;
    heap->size = size;
    heap->flags = flags;
    heap->lastblk = NULL;

    xHeapState* sp = &heap->state[heap->state_idx];

    sp->curr = base;
    sp->blk_ct = 0;

    if (flags & XMEMHEAP_UNK0x10000)
    {
        heap->max_blks = 1 << XMEMHEAP_GETMAXBLOCKS(flags);
        heap->blk = (xMemBlock*)malloc(heap->max_blks * sizeof(xMemBlock));

        memset(heap->blk, 0, heap->max_blks * sizeof(xMemBlock));
    }
    else
    {
        heap->max_blks = -1;
        heap->blk = NULL;
    }

    heap->opp_heap[0] = -1;
    heap->opp_heap[1] = -1;
}

STUB void* xMemAlloc(uint32 heapID, uint32 size, int32 align)
{
#if 1
    void* mem = RwMalloc(size, rwMEMHINTDUR_GLOBAL);
    memset(mem, 0, size);
    return mem;
#endif
}

void* xMemPushTemp(uint32 size)
{
    return RwMalloc(size, rwMEMHINTDUR_FUNCTION);
}

void xMemPopTemp(void* addr)
{
    RwFree(addr);
}

int32 xMemPushBase(uint32 heapID)
{
    xMemHeap* heap = &gxHeap[heapID];

    heap->state_idx++;

    xHeapState* sp = &heap->state[heap->state_idx];

    *sp = *(sp - 1);

    if (sMemBaseNotifyFunc)
    {
        sMemBaseNotifyFunc();
    }

    return heap->state_idx - 1;
}

int32 xMemPushBase()
{
    return xMemPushBase(gActiveHeap);
}

int32 xMemPopBase(uint32 heapID, int32 depth)
{
    xMemHeap* heap = &gxHeap[heapID];

    heap->state_idx = (depth < 0) ? heap->state_idx + depth : depth;

    if (sMemBaseNotifyFunc)
    {
        sMemBaseNotifyFunc();
    }

    return heap->state_idx;
}

int32 xMemPopBase(int32 depth)
{
    return xMemPopBase(gActiveHeap, depth);
}

int32 xMemGetBase(uint32 heapID)
{
    return gxHeap[heapID].state_idx;
}

void xMemRegisterBaseNotifyFunc(xMemBaseNotifyCallback func)
{
    sMemBaseNotifyFunc = func;
}

int32 xMemGetBase()
{
    return xMemGetBase(gActiveHeap);
}