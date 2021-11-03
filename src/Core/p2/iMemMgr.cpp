#include "iMemMgr.h"

#include "xMemMgr.h"

#ifdef _WIN32
#include <windows.h>

#define DEFAULT_HEAP_SIZE (16 << 20)

static HANDLE sHeap;
static size_t sHeapSize;
static size_t sMemBaseAlloc;
static size_t sMemTopAlloc;
#endif

void iMemInit()
{
#ifdef _WIN32
	sHeap = GetProcessHeap();

	if (!sHeap)
	{
		exit(-5);
	}

	gMemInfo.system.addr = NULL;
	gMemInfo.system.size = 16;
	gMemInfo.system.flags = 0x20;

	ULONG_PTR stack_begin, stack_end;
	GetCurrentThreadStackLimits(&stack_begin, &stack_end);

	gMemInfo.stack.addr = stack_begin;
	gMemInfo.stack.size = stack_end - stack_begin;
	gMemInfo.stack.flags = 0x820;

	sHeapSize = DEFAULT_HEAP_SIZE;

	sMemBaseAlloc = (size_t)HeapAlloc(sHeap, HEAP_GENERATE_EXCEPTIONS, sHeapSize);
	sMemTopAlloc = sMemBaseAlloc + sHeapSize;

	gMemInfo.DRAM.addr = sMemBaseAlloc;
	gMemInfo.DRAM.size = sHeapSize;
	gMemInfo.DRAM.flags = 0x820;

	gMemInfo.SRAM.addr = NULL;
	gMemInfo.SRAM.size = 32;
	gMemInfo.SRAM.flags = 0x660;
#endif
}

void iMemExit()
{
#ifdef _WIN32
	HeapFree(sHeap, 0, (void*)gMemInfo.DRAM.addr);

	gMemInfo.DRAM.addr = NULL;
#endif
}