#include "xShadow.h"

#include "xMath.h"

RpLight* ShadowLight;
RwRGBAReal ShadowLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
RwCamera* ShadowCamera;
RwRaster* ShadowCameraRaster;
static RwRaster* ShadowRenderRaster;

static uint32 shadow_ent_count;

static bool32 SetupShadow();
static void ShadowCameraDestroy(RwCamera* shadowCamera);
static RwRaster* ShadowRasterCreate(int32 res);
static RwCamera* ShadowCameraCreatePersp(int32 res);

void xShadowInit()
{
	xShadowCameraCreate();

	shadow_ent_count = 0;
	
	ShadowLight = RpLightCreate(rpLIGHTDIRECTIONAL);

	RpLightSetColor(ShadowLight, &ShadowLightColor);
	RpLightSetFrame(ShadowLight, RwFrameCreate());
}

bool32 xShadowCameraCreate()
{
	return (SetupShadow() != 0);
}

static bool32 SetupShadow()
{
	int32 res = 256;

	while (res > 640 || res > 480)
	{
		res /= 2;
	}

	ShadowCamera = ShadowCameraCreatePersp(res);

	if (!ShadowCamera)
	{
		return FALSE;
	}

	ShadowCameraRaster = ShadowRasterCreate(res);

	if (!ShadowCameraRaster)
	{
		return FALSE;
	}

	RwCameraSetRaster(ShadowCamera, ShadowCameraRaster);

	return TRUE;
}

STUB void xShadowSetWorld(RpWorld* world)
{

}

static void ShadowCameraDestroy(RwCamera* shadowCamera)
{
	if (shadowCamera)
	{
		RwRaster* raster;
		RwFrame* frame;

		_rwFrameSyncDirty();

		frame = RwCameraGetFrame(shadowCamera);

		if (frame)
		{
			RwCameraSetFrame(shadowCamera, NULL);
			RwFrameDestroy(frame);
		}

		raster = RwCameraGetZRaster(shadowCamera);

		if (raster)
		{
			RwCameraSetZRaster(shadowCamera, NULL);
			RwRasterDestroy(raster);
		}

		raster = RwCameraGetRaster(shadowCamera);

		if (raster)
		{
			RwCameraSetRaster(shadowCamera, NULL);
			RwRasterDestroy(raster);
		}

		RwCameraDestroy(shadowCamera);
	}
	
}

static RwRaster* ShadowRasterCreate(int32 res)
{
	return RwRasterCreate(res, res, 0, rwRASTERTYPECAMERATEXTURE);
}

static RwCamera* ShadowCameraCreatePersp(int32 res)
{
	RwCamera* camera = RwCameraCreate();

	if (camera)
	{
		RwCameraSetFrame(camera, RwFrameCreate());

		RwV2d vw;
		vw.x = DEG2RAD(0.1f);
		vw.y = DEG2RAD(0.1f);

		RwCameraSetViewWindow(camera, &vw);

		RwFrame* frame = RwCameraGetFrame(camera);

		if (frame)
		{
			RwRaster* raster = RwRasterCreate(res, res, 0, rwRASTERTYPEZBUFFER);

			if (raster)
			{
				RwCameraSetZRaster(camera, raster);

				return camera;
			}
		}
	}

	ShadowCameraDestroy(camera);

	return NULL;
}

STUB void xShadowManager_Init(int32 numEnts)
{

}