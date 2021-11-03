#ifndef XBASE_H
#define XBASE_H

#include "xLinkAsset.h"
#include "xserializer.h"

struct xBase;

typedef bool32 (*xBaseEventCallback)(xBase* from, xBase* to, uint32 toEvent, const float32* toParam, xBase* toParamWidget);

struct xBase
{
	uint32 id;
	uint8 baseType;
	uint8 linkCount;
	uint16 baseFlags;
	xLinkAsset* link;
	xBaseEventCallback eventFunc;
};

struct xBaseAsset
{
	uint32 id;
	uint8 baseType;
	uint8 linkCount;
	uint16 baseFlags;
};

#define XBASE_ENABLED 0x1
#define XBASE_PERSISTENT 0x2
#define XBASE_VALID 0x4
#define XBASE_UNK10 0x10
#define XBASE_IS_ENT 0x20

void xBaseInit(xBase* xb, const xBaseAsset* asset);
void xBaseSetup(xBase*);
void xBaseSave(xBase* ent, xSerial* s);
void xBaseLoad(xBase* ent, xSerial* s);
void xBaseReset(xBase* xb, xBaseAsset* asset);

inline bool xBaseIsEnabled(const xBase* xb)
{
	return xb->baseFlags & XBASE_ENABLED;
}

inline uint32 xBaseIsValid(xBase* xb)
{
	return xb->baseFlags & XBASE_VALID;
}

inline void xBaseEnable(xBase* xb)
{
	xb->baseFlags |= XBASE_ENABLED;
}

inline void xBaseDisable(xBase* xb)
{
	xb->baseFlags &= ~XBASE_ENABLED;
}

inline void xBaseValidate(xBase* xb)
{
	xb->baseFlags |= XBASE_VALID;
}

#define xBaseIsEnt(xb) (((xb)->baseFlags & XBASE_IS_ENT) != 0)

#endif