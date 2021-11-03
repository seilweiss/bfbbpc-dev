#ifndef ZCONDITIONAL_H
#define ZCONDITIONAL_H

#include "xBase.h"

struct zCondAsset : xBaseAsset
{
	uint32 constNum;
	uint32 expr1;
	uint32 op;
	uint32 value_asset;
};

typedef struct _zConditional : xBase
{
	zCondAsset* asset;
} zConditional;

void zConditionalInit(void* b, void* asset);

#endif