#ifndef XMEMMGR_H
#define XMEMMGR_H

#include <types.h>

typedef struct xMemArea_tag
{
	uint32 addr;
	uint32 size;
	uint32 flags;
} xMemArea;

typedef struct xMemInfo_tag
{
	xMemArea system;
	xMemArea stack;
	xMemArea DRAM;
	xMemArea SRAM;
} xMemInfo;

typedef struct xHeapState_tag
{
	uint32 curr;
	uint16 blk_ct;
	uint16 pad;
	uint32 used;
	uint32 wasted;
} xHeapState;

typedef struct xMemBlock_tag
{
	uint32 addr;
	uint32 size;
	int32 align;
} xMemBlock;

typedef struct xMemHeap_tag
{
	uint32 flags;
	uint32 hard_base;
	uint32 size;
	int16 opp_heap[2];
	xHeapState state[12];
	uint16 state_idx;
	uint16 max_blks;
	xMemBlock* blk;
	xMemBlock* lastblk;
} xMemHeap;

#define XMEMHEAP_UNK0x100 0x100
#define XMEMHEAP_UNK0x10000 0x10000
#define XMEMHEAP_ALIGN(x) ((flags) & 0x1F << 9)
#define XMEMHEAP_GETALIGN(flags) ((flags) >> 9 & 0x1F)
#define XMEMHEAP_MAXBLOCKS(x) ((flags) & 0x1 << 14)
#define XMEMHEAP_GETMAXBLOCKS(flags) ((flags) >> 14 & 0x1)

struct xMemPool;

typedef void(*xMemPoolInitCallback)(xMemPool*, void*);

struct xMemPool
{
	void* FreeList;
	uint16 NextOffset;
	uint16 Flags;
	void* UsedList;
	xMemPoolInitCallback InitCB;
	void* Buffer;
	uint16 Size;
	uint16 NumRealloc;
	uint32 Total;
};

extern xMemInfo gMemInfo;
extern uint32 gActiveHeap;
extern xMemHeap gxHeap[3];

typedef void(*xMemBaseNotifyCallback)();

void xMemInit();
void xMemExit();
void xMemInitHeap(xMemHeap* heap, uint32 base, uint32 size, uint32 flags);
void* xMemAlloc(uint32 heapID, uint32 size, int32 align);
void* xMemPushTemp(uint32 size);
void xMemPopTemp(void* addr);
int32 xMemPushBase(uint32 heapID);
int32 xMemPushBase();
int32 xMemPopBase(uint32 heapID, int32 depth);
int32 xMemPopBase(int32 depth);
int32 xMemGetBase(uint32 heapID);
void xMemRegisterBaseNotifyFunc(xMemBaseNotifyCallback func);
int32 xMemGetBase();

#define xMEMALLOC(size) xMemAlloc(gActiveHeap, (size), 0)
#define xMEMALLOCALIGN(size, align) xMemAlloc(gActiveHeap, (size), (align))

#endif