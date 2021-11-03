#ifndef XANIM_H
#define XANIM_H

#include "xModel.h"
#include "xMemMgr.h"

struct xAnimFile
{
	xAnimFile* Next;
	const char* Name;
	uint32 ID;
	uint32 FileFlags;
	float32 Duration;
	float32 TimeOffset;
	uint16 BoneCount;
	uint8 NumAnims[2];
	void** RawData;
};

struct xAnimMultiFileBase
{
	uint32 Count;
};

struct xAnimMultiFileEntry
{
	uint32 ID;
	xAnimFile* File;
};

struct xAnimMultiFile : xAnimMultiFileBase
{
	xAnimMultiFileEntry Files[1];
};

struct xAnimEffect;
struct xAnimActiveEffect;
struct xAnimSingle;

typedef uint32(*xAnimEffectCallback)(uint32, xAnimActiveEffect*, xAnimSingle*, void*);

struct xAnimEffect
{
	xAnimEffect* Next;
	uint32 Flags;
	float32 StartTime;
	float32 EndTime;
	xAnimEffectCallback Callback;
};

struct xAnimActiveEffect
{
	xAnimEffect* Effect;
	uint32 Handle;
};

struct xAnimTransition;
struct xAnimTransitionList;

struct xAnimState
{
	xAnimState* Next;
	const char* Name;
	uint32 ID;
	uint32 Flags;
	uint32 UserFlags;
	float32 Speed;
	xAnimFile* Data;
	xAnimEffect* Effects;
	xAnimTransitionList* Default;
	xAnimTransitionList* List;
	float32* BoneBlend;
	float32* TimeSnap;
	float32 FadeRecip;
	uint16* FadeOffset;
	void* CallbackData;
	xAnimMultiFile* MultiFile;
	void(*BeforeEnter)(xAnimPlay*, xAnimState*);
	void(*StateCallback)(xAnimState*, xAnimSingle*, void*);
	void(*BeforeAnimMatrices)(xAnimPlay*, xQuat*, xVec3*, int32);
};

typedef uint32(*xAnimTransitionCallback)(xAnimTransition*, xAnimSingle*, void*);

struct xAnimTransition
{
	xAnimTransition* Next;
	xAnimState* Dest;
	xAnimTransitionCallback Conditional;
	xAnimTransitionCallback Callback;
	uint32 Flags;
	uint32 UserFlags;
	float32 SrcTime;
	float32 DestTime;
	uint16 Priority;
	uint16 QueuePriority;
	float32 BlendRecip;
	uint16* BlendOffset;
};

struct xAnimTransitionList
{
	xAnimTransitionList* Next;
	xAnimTransition* T;
};

struct xAnimTable
{
	xAnimTable* Next;
	const char* Name;
	xAnimTransition* TransitionList;
	xAnimState* StateList;
	uint32 AnimIndex;
	uint32 MorphIndex;
	uint32 UserFlags;
};

struct xAnimPlay;

struct xAnimSingle
{
	uint32 SingleFlags;
	xAnimState* State;
	float32 Time;
	float32 CurrentSpeed;
	float32 BilinearLerp[2];
	xAnimEffect* Effect;
	uint32 ActiveCount;
	float32 LastTime;
	xAnimActiveEffect* ActiveList;
	xAnimPlay* Play;
	xAnimTransition* Sync;
	xAnimTransition* Tran;
	xAnimSingle* Blend;
	float32 BlendFactor;
	uint32 pad;
};

struct xAnimPlay
{
	xAnimPlay* Next;
	uint16 NumSingle;
	uint16 BoneCount;
	xAnimSingle* Single;
	void* Object;
	xAnimTable* Table;
	xMemPool* Pool;
	xModelInstance* ModelInst;
	void(*BeforeAnimMatrices)(xAnimPlay*, xQuat*, xVec3*, int32);
};

void xAnimInit();
void xAnimTempTransitionInit(uint32 count);
xAnimState* xAnimTableGetState(xAnimTable* table, const char* name);
void xAnimPoolInit(xMemPool* pool, uint32 count, uint32 singles, uint32 blendFlags, uint32 effectMax);
xAnimPlay* xAnimPoolAlloc(xMemPool* pool, void* object, xAnimTable* table, xModelInstance* modelInst);

#endif