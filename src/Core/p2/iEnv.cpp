#include "iEnv.h"

#include "iCamera.h"
#include "xMemMgr.h"

static RpWorld* sPipeWorld = NULL;
static RwCamera* sPipeCamera = NULL;

UNCHECKED static RpAtomic* SetPipelineCB(RpAtomic* atomic, void* data)
{
	if (RwCameraBeginUpdate(sPipeCamera))
	{
		RpAtomicInstance(atomic);
		RwCameraEndUpdate(sPipeCamera);
	}

	if ((RxPipeline*)data)
	{
		RpAtomicSetPipeline(atomic, (RxPipeline*)data);
	}

	return atomic;
}

UNCHECKED static void iEnvSetBSP(iEnv* env, int32 envDataType, RpWorld* bsp)
{
	if (envDataType == IENV_TYPE_BSP)
	{
		env->world = bsp;
	}
	else if (envDataType == IENV_TYPE_COLLISION)
	{
		env->collision = bsp;
	}
	else if (envDataType == IENV_TYPE_FX)
	{
		env->fx = bsp;
	}
	else if (envDataType == IENV_TYPE_CAMERA)
	{
		env->camera = bsp;
	}
}

UNCHECKED void iEnvLoad(iEnv* env, const void* data, uint32, int32 dataType)
{
	RpWorld* bsp = (RpWorld*)data;
	xJSPHeader* jsp = (xJSPHeader*)data;
	
	if (((char*)data)[0] == 'J' &&
		((char*)data)[1] == 'S' &&
		((char*)data)[2] == 'P' &&
		((char*)data)[3] == '\0')
	{
		if (dataType == IENV_TYPE_BSP)
		{
			RwBBox tmpbbox = { 1000.0f, 1000.0f, 1000.0f, -1000.0f, -1000.0f, -1000.0f };

			env->world = RpWorldCreate(&tmpbbox);

			sPipeCamera = iCameraCreate(640, 480, 0);
			sPipeWorld = env->world;

			RpWorldAddCamera(sPipeWorld, sPipeCamera);

			env->jsp = jsp;

			RpClumpForAllAtomics(env->jsp->clump, SetPipelineCB, NULL);
			xClumpColl_InstancePointers(env->jsp->colltree, env->jsp->clump);

			RpWorldRemoveCamera(sPipeWorld, sPipeCamera);
			iCameraDestroy(sPipeCamera);

			sPipeWorld = NULL;
			sPipeCamera = NULL;
		}
	}
	else
	{
		if (dataType == IENV_TYPE_BSP)
		{
			env->jsp = NULL;
		}

		iEnvSetBSP(env, dataType, bsp);
	}
	
	if (dataType == IENV_TYPE_BSP)
	{
		env->memlvl = xMemGetBase();
	}
}

void iEnvDefaultLighting(iEnv*)
{
}

void iEnvLightingBasics(iEnv*, xEnvAsset*)
{
}