#include "zEnt.h"

#include "zScene.h"
#include "zAnimList.h"
#include "zMain.h"
#include "xstransvc.h"

UNCHECKED void zEntInit(zEnt* ent, xEntAsset* asset, uint32 type)
{
	xEntInit(ent, asset);

	ent->update = (xEntUpdateCallback)zEntUpdate;

	if (type == 'PLYR')
	{
		ent->collType = XENT_COLLTYPE_PLYR;
		ent->collLev = 4;
		ent->bound.type = XBOUND_TYPE_SPHERE;

		zEntParseModelInfo(ent, asset->modelInfoID);
	}
	else if (type == 'VIL ')
	{
		ent->collType = XENT_COLLTYPE_NPC;
		ent->collLev = 4;
		ent->bound.type = XBOUND_TYPE_SPHERE;
		ent->moreFlags |= 0x10;

		zEntParseModelInfo(ent, asset->modelInfoID);
	}
	else if (type == 'ITEM')
	{
		ent->collType = XENT_COLLTYPE_STAT;
		ent->collLev = 4;
		ent->bound.type = XBOUND_TYPE_SPHERE;
		ent->eventFunc = NULL;

		zEntParseModelInfo(ent, asset->modelInfoID);
	}
	else if (type == 'PKUP')
	{
		ent->collType = XENT_COLLTYPE_NONE;
		ent->bound.type = XBOUND_TYPE_NA;
	}
	else if (type == 'PLAT')
	{
		ent->collType = XENT_COLLTYPE_DYN;

		if (asset->moreFlags & 0x2)
		{
			ent->collLev = 5;
		}
		else
		{
			ent->collLev = 4;
		}

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.type = XBOUND_TYPE_OBB;
		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}
	else if (type == 'PEND')
	{
		ent->collType = XENT_COLLTYPE_DYN;

		if (asset->moreFlags & 0x2)
		{
			ent->collLev = 5;
		}
		else
		{
			ent->collLev = 4;
		}

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.type = XBOUND_TYPE_OBB;
		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}
	else if (type == 'TRIG')
	{
		ent->collType = XENT_COLLTYPE_TRIG;
		ent->bound.type = XBOUND_TYPE_NA;
	}
	else if (type == 'HANG')
	{
		ent->collType = XENT_COLLTYPE_DYN;
		ent->collLev = 4;
		ent->bound.type = XBOUND_TYPE_SPHERE;

		zEntParseModelInfo(ent, asset->modelInfoID);
	}
	else if (type == 'SIMP')
	{
		ent->collType = XENT_COLLTYPE_STAT;

		if (asset->moreFlags & 0x2)
		{
			ent->collLev = 5;
		}
		else
		{
			ent->collLev = 4;
		}

		ent->bound.type = XBOUND_TYPE_OBB;

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}
	else if (type == 'UI  ')
	{
	}
	else if (type == 'BUTN')
	{
		ent->collType = XENT_COLLTYPE_DYN;

		if (asset->moreFlags & 0x2)
		{
			ent->collLev = 5;
		}
		else
		{
			ent->collLev = 4;
		}

		ent->bound.type = XBOUND_TYPE_OBB;
		ent->moreFlags |= 0x10;

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}
	else if (type == 'DSTR')
	{
		ent->collType = XENT_COLLTYPE_DYN;

		if (asset->moreFlags & 0x2)
		{
			ent->collLev = 5;
		}
		else
		{
			ent->collLev = 4;
		}

		ent->bound.type = XBOUND_TYPE_OBB;
		ent->moreFlags |= 0x10;

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}
	else if (type == 'EGEN')
	{
		ent->collType = XENT_COLLTYPE_DYN;

		if (asset->moreFlags & 0x2)
		{
			ent->collLev = 5;
		}
		else
		{
			ent->collLev = 4;
		}

		ent->bound.type = XBOUND_TYPE_OBB;

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}
	else if (type == 'TBOX')
	{
		ent->collType = XENT_COLLTYPE_STAT;
		ent->collLev = 5;
		ent->bound.type = XBOUND_TYPE_OBB;

		zEntParseModelInfo(ent, asset->modelInfoID);

		ent->bound.mat = (xMat4x3*)ent->model->Mat;
	}

	if (asset->animListID != 0)
	{
		int32 num_used = zAnimListGetNumUsed(asset->animListID);

		if (num_used > 0)
		{
			ent->atbl = zAnimListGetTable(asset->animListID);

			xAnimPoolAlloc(&globals.sceneCur->mempool, ent, ent->atbl, ent->model);

			xAnimState* ast = xAnimTableGetState(ent->atbl, "idle");

			if (ast)
			{
				xAnimSingle* single = ent->model->Anim->Single;

				single->State = ast;
				single->Time = 0.0f;
				single->CurrentSpeed = 1.0f;

				xModelEval(ent->model);
			}
		}
		else
		{
			ent->atbl = NULL;
		}
	}
	else
	{
		ent->atbl = NULL;
	}

	xEntInitForType(ent);
}

void zEntUpdate(zEnt* ent, zScene* scene, float32 elapsedSec)
{
	xEntUpdate(ent, scene, elapsedSec);
}

STUB void zEntEventAll(xBase* from, uint32 fromEvent, uint32 toEvent, float32* toParam)
{

}

STUB void zEntEventAllOfType(uint32 toEvent, uint32 type)
{

}

UNCHECKED xModelInstance* zEntRecurseModelInfo(void* info, xEnt* ent)
{
	uint32 i;
	uint32 bufsize;
	RpAtomic* imodel;
	xModelInstance* tempInst[64];
	xModelAssetInfo* zinfo = (xModelAssetInfo*)info;
	xModelAssetInst* zinst = (xModelAssetInst*)(zinfo + 1);

	for (i = 0; i < zinfo->NumModelInst; i++)
	{
		info = xSTFindAsset(zinst[i].ModelID);

		if (*(uint32*)info == 'FNIM')
		{
			tempInst[i] = zEntRecurseModelInfo(info, ent);

			if (i != 0)
			{
				tempInst[i]->Flags |= zinst[i].Flags;
				tempInst[i]->BoneIndex = zinst[i].Bone;

				xModelInstanceAttach(tempInst[i], tempInst[zinst[i].Parent]);
			}
		}
		else
		{
			imodel = (RpAtomic*)info;

			if (i == 0)
			{
				tempInst[i] = xModelInstanceAlloc(imodel, ent, 0, 0, NULL);
				tempInst[i]->modelID = zinst[i].ModelID;

				while (imodel = iModelFile_RWMultiAtomic(imodel))
				{
					xModelInstanceAttach(xModelInstanceAlloc(imodel, ent, 0x2000, 0, NULL), tempInst[i]);
				}
			}
			else
			{
				tempInst[i] = xModelInstanceAlloc(imodel, ent, zinst[i].Flags, zinst[i].Bone, NULL);

				xModelInstanceAttach(tempInst[i], tempInst[zinst[i].Parent]);

				while (imodel = iModelFile_RWMultiAtomic(imodel))
				{
					xModelInstanceAttach(xModelInstanceAlloc(imodel, ent, 0x2000, 0, NULL), tempInst[i]);
				}
			}
		}
	}

	if (zinfo->AnimTableID != 0)
	{
		xAnimTable* table = (xAnimTable*)xSTFindAsset(zinfo->AnimTableID, &bufsize);

		tempInst[0]->Anim = xAnimPoolAlloc(&globals.sceneCur->mempool, ent, table, tempInst[0]);
	}
}

UNCHECKED void zEntParseModelInfo(xEnt* ent, uint32 assetID)
{
	uint32 bufsize;
	void* info;

	info = xSTFindAsset(assetID, &bufsize);

	if (*(uint32*)info == 'FNIM')
	{
		ent->model = zEntRecurseModelInfo(info, ent);
	}
	else
	{
		xEntLoadModel(ent, (RpAtomic*)info);

		ent->model->modelID = assetID;
	}
}