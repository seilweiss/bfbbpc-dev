#ifndef XMODEL_H
#define XMODEL_H

#include "iModel.h"
#include "xMath3.h"
#include "xLightKit.h"

#include <rwcore.h>
#include <rpworld.h>

struct xModelPool;
struct xModelBucket;
struct xAnimPlay;
struct xSurface;

struct xModelInstance
{
	xModelInstance* Next;
	xModelInstance* Parent;
	xModelPool* Pool;
	xAnimPlay* Anim;
	RpAtomic* Data;
	uint32 PipeFlags;
	float32 RedMultiplier;
	float32 GreenMultiplier;
	float32 BlueMultiplier;
	float32 Alpha;
	float32 FadeStart;
	float32 FadeEnd;
	xSurface* Surf;
	xModelBucket** Bucket;
	xModelInstance* BucketNext;
	xLightKit* LightKit;
	void* Object;
	uint16 Flags;
	uint8 BoneCount;
	uint8 BoneIndex;
	uint8* BoneRemap;
	RwMatrix* Mat;
	xVec3 Scale;
	uint32 modelID;
	uint32 shadowID;
	RpAtomic* shadowmapAtomic;

	struct
	{
		xVec3* verts;
	} anim_coll;
};

struct xModelPool
{
	xModelPool* Next;
	uint32 NumMatrices;
	xModelInstance* List;
};

struct xModelTag
{
	xVec3 v;
	uint32 matidx;
	float32 wt[4];
};

struct xModelPipeInfo
{
	uint32 ModelHashID;
	uint32 SubObjectBits;
	uint32 PipeFlags;
};

struct xModelPipeLookup
{
	RpAtomic* model;
	uint32 PipeFlags;
};

struct xModelAssetInfo
{
	uint32 Magic;
	uint32 NumModelInst;
	uint32 AnimTableID;
	uint32 CombatID;
	uint32 BrainID;
};

struct xModelAssetInst
{
	uint32 ModelID;
	uint16 Flags;
	uint8 Parent;
	uint8 Bone;
	float32 MatRight[3];
	float32 MatUp[3];
	float32 MatAt[3];
	float32 MatPos[3];
};

extern int32 xModelPipeNumTables;
extern int32 xModelPipeCount[16];
extern xModelPipeInfo* xModelPipeData[16];
extern int32 xModelLookupCount;
extern xModelPipeLookup* xModelLookupList;
extern bool32 xModelInstStaticAlloc;

void xModelInit();
void xModelPoolInit(uint32 count, uint32 numMatrices);
xModelInstance* xModelInstanceAlloc(RpAtomic* data, void* object, uint16 flags, uint8 boneIndex, uint8* boneRemap);
void xModelInstanceAttach(xModelInstance* inst, xModelInstance* parent);
void xModelEval(xModelInstance* modelInst);
void xModel_SceneExit(RpWorld* world);

inline xMat4x3* xModelGetFrame(xModelInstance* modelInst)
{
	return (xMat4x3*)modelInst->Mat;
}

#endif