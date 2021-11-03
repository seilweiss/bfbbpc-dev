#ifndef XMODELBUCKET_H
#define XMODELBUCKET_H

#include "xModel.h"

struct xModelBucket
{
	RpAtomic* Data;
	RpAtomic* OriginalData;
	xModelInstance* List;
	int32 ClipFlags;
	uint32 PipeFlags;
};

void xModelBucket_PreCountReset();
void xModelBucket_PreCountBucket(RpAtomic* data, uint32 pipeFlags, uint32 subObjects);
void xModelBucket_PreCountAlloc(int32 maxAlphaModels);
void xModelBucket_InsertBucket(RpAtomic* data, uint32 pipeFlags, uint32 subObjects);
void xModelBucket_Init();
void xModelBucket_RenderAlphaBegin();
void xModelBucket_RenderAlphaLayer(int32 maxLayer);
void xModelBucket_RenderAlphaEnd();
void xModelBucket_Deinit();

inline void xModelBucket_RenderAlpha()
{
    xModelBucket_RenderAlphaBegin();
    xModelBucket_RenderAlphaLayer(31);
    xModelBucket_RenderAlphaEnd();
}

#endif