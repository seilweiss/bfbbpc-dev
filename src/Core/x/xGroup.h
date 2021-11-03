#ifndef XGROUP_H
#define XGROUP_H

#include "xBase.h"

struct xGroupAsset : xBaseAsset
{
	uint16 itemCount;
	uint16 groupFlags;
};

struct xGroup : xBase
{
	xGroupAsset* asset;
	xBase** item;
	uint32 last_index;
	int32 flg_group;
};

void xGroupInit(void* b, void* asset);
void xGroupSetup(xGroup* g);
uint32 xGroupGetCount(xGroup* g);
xBase* xGroupGetItemPtr(xGroup* g, uint32 index);

#endif