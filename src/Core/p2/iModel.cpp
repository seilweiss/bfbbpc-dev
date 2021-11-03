#include "iModel.h"

#include "xModel.h"

#include <rphanim.h>
#include <rpskin.h>
#include <rpusrdat.h>

UNCHECKED static RwFrame* GetChildFrameHierarchy(RwFrame* frame, void* data)
{
	RpHAnimHierarchy* hierarchy = RpHAnimFrameGetHierarchy(frame);

	if (!hierarchy)
	{
		RwFrameForAllChildren(frame, GetChildFrameHierarchy, data);
	}
	else
	{
		*(RpHAnimHierarchy**)data = hierarchy;

		return NULL;
	}

	return frame;
}

UNCHECKED static RpHAnimHierarchy* GetHierarchy(RpAtomic* model)
{
	RpHAnimHierarchy* hierarchy = NULL;

	GetChildFrameHierarchy(RpAtomicGetFrame(model), &hierarchy);

	return hierarchy;
}

UNCHECKED static RpAtomic* NextAtomicCallback(RpAtomic* atomic, void* data)
{
	RpAtomic** nextModel = (RpAtomic**)data;

	if (*nextModel == atomic)
	{
		*nextModel = NULL;
	}
	else if (!*nextModel)
	{
		*nextModel = atomic;
	}

	return atomic;
}

UNCHECKED RpAtomic* iModelFile_RWMultiAtomic(RpAtomic* model)
{
	RpClump* clump;
	RpAtomic* nextModel;

	if (!model)
	{
		return NULL;
	}

	clump = RpAtomicGetClump(model);
	nextModel = model;

	RpClumpForAllAtomics(clump, NextAtomicCallback, &nextModel);

	return nextModel;
}

UNCHECKED uint32 iModelNumBones(RpAtomic* model)
{
	RpHAnimHierarchy* hierarchy = GetHierarchy(model);

	if (hierarchy)
	{
		return hierarchy->numNodes;
	}

	return 0;
}

UNCHECKED static uint32 iModelTagUserData(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z, int32 closeV)
{
	int32 i;
	int32 count;
	RpUserDataArray* array;
	RpUserDataArray* testarray;
	float32 distSqr;
	float32 closeDistSqr;
	int32 numTags;
	int32 t;
	xModelTag* tagList;

	count = RpGeometryGetUserDataArrayCount(model->geometry);
	array = NULL;

	for (i = 0; i < count; i++)
	{
		testarray = RpGeometryGetUserDataArray(model->geometry, i);

		if (strcmp(testarray->name, "HI_Tags") == 0)
		{
			array = testarray;
			break;
		}
	}

	if (!array)
	{
		memset(tag, 0, sizeof(xModelTag));
		return 0;
	}

	closeDistSqr = 1000000000.0f;
	numTags = *(int32*)array->data;
	tagList = (xModelTag*)((int32*)array->data + 1);

	if (closeV < 0 || closeV > numTags)
	{
		closeV = 0;

		for (t = 0; t < numTags; t++)
		{
			distSqr = SQR(tagList[t].v.x - x) + SQR(tagList[t].v.y - y) + SQR(tagList[t].v.z - z);

			if (distSqr < closeDistSqr)
			{
				closeDistSqr = distSqr;
				closeV = t;
			}
		}

		if (tag)
		{
			*tag = tagList[closeV];
		}
	}
	else
	{
		if (tag)
		{
			*tag = tagList[closeV];
		}
	}

	return closeV;
}

UNCHECKED static uint32 iModelTagInternal(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z, int32 closeV)
{
	RpGeometry* geom;
	RwV3d* vert;
	int32 v;
	int32 numV;
	float32 distSqr;
	float32 closeDistSqr;
	RpSkin* skin;
	const RwMatrixWeights* wt;
	
	geom = model->geometry;
	vert = geom->morphTarget->verts;

	if (!vert)
	{
		return iModelTagUserData(tag, model, x, y, z, closeV);
	}

	numV = geom->numVertices;
	closeDistSqr = 1000000000.0f;

	if (closeV < 0 || closeV > numV)
	{
		closeV = 0;

		for (v = 0; v < numV; v++)
		{
			distSqr = SQR(vert[v].x - x) + SQR(vert[v].y - y) + SQR(vert[v].z - z);

			if (distSqr < closeDistSqr)
			{
				closeDistSqr = distSqr;
				closeV = v;
			}
		}

		if (tag)
		{
			tag->v.x = x;
			tag->v.y = y;
			tag->v.z = z;
		}
	}
	else
	{
		if (tag)
		{
			tag->v.x = vert[closeV].x;
			tag->v.y = vert[closeV].y;
			tag->v.z = vert[closeV].z;
		}
	}

	if (tag)
	{
		skin = RpSkinGeometryGetSkin(model->geometry);

		if (skin)
		{
			wt = &RpSkinGetVertexBoneWeights(skin)[closeV];

			tag->matidx = RpSkinGetVertexBoneIndices(skin)[closeV];
			tag->wt[0] = wt->w0;
			tag->wt[1] = wt->w1;
			tag->wt[2] = wt->w2;
			tag->wt[3] = wt->w3;
		}
	}

	return closeV;
}

uint32 iModelTagSetup(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z)
{
	return iModelTagInternal(tag, model, x, y, z, -1);
}