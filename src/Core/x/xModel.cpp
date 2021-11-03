#include "xModel.h"

int32 xModelPipeNumTables = 0;
int32 xModelPipeCount[16];
xModelPipeInfo* xModelPipeData[16];
int32 xModelLookupCount = 0;
xModelPipeLookup* xModelLookupList = NULL;
bool32 xModelInstStaticAlloc = FALSE;

STUB void xModelInit()
{

}

STUB void xModelPoolInit(uint32 count, uint32 numMatrices)
{

}

STUB xModelInstance* xModelInstanceAlloc(RpAtomic* data, void* object, uint16 flags, uint8 boneIndex, uint8* boneRemap)
{
	// r30 = 0

	int32 i;
	uint32 boneCount;
	uint32 matCount;
	xModelPool* curr;
	xModelPool* found;
	xModelInstance* dude;
	RwMatrix* allocmats;

	boneCount = iModelNumBones(data);
	matCount = boneCount + ();
}

STUB void xModelInstanceAttach(xModelInstance* inst, xModelInstance* parent)
{

}

STUB void xModelEval(xModelInstance* modelInst)
{

}

STUB void xModel_SceneExit(RpWorld* world)
{

}