#include "zCameraTweak.h"

#include "zCamera.h"

static int32 sCamTweakCount;
static zCamTweak sCamTweakList[8];
static float32 sCamTweakLerp;
static float32 sCamTweakTime;
static float32 sCamTweakPitch[2];
static float32 sCamTweakDistMult[2];
static float32 sCamTweakPitchCur;
static float32 sCamTweakDistMultCur;
static float32 sCamD;
static float32 sCamH;
static float32 sCamPitch;
static zCamTweakLook zcam_neartweak;
static zCamTweakLook zcam_fartweak;

static void zCameraTweak_LookPreCalc(zCamTweakLook* tlook, float32 d, float32 h, float32 pitch)
{
	float32 f1 = d * itan(pitch);

	tlook->h = h - f1;
	tlook->dist = xsqrt(SQR(f1) + SQR(pitch));
	tlook->pitch = pitch;
}

void zCameraTweakGlobal_Init()
{
	zCameraTweak_LookPreCalc(&zcam_neartweak, zcam_near_d, zcam_near_h, zcam_near_pitch);
	zCameraTweak_LookPreCalc(&zcam_fartweak, zcam_far_d, zcam_far_h, zcam_far_pitch);
	
	zCameraTweakGlobal_Reset();
}

void zCameraTweakGlobal_Reset()
{
	sCamTweakCount = 0;
	sCamTweakPitch[0] = 0.0f;
	sCamTweakPitch[1] = 0.0f;
	sCamTweakDistMult[0] = 1.0f;
	sCamTweakDistMult[1] = 1.0f;
	sCamTweakTime = 0.1f;
	sCamTweakLerp = 0.0f;
	sCamTweakPitchCur = 0.0f;
	sCamTweakDistMultCur = 1.0f;
}

STUB void zCameraTweak_Init(xBase& data, xDynAsset& asset, ulong32)
{

}