#include "xFX.h"

RpLight* MainLight;
uint32 xfx_initted = 0;

static void LightResetFrame(RpLight* light);

void xFXInit()
{
	if (!xfx_initted)
	{
		xfx_initted = 1;

		RpLight* light = RpLightCreate(rpLIGHTDIRECTIONAL);

		if (light)
		{
			RwFrame* frame = RwFrameCreate();

			if (frame)
			{
				RpLightSetFrame(light, frame);
				LightResetFrame(light);

				MainLight = light;
			}
			else
			{
				RpLightDestroy(light);
			}
		}
		
		xFXanimUVCreate();
		xFXAuraInit();
	}
}

static void LightResetFrame(RpLight* light)
{
	RwV3d xaxis = { 1, 0, 0 };
	RwV3d yaxis = { 0, 1, 0 };

	RwFrame* frame = RpLightGetFrame(light);

	RwFrameRotate(frame, &xaxis, 45.0f, rwCOMBINEREPLACE);
	RwFrameRotate(frame, &yaxis, 45.0f, rwCOMBINEPOSTCONCAT);
}

STUB void xFX_SceneEnter(RpWorld* world)
{

}

STUB void xFXPreAllocMatFX(RpClump* clump)
{

}

STUB uint32 xFXanimUVCreate()
{
	return TRUE;
}

STUB void xFXFireworksInit(const char* fireworksTrailEmitter, const char* fireworksEmitter1, const char* fireworksEmitter2, const char* fireworksSound, const char* fireworksLaunchSound)
{

}

STUB void xFXStreakInit()
{

}

STUB void xFXShineInit()
{

}

STUB void xFXRibbonSceneEnter()
{

}

STUB void xFXAuraInit()
{

}

STUB void xFXStartup()
{

}

STUB void xFXShutdown()
{

}

STUB void xFXSceneInit()
{

}

STUB void xFXSceneSetup()
{

}

STUB void xFXScenePrepare()
{

}

STUB void xFXSceneFinish()
{

}