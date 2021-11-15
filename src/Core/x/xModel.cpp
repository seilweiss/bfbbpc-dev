#include "xModel.h"

#include "xMemMgr.h"
#include "xModelBucket.h"

static xModelPool* sxModelPoolList = NULL;
static RwCamera* subcamera = NULL;

int32 xModelPipeNumTables = 0;
int32 xModelPipeCount[16];
xModelPipeInfo* xModelPipeData[16];
int32 xModelLookupCount = 0;
xModelPipeLookup* xModelLookupList = NULL;
bool32 xModelInstStaticAlloc = FALSE;

static RwCamera* CameraCreate(int32 width, int32 height, bool32 zbuffer);
static void CameraDestroy(RwCamera* camera);

UNCHECKED uint32 xModelGetPipeFlags(RpAtomic* model)
{
	int32 i;

	for (i = 0; i < xModelLookupCount; i++)
	{
		if (xModelLookupList[i].model == model)
		{
			return xModelLookupList[i].PipeFlags;
		}
	}

	return 0;
}

UNCHECKED void xModelInit()
{
	iModelInit();

	sxModelPoolList = NULL;

	if (!subcamera)
	{
		subcamera = CameraCreate(0, 0, TRUE);
	}

	xModelPipeNumTables = 0;
}

UNCHECKED void xModelPoolInit(uint32 count, uint32 numMatrices)
{
	int32 i;
	uint8* buffer;
	RwMatrix* mat;
	xModelPool* pool;
	xModelPool* curr;
	xModelPool** prev;
	xModelInstance* inst;

	if (numMatrices < 1)
	{
		numMatrices = 1;
	}

	buffer = (uint8*)xMEMALLOCALIGN(count * numMatrices * sizeof(RwMatrix) +
									count * sizeof(xModelInstance) +
									sizeof(xModelPool), 16);

	mat = (RwMatrix*)buffer;
	pool = (xModelPool*)(mat + count * numMatrices);
	inst = (xModelInstance*)(pool + 1);

	for (i = 0; i < count; i++)
	{
		inst[i].Next = &inst[i + 1];
		inst[i].Pool = pool;
		inst[i].Mat = mat;

		mat += numMatrices;
	}

	inst[count - 1].Next = NULL;
	
	pool->NumMatrices = numMatrices;
	pool->List = inst;

	curr = sxModelPoolList;
	prev = &sxModelPoolList;

	while (curr && numMatrices < curr->NumMatrices)
	{
		prev = &curr->Next;
		curr = curr->Next;
	}

	pool->Next = curr;

	*prev = pool;
}

UNCHECKED xModelInstance* xModelInstanceAlloc(RpAtomic* data, void* object, uint16 flags, uint8 boneIndex, uint8* boneRemap)
{
	int32 i;
	uint32 boneCount;
	uint32 matCount;
	xModelPool* curr;
	xModelPool* found = NULL;
	xModelInstance* dude;
	RwMatrix* allocmats;

	boneCount = iModelNumBones(data);
	matCount = boneCount + ((flags & 0x40) >> 6) + 1;

	if (xModelInstStaticAlloc)
	{
		if (flags & 0x2000)
		{
			dude = (xModelInstance*)xMEMALLOC(sizeof(xModelInstance));
			
			dude->Pool = NULL;
			dude->Mat = NULL;
		}
		else
		{
			allocmats = (RwMatrix*)xMEMALLOCALIGN(matCount * sizeof(RwMatrix) + sizeof(xModelInstance), 16);
			dude = (xModelInstance*)(allocmats + matCount);
			
			dude->Pool = NULL;
			dude->Mat = allocmats;
		}
	}
	else
	{
		if (flags & 0x2000)
		{
			flags &= ~0x2000;
			flags |= 0x8;
		}

		curr = sxModelPoolList;

		while (curr)
		{
			if (curr->List != NULL && matCount < curr->NumMatrices)
			{
				found = curr;
			}
		}

		if (!found)
		{
			return NULL;
		}

		dude = found->List;
		found->List = dude->Next;
	}

	dude->Next = NULL;
	dude->Parent = NULL;
	dude->Anim = NULL;
	dude->Data = data;
	dude->Object = object;
	dude->Flags = flags | 0x3;
	dude->BoneCount = boneCount;
	dude->BoneIndex = boneIndex;
	dude->BoneRemap = boneRemap;
	dude->modelID = 0;
	dude->shadowID = 0xDEADBEEF;
	dude->Scale.x = 0.0f;
	dude->Scale.y = 0.0f;
	dude->Scale.z = 0.0f;
	dude->RedMultiplier = 1.0f;
	dude->GreenMultiplier = 1.0f;
	dude->BlueMultiplier = 1.0f;
	dude->Alpha = data->geometry->matList.materials[0]->color.alpha / 255.0f;
	dude->Surf = NULL;
	dude->FadeStart = 9e+37f;
	dude->FadeEnd = 1e+38f;

	if (dude->Mat)
	{
		for (i = 0; i < matCount; i++)
		{
			xMat4x3Identity((xMat4x3*)&dude->Mat[i]);
		}
	}

	dude->Bucket = xModelBucket_GetBuckets(dude->Data);
	dude->LightKit = NULL;

	if (dude->Bucket)
	{
		dude->PipeFlags = dude->Bucket[0]->PipeFlags;
	}
	else
	{
		dude->PipeFlags = xModelGetPipeFlags(dude->Data);
	}

	if (!(dude->PipeFlags & 0xf80000))
	{
		dude->PipeFlags |= 0x980000;
	}

	dude->anim_coll.verts = NULL;

	return dude;
}

STUB void xModelInstanceAttach(xModelInstance* inst, xModelInstance* parent)
{

}

STUB void xModelEval(xModelInstance* modelInst)
{

}

UNCHECKED static RwCamera* CameraCreate(int32 width, int32 height, bool32 zbuffer)
{
	RwCamera* camera;

	camera = RwCameraCreate();

	if (camera)
	{
		RwCameraSetFrame(camera, RwFrameCreate());
		RwCameraSetRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA));

		if (zbuffer)
		{
			RwCameraSetZRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER));
		}

		if (RwCameraGetFrame(camera) != NULL &&
			RwCameraGetRaster(camera) != NULL &&
			(!zbuffer || RwCameraGetZRaster(camera) != NULL))
		{
			return camera;
		}
	}

	CameraDestroy(camera);

	return NULL;
}

UNCHECKED static void CameraDestroy(RwCamera* camera)
{
	RwRaster* raster;
	RwFrame* frame;
	
	if (camera)
	{
		_rwFrameSyncDirty();

		frame = RwCameraGetFrame(camera);

		if (frame)
		{
			RwCameraSetFrame(camera, NULL);
			RwFrameDestroy(frame);
		}

		raster = RwCameraGetRaster(camera);

		if (raster)
		{
			RwRasterDestroy(raster);
			RwCameraSetRaster(camera, NULL);
		}

		raster = RwCameraGetZRaster(camera);

		if (raster)
		{
			RwRasterDestroy(raster);
			RwCameraSetZRaster(camera, NULL);
		}

		RwCameraDestroy(camera);
	}
}

STUB void xModel_SceneExit(RpWorld* world)
{

}