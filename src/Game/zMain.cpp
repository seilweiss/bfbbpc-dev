#include "zMain.h"

#include "iSystem.h"
#include "xMemMgr.h"
#include "zVar.h"
#include "zAssetTypes.h"
#include "xFont.h"
#include "xTRC.h"
#include "iTime.h"
#include "xutil.h"
#include "xserializer.h"
#include "zDispatcher.h"
#include "xScrFx.h"
#include "xFX.h"
#include "xParMgr.h"
#include "zParCmd.h"
#include "iEnv.h"
#include "zEntPickup.h"
#include "zCameraTweak.h"
#include "xPad.h"
#include "xsavegame.h"
#include "xDebug.h"
#include "zShrapnel.h"
#include "xBehaveMgr.h"
#include "zNPCMgr.h"
#include "xstransvc.h"
#include "xIni.h"
#include "iFile.h"
#include "xString.h"
#include "zGame.h"
#include "zUI.h"
#include "xMath.h"
#include "xCamera.h"
#include "zCamera.h"
#include "zEntPlayerBungeeState.h"
#include "zEntPlayerOOBState.h"
#include "zTalkBox.h"
#include "zScene.h"
#include "xShadowSimple.h"
#include "zPickupTable.h"
#include "xModel.h"
#include "zCutsceneMgr.h"
#include "zMenu.h"
#include "zGameState.h"

#include <string.h>

#include <SDL.h>

zGlobals globals;
xGlobals* xglobals = &globals;
bool32 sShowMenuOnBoot = TRUE;
float32 gSkipTimeCutscene = 1.0f;
float32 gSkipTimeFlythrough = 1.0f;

static SERIAL_PERCID_SIZE g_xser_sizeinfo[] =
{
	{ 'PLYR', 0x148 },
	{ 'CNTR', 0x1e0 },
	{ NULL }
};

static void zMainOutputMgrSetup();
static void zMainInitGlobals();
static void zMainParseINIGlobals(xIniFile* ini);
static void zMainMemLvlChkCB();
void zMainShowProgressBar();
static void zMainLoop();
static void zMainReadINI();
void zMainFirstScreen(int32 mode);
void zMainMemCardSpaceQuery();
static void zMainLoadFontHIP();

#define LOAD_SCENE(id, flg_hiphop, debug)                                                          \
{                                                                                                  \
    iTime t;                                                                                       \
                                                                                                   \
    if ((debug))                                                                                   \
    {                                                                                              \
        t = iTimeGet();                                                                            \
        XPRINTF("LOAD_SCENE (%s): Preloading scene (time %f)\n", xUtil_idtag2string((id)), iTimeDiffSec(t));      \
    }                                                                                              \
                                                                                                   \
    xSTPreLoadScene((id), NULL, (flg_hiphop));                                                     \
                                                                                                   \
    if ((debug))                                                                                   \
    {                                                                                              \
        t = iTimeGet();                                                                            \
        XPRINTF("LOAD_SCENE (%s): Queuing scene assets (time %f)\n", xUtil_idtag2string((id)), iTimeDiffSec(t));  \
    }                                                                                              \
                                                                                                   \
    xSTQueueSceneAssets((id), (flg_hiphop));                                                       \
                                                                                                   \
    if ((debug))                                                                                   \
    {                                                                                              \
        t = iTimeGet();                                                                            \
        XPRINTF("LOAD_SCENE (%s): Loading scene (time %f)\n", xUtil_idtag2string((id)), iTimeDiffSec(t));         \
    }                                                                                              \
                                                                                                   \
    while (xSTLoadStep((id)) < 1.0f) {}                                                            \
                                                                                                   \
    xSTDisconnect((id), (flg_hiphop));                                                             \
                                                                                                   \
    if ((debug))                                                                                   \
    {                                                                                              \
        t = iTimeGet();                                                                            \
        XPRINTF("LOAD_SCENE (%s): Done loading (time %f)\n", xUtil_idtag2string((id)), iTimeDiffSec(t));          \
    }                                                                                              \
}

STUB int main(int argc, char** argv)
{
	memset(&globals, 0, sizeof(globals));

	globals.firstStartPressed = true;

	uint32 options = 0;

	iSystemInit(options);
	zMainOutputMgrSetup();
	xMemRegisterBaseNotifyFunc(zMainMemLvlChkCB);

	zMainInitGlobals();

	var_init();
	zAssetStartup();

	zMainLoadFontHIP();

	xfont::init();

	zMainFirstScreen(1);

	zMainShowProgressBar();

	xTRCInit();
	zMainReadINI();
	iFuncProfileParse("scooby.elf", globals.profile);
	xUtilStartup();
	xSerialStartup(128, g_xser_sizeinfo);
	zDispatcher_Startup();
	xScrFxInit();
	xFXStartup();

	zMainShowProgressBar();

	xParMgrInit();
	zParCmdInit();
	iEnvStartup();
	zEntPickup_Startup();
	zCameraTweakGlobal_Init();

	globals.option_vibration = TRUE;
	globals.pad0 = xPadEnable(globals.currentActivePad);
	globals.pad1 = NULL;

	gDebugPad = NULL;

	xPadRumbleEnable(globals.currentActivePad, globals.option_vibration);

	xSGStartup();
	xDebugTimestampScreen();
	zShrapnel_GameInit();

	zMainShowProgressBar();

	xBehaveMgr_Startup();
	zNPCMgr_Startup();

	zMainLoop();

	zNPCMgr_Shutdown();
	xBehaveMgr_Shutdown();
	zAssetShutdown();
	xFXShutdown();
	zDispatcher_Shutdown();
	xSGShutdown();
	xSerialShutdown();
	xUtilShutdown();
	iSystemExit();

	return 0;
}

static void zMainOutputMgrSetup()
{
	XPRINTF("zMainOutputMgrSetup finished at time %f (tick %d)\n", iTimeDiffSec(iTimeGet()), iTimeGet());
}

static void zMainInitGlobals()
{
	memset(&globals, 0, sizeof(globals));

	globals.sceneFirst = TRUE;

	XPRINTF("zMainInitGlobals finished at time %f (tick %d)\n", iTimeDiffSec(iTimeGet()), iTimeGet());
}

static int32 ParseFloatList(float32* dest, char* strbuf, int32 max)
{
	return xStrParseFloatList(dest, strbuf, max);
}

#define xIniGetFloatList(dest, ini, tok, def)\
{\
	static float32 fbuf[] = {def};\
	ParseFloatList((float32*)memcpy((dest), fbuf, sizeof(fbuf)), xIniGetString((ini), (tok)), sizeof(fbuf) / sizeof(float32));\
}

static void zLedgeAdjust(zLedgeGrabParams* ledge)
{
	ledge->animGrab *= 1.0f / 30.0f;
}

NONMATCH static void zMainParseINIGlobals(xIniFile* ini)
{
	globals.player.g.AnalogMin = xIniGetInt(ini, "g.AnalogMin", 32);
	globals.player.g.AnalogMax = xIniGetInt(ini, "g.AnalogMax", 110);

	xScrFxLetterBoxSetSize(xIniGetFloat(ini, "ScrFxLetterBoxSize", 0.0f));
	xScrFxLetterBoxSetAlpha(xIniGetInt(ini, "ScrFxLetterBoxAlpha", 255));

	globals.player.g.InitialShinyCount = xIniGetInt(ini, "g.InitialShinyCount", 0);
	globals.player.g.InitialSpatulaCount = xIniGetInt(ini, "g.InitialSpatulaCount", 0);
	
	globals.player.g.ShinyValuePurple = xIniGetInt(ini, "g.ShinyValuePurple", 100);
	globals.player.g.ShinyValueBlue = xIniGetInt(ini, "g.ShinyValueBlue", 25);
	globals.player.g.ShinyValueGreen = xIniGetInt(ini, "g.ShinyValueGreen", 10);
	globals.player.g.ShinyValueYellow = xIniGetInt(ini, "g.ShinyValueYellow", 5);
	globals.player.g.ShinyValueRed = xIniGetInt(ini, "g.ShinyValueRed", 1);

	globals.player.g.ShinyValueCombo0 = xIniGetInt(ini, "g.ShinyValueCombo0", 0);
	globals.player.g.ShinyValueCombo1 = xIniGetInt(ini, "g.ShinyValueCombo1", 0);
	globals.player.g.ShinyValueCombo2 = xIniGetInt(ini, "g.ShinyValueCombo2", 2);
	globals.player.g.ShinyValueCombo3 = xIniGetInt(ini, "g.ShinyValueCombo3", 3);
	globals.player.g.ShinyValueCombo4 = xIniGetInt(ini, "g.ShinyValueCombo4", 3);
	globals.player.g.ShinyValueCombo5 = xIniGetInt(ini, "g.ShinyValueCombo5", 5);
	globals.player.g.ShinyValueCombo6 = xIniGetInt(ini, "g.ShinyValueCombo6", 10);
	globals.player.g.ShinyValueCombo7 = xIniGetInt(ini, "g.ShinyValueCombo7", 15);
	globals.player.g.ShinyValueCombo8 = xIniGetInt(ini, "g.ShinyValueCombo8", 20);
	globals.player.g.ShinyValueCombo9 = xIniGetInt(ini, "g.ShinyValueCombo9", 25);
	globals.player.g.ShinyValueCombo10 = xIniGetInt(ini, "g.ShinyValueCombo10", 30);
	globals.player.g.ShinyValueCombo11 = xIniGetInt(ini, "g.ShinyValueCombo11", 40);
	globals.player.g.ShinyValueCombo12 = xIniGetInt(ini, "g.ShinyValueCombo12", 50);
	globals.player.g.ShinyValueCombo13 = xIniGetInt(ini, "g.ShinyValueCombo13", 60);
	globals.player.g.ShinyValueCombo14 = xIniGetInt(ini, "g.ShinyValueCombo14", 75);
	globals.player.g.ShinyValueCombo15 = xIniGetInt(ini, "g.ShinyValueCombo15", 100);

	globals.player.g.ComboTimer = xIniGetFloat(ini, "g.ComboTimer", 1.0f);

	globals.player.g.SundaeTime = xIniGetFloat(ini, "g.SundaeTime", 6.0f);
	globals.player.g.SundaeMult = xIniGetFloat(ini, "g.SundaeMult", 1.5f);

	globals.player.g.BBashTime = xIniGetFloat(ini, "g.BBashTime", 0.3f);
	globals.player.g.BBashDelay = xIniGetFloat(ini, "g.BBashDelay", 0.15f);
	globals.player.g.BBashCVTime = xIniGetFloat(ini, "g.BBashCVTime", 0.15f);
	globals.player.g.BBashHeight = xIniGetFloat(ini, "g.BBashHeight", 3.0f);
	
	globals.player.g.BBounceSpeed = xIniGetFloat(ini, "g.BBounceSpeed", 10.0f);
	
	globals.player.g.BSpinMinFrame = xIniGetFloat(ini, "g.BSpinMinFrame", 2.0f);
	globals.player.g.BSpinMaxFrame = xIniGetFloat(ini, "g.BSpinMaxFrame", 20.0f);
	globals.player.g.BSpinRadius = xIniGetFloat(ini, "g.BSpinRadius", 0.3f);

	globals.player.g.BSpinMinFrame *= 1.0f / 30.0f;
	globals.player.g.BSpinMaxFrame *= 1.0f / 30.0f;

	globals.player.g.SandyMeleeMinFrame = xIniGetFloat(ini, "g.SandyMeleeMinFrame", 1.0f);
	globals.player.g.SandyMeleeMaxFrame = xIniGetFloat(ini, "g.SandyMeleeMaxFrame", 9.0f);
	globals.player.g.SandyMeleeRadius = xIniGetFloat(ini, "g.SandyMeleeRadius", 0.3f);

	globals.player.g.SandyMeleeMinFrame *= 1.0f / 30.0f;
	globals.player.g.SandyMeleeMaxFrame *= 1.0f / 30.0f;

	globals.player.g.DamageTimeHit = xIniGetFloat(ini, "g.DamageTimeHit", 0.5f);
	globals.player.g.DamageTimeSurface = xIniGetFloat(ini, "g.DamageTimeSurface", 1.0f);
	globals.player.g.DamageTimeEGen = xIniGetFloat(ini, "g.DamageTimeEGen", 1.0f);
	globals.player.g.DamageSurfKnock = xIniGetFloat(ini, "g.DamageSurfKnock", 1.75f);
	globals.player.g.DamageGiveHealthKnock = xIniGetFloat(ini, "g.DamageGiveHealthKnock", 1.75f);
	
	globals.player.g.BubbleBowlTimeDelay = xIniGetFloat(ini, "g.BubbleBowlTimeDelay", 1.0f);
	globals.player.g.BubbleBowlLaunchPosLeft = xIniGetFloat(ini, "g.BubbleBowlLaunchPosLeft", 0.0f);
	globals.player.g.BubbleBowlLaunchPosUp = xIniGetFloat(ini, "g.BubbleBowlLaunchPosUp", 1.0f);
	globals.player.g.BubbleBowlLaunchPosAt = xIniGetFloat(ini, "g.BubbleBowlLaunchPosAt", 1.5f);
	globals.player.g.BubbleBowlLaunchVelLeft = xIniGetFloat(ini, "g.BubbleBowlLaunchVelLeft", 0.0f);
	globals.player.g.BubbleBowlLaunchVelUp = xIniGetFloat(ini, "g.BubbleBowlLaunchVelUp", 0.0f);
	globals.player.g.BubbleBowlLaunchVelAt = xIniGetFloat(ini, "g.BubbleBowlLaunchVelAt", 10.0f);
	globals.player.g.BubbleBowlPercentIncrease = xIniGetFloat(ini, "g.BubbleBowlPercentIncrease", 0.85f);
	globals.player.g.BubbleBowlMinSpeed = xIniGetFloat(ini, "g.BubbleBowlMinSpeed", 0.1f);
	globals.player.g.BubbleBowlMinRecoverTime = xIniGetFloat(ini, "g.BubbleBowlMinRecoverTime", 0.15f);

	globals.player.g.SlideAccelVelMin = xIniGetFloat(ini, "g.SlideAccelVelMin", 6.0f);
	globals.player.g.SlideAccelVelMax = xIniGetFloat(ini, "g.SlideAccelVelMax", 9.0f);
	globals.player.g.SlideAccelStart = xIniGetFloat(ini, "g.SlideAccelStart", 9.0f);
	globals.player.g.SlideAccelEnd = xIniGetFloat(ini, "g.SlideAccelEnd", 4.5f);
	globals.player.g.SlideAccelPlayerFwd = xIniGetFloat(ini, "g.SlideAccelPlayerFwd", 5.0f);
	globals.player.g.SlideAccelPlayerBack = xIniGetFloat(ini, "g.SlideAccelPlayerBack", 5.0f);
	globals.player.g.SlideAccelPlayerSide = xIniGetFloat(ini, "g.SlideAccelPlayerSide", 15.0f);
	globals.player.g.SlideVelMaxStart = xIniGetFloat(ini, "g.SlideVelMaxStart", 12.0f);
	globals.player.g.SlideVelMaxEnd = xIniGetFloat(ini, "g.SlideVelMaxEnd", 17.0f);
	globals.player.g.SlideVelMaxIncTime = xIniGetFloat(ini, "g.SlideVelMaxIncTime", 6.0f);
	globals.player.g.SlideVelMaxIncAccel = xIniGetFloat(ini, "g.SlideVelMaxIncAccel", 1.0f);
	globals.player.g.SlideAirHoldTime = xIniGetFloat(ini, "g.SlideAirHoldTime", 1.0f);
	globals.player.g.SlideAirSlowTime = xIniGetFloat(ini, "g.SlideAirSlowTime", 1.5f);
	globals.player.g.SlideAirDblHoldTime = xIniGetFloat(ini, "g.SlideAirDblHoldTime", 1.0f);
	globals.player.g.SlideAirDblSlowTime = xIniGetFloat(ini, "g.SlideAirDblSlowTime", 1.5f);
	globals.player.g.SlideVelDblBoost = xIniGetFloat(ini, "g.SlideVelDblBoost", 6.0f);

	globals.player.g.TakeDamage = xIniGetInt(ini, "g.TakeDamage", 1);

	globals.player.g.CheatSpongeball = xIniGetInt(ini, "g.CheatSpongeball", 0);
	globals.player.g.CheatPlayerSwitch = xIniGetInt(ini, "g.CheatPlayerSwitch", 0);
	globals.player.g.CheatAlwaysPortal = xIniGetInt(ini, "g.CheatAlwaysPortal", 0);
	globals.player.g.CheatFlyToggle = xIniGetInt(ini, "g.CheatFlyToggle", 0);
	globals.player.g.DisableForceConversation = xIniGetInt(ini, "g.DisableForceConversation", 0);

#ifndef DEBUG
	globals.player.g.CheatFlyToggle = 0;
	globals.player.g.CheatAlwaysPortal = 0;
#endif

	globals.player.g.StartSlideAngle = xIniGetFloat(ini, "g.StartSlideAngle", 30.0f);
	globals.player.g.StopSlideAngle = xIniGetFloat(ini, "g.StopSlideAngle", 10.0f);
	globals.player.g.RotMatchMaxAngle = xIniGetFloat(ini, "g.RotMatchMaxAngle", 30.0f);
	globals.player.g.RotMatchMatchTime = xIniGetFloat(ini, "g.RotMatchMatchTime", 0.1f);
	globals.player.g.RotMatchRelaxTime = xIniGetFloat(ini, "g.RotMatchRelaxTime", 0.3f);
	globals.player.g.Gravity = xIniGetFloat(ini, "g.Gravity", 30.0f);

	globals.player.g.StartSlideAngle = DEG2RAD(globals.player.g.StartSlideAngle);
	globals.player.g.StopSlideAngle = DEG2RAD(globals.player.g.StopSlideAngle);
	globals.player.g.RotMatchMaxAngle = DEG2RAD(globals.player.g.RotMatchMaxAngle);

	uint32 use_degrees = (xIniGetFloat(ini, "zcam_highbounce_pitch", 180.0f) == 180.0f); // todo confirm with decomp

	xcam_collis_radius = xIniGetFloat(ini, "xcam_collis_radius", xcam_collis_radius);
	xcam_collis_stiffness = xIniGetFloat(ini, "xcam_collis_stiffness", xcam_collis_stiffness);
	
	zcam_pad_pyaw_scale = xIniGetFloat(ini, "zcam_pad_pyaw_scale", zcam_pad_pyaw_scale);
	zcam_pad_pitch_scale = xIniGetFloat(ini, "zcam_pad_pitch_scale", zcam_pad_pitch_scale);
	zcam_near_d = xIniGetFloat(ini, "zcam_near_d", zcam_near_d);
	zcam_near_h = xIniGetFloat(ini, "zcam_near_h", zcam_near_h);
	zcam_near_pitch = xIniGetFloat(ini, "zcam_near_pitch", use_degrees ? RAD2DEG(zcam_near_pitch) : zcam_near_pitch);
	zcam_far_d = xIniGetFloat(ini, "zcam_far_d", zcam_far_d);
	zcam_far_h = xIniGetFloat(ini, "zcam_far_h", zcam_far_h);
	zcam_far_pitch = xIniGetFloat(ini, "zcam_far_pitch", use_degrees ? RAD2DEG(zcam_far_pitch) : zcam_far_pitch);
	zcam_above_d = xIniGetFloat(ini, "zcam_above_d", zcam_above_d);
	zcam_above_h = xIniGetFloat(ini, "zcam_above_h", zcam_above_h);
	zcam_above_pitch = xIniGetFloat(ini, "zcam_above_pitch", use_degrees ? RAD2DEG(zcam_above_pitch) : zcam_above_pitch);
	zcam_below_d = xIniGetFloat(ini, "zcam_below_d", zcam_below_d);
	zcam_below_h = xIniGetFloat(ini, "zcam_below_h", zcam_below_h);
	zcam_below_pitch = xIniGetFloat(ini, "zcam_below_pitch", use_degrees ? RAD2DEG(zcam_below_pitch) : zcam_below_pitch);
	zcam_highbounce_d = xIniGetFloat(ini, "zcam_highbounce_d", zcam_highbounce_d);
	zcam_highbounce_h = xIniGetFloat(ini, "zcam_highbounce_h", zcam_highbounce_h);
	zcam_highbounce_pitch = xIniGetFloat(ini, "zcam_highbounce_pitch", use_degrees ? RAD2DEG(zcam_highbounce_pitch) : zcam_highbounce_pitch);
	zcam_wall_d = xIniGetFloat(ini, "zcam_wall_d", zcam_wall_d);
	zcam_wall_h = xIniGetFloat(ini, "zcam_wall_h", zcam_wall_h);
	zcam_wall_pitch = xIniGetFloat(ini, "zcam_wall_pitch", use_degrees ? RAD2DEG(zcam_wall_pitch) : zcam_wall_pitch);
	zcam_overrot_min = xIniGetFloat(ini, "zcam_overrot_min", RAD2DEG(zcam_overrot_min));
	zcam_overrot_mid = xIniGetFloat(ini, "zcam_overrot_mid", RAD2DEG(zcam_overrot_mid));
	zcam_overrot_max = xIniGetFloat(ini, "zcam_overrot_max", RAD2DEG(zcam_overrot_max));
	zcam_overrot_rate = xIniGetFloat(ini, "zcam_overrot_rate", zcam_overrot_rate);
	zcam_overrot_tstart = xIniGetFloat(ini, "zcam_overrot_tstart", zcam_overrot_tstart);
	zcam_overrot_tend = xIniGetFloat(ini, "zcam_overrot_tend", zcam_overrot_tend);
	zcam_overrot_velmin = xIniGetFloat(ini, "zcam_overrot_velmin", zcam_overrot_velmin);
	zcam_overrot_velmax = xIniGetFloat(ini, "zcam_overrot_velmax", zcam_overrot_velmax);
	zcam_overrot_tmanual = xIniGetFloat(ini, "zcam_overrot_tmanual", zcam_overrot_tmanual);

	if (use_degrees)
	{
		zcam_near_pitch = DEG2RAD(zcam_near_pitch);
		zcam_far_pitch = DEG2RAD(zcam_far_pitch);
		zcam_above_pitch = DEG2RAD(zcam_above_pitch);
		zcam_below_pitch = DEG2RAD(zcam_below_pitch);
		zcam_highbounce_pitch = DEG2RAD(zcam_highbounce_pitch);
		zcam_wall_pitch = DEG2RAD(zcam_wall_pitch);
	}

	zcam_overrot_min = DEG2RAD(zcam_overrot_min);
	zcam_overrot_mid = DEG2RAD(zcam_overrot_mid);
	zcam_overrot_max = DEG2RAD(zcam_overrot_max);

	gSkipTimeCutscene = xIniGetFloat(ini, "gSkipTimeCutscene", gSkipTimeCutscene);
	gSkipTimeFlythrough = xIniGetFloat(ini, "gSkipTimeFlythrough", gSkipTimeFlythrough);

	gSkipTimeCutscene = MAX(gSkipTimeCutscene, 1.0f);
	gSkipTimeFlythrough = MAX(gSkipTimeFlythrough, 0.0f);

	globals.player.carry.minDist = xIniGetFloat(ini, "carry.minDist", 0.675f);
	globals.player.carry.maxDist = xIniGetFloat(ini, "carry.maxDist", 1.9f);
	globals.player.carry.minHeight = xIniGetFloat(ini, "carry.minHeight", -0.2f);
	globals.player.carry.maxHeight = xIniGetFloat(ini, "carry.maxHeight", 0.4f);
	globals.player.carry.maxCosAngle = xIniGetFloat(ini, "carry.maxCosAngle", 45.0f);
	globals.player.carry.throwMinDist = xIniGetFloat(ini, "carry.throwMinDist", 1.5f);
	globals.player.carry.throwMaxDist = xIniGetFloat(ini, "carry.throwMaxDist", 12.0f);
	globals.player.carry.throwMinHeight = xIniGetFloat(ini, "carry.throwMinHeight", -3.0f);
	globals.player.carry.throwMaxHeight = xIniGetFloat(ini, "carry.throwMaxHeight", 5.0f);
	globals.player.carry.throwMaxStack = xIniGetFloat(ini, "carry.throwMaxStack", 2.75f);
	globals.player.carry.throwMaxCosAngle = xIniGetFloat(ini, "carry.throwMaxCosAngle", 25.0f);
	globals.player.carry.grabLerpMin = xIniGetFloat(ini, "carry.grabLerpMin", 0.0f);
	globals.player.carry.grabLerpMax = xIniGetFloat(ini, "carry.grabLerpMax", 0.2f);
	globals.player.carry.throwGravity = xIniGetFloat(ini, "carry.throwGravity", 30.0f);
	globals.player.carry.throwHeight = xIniGetFloat(ini, "carry.throwHeight", 3.75f);
	globals.player.carry.throwDistance = xIniGetFloat(ini, "carry.throwDistance", 10.0f);
	globals.player.carry.fruitFloorDecayMin = xIniGetFloat(ini, "carry.fruitFloorDecayMin", 0.3f);
	globals.player.carry.fruitFloorDecayMax = xIniGetFloat(ini, "carry.fruitFloorDecayMax", 6.0f);
	globals.player.carry.fruitFloorBounce = xIniGetFloat(ini, "carry.fruitFloorBounce", 0.15f);
	globals.player.carry.fruitFloorFriction = xIniGetFloat(ini, "carry.fruitFloorFriction", 0.4f);
	globals.player.carry.fruitCeilingBounce = xIniGetFloat(ini, "carry.fruitCeilingBounce", 0.1f);
	globals.player.carry.fruitWallBounce = xIniGetFloat(ini, "carry.fruitWallBounce", 0.5f);
	globals.player.carry.fruitLifetime = xIniGetFloat(ini, "carry.fruitLifetime", 15.0f);

	globals.player.carry.maxCosAngle = icos(DEG2RAD(globals.player.carry.maxCosAngle));
	globals.player.carry.throwMaxCosAngle = icos(DEG2RAD(globals.player.carry.throwMaxCosAngle));

	globals.player.g.PowerUp[POWERUP_BUBBLEBOWL] = xIniGetInt(ini, "g.BubbleBowl", 0);
	globals.player.g.PowerUp[POWERUP_CRUISEBUBBLE] = xIniGetInt(ini, "g.CruiseBubble", 0);

	memcpy(globals.player.g.InitialPowerUp, globals.player.g.PowerUp, POWERUP_COUNT);

	xIniGetFloatList(globals.player.sb.MoveSpeed, ini, "sb.MoveSpeed", (0.6f, 3.0f, 7.0f, 0.1f, 0.5f, 1.0f));
	xIniGetFloatList(globals.player.sb.AnimSneak, ini, "sb.AnimSneak", (1.5f, 0.5f, 1.5f));
	xIniGetFloatList(globals.player.sb.AnimWalk, ini, "sb.AnimWalk", (3.0f, 0.5f, 2.5f));
	xIniGetFloatList(globals.player.sb.AnimRun, ini, "sb.AnimRun", (3.0f, 0.5f, 2.5f));

	globals.player.sb.JumpGravity = xIniGetFloat(ini, "sb.JumpGravity", 7.0f);
	globals.player.sb.GravSmooth = xIniGetFloat(ini, "sb.GravSmooth", 0.25f);
	globals.player.sb.FloatSpeed = xIniGetFloat(ini, "sb.FloatSpeed", 1.0f);
	globals.player.sb.ButtsmashSpeed = xIniGetFloat(ini, "sb.ButtsmashSpeed", 5.0f);

	globals.player.sb.ledge.animGrab = xIniGetFloat(ini, "sb.ledge.animGrab", 3.0f);
	
	zLedgeAdjust(&globals.player.sb.ledge);

	bungee_state::load_settings(*ini);
	oob_state::load_settings(*ini);
	ztalkbox::load_settings(*ini);

	xIniGetFloatList((float32*)&globals.player.sb.Jump, ini, "sb.Jump", (2.0f, 0.7f, 0.35f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sb.Double, ini, "sb.Double", (1.0f, 0.7f, 0.35f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sb.Bounce, ini, "sb.Bounce", (1.5f, 0.2f, 0.2f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sb.Spring, ini, "sb.Spring", (3.0f, 0.2f, 0.2f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sb.Wall, ini, "sb.Wall", (0.0f, 0.0f, 0.0f, 0.0f));

	globals.player.sb.WallJumpVelocity = xIniGetFloat(ini, "sb.WallJumpVelocity", 0.0f);

	globals.player.sb.spin_damp_xz = xIniGetFloat(ini, "sb.spin_damp_xz", 15.0f);
	globals.player.sb.spin_damp_y = xIniGetFloat(ini, "sb.spin_damp_y", 15.0f);

	globals.player.s = &globals.player.sb;

	CalcJumpImpulse(&globals.player.sb.Jump, NULL);
	CalcJumpImpulse(&globals.player.sb.Double, NULL);
	CalcJumpImpulse(&globals.player.sb.Bounce, NULL);
	CalcJumpImpulse(&globals.player.sb.Spring, NULL);
	CalcJumpImpulse(&globals.player.sb.Wall, NULL);

	globals.player.sb_model_indices[SB_MODEL_BODY] = xIniGetInt(ini, "SB.model_index.body", 0);
	globals.player.sb_model_indices[SB_MODEL_ARM_L] = xIniGetInt(ini, "SB.model_index.arm_l", 1);
	globals.player.sb_model_indices[SB_MODEL_ARM_R] = xIniGetInt(ini, "SB.model_index.arm_r", 2);
	globals.player.sb_model_indices[SB_MODEL_ASS] = xIniGetInt(ini, "SB.model_index.ass", 3);
	globals.player.sb_model_indices[SB_MODEL_UNDERWEAR] = xIniGetInt(ini, "SB.model_index.underwear", 4);
	globals.player.sb_model_indices[SB_MODEL_WAND] = xIniGetInt(ini, "SB.model_index.wand", 5);
	globals.player.sb_model_indices[SB_MODEL_TONGUE] = xIniGetInt(ini, "SB.model_index.tongue", 6);
	globals.player.sb_model_indices[SB_MODEL_BUBBLE_HELMET] = xIniGetInt(ini, "SB.model_index.bubble_helmet", 7);
	globals.player.sb_model_indices[SB_MODEL_BUBBLE_SHOE_L] = xIniGetInt(ini, "SB.model_index.bubble_shoe_l", 8);
	globals.player.sb_model_indices[SB_MODEL_BUBBLE_SHOE_R] = xIniGetInt(ini, "SB.model_index.bubble_shoe_r", 9);
	globals.player.sb_model_indices[SB_MODEL_SHADOW_WAND] = xIniGetInt(ini, "SB.model_index.shadow_wand", 10);
	globals.player.sb_model_indices[SB_MODEL_SHADOW_ARM_L] = xIniGetInt(ini, "SB.model_index.shadow_arm_l", 11);
	globals.player.sb_model_indices[SB_MODEL_SHADOW_ARM_R] = xIniGetInt(ini, "SB.model_index.shadow_arm_r", 12);
	globals.player.sb_model_indices[SB_MODEL_SHADOW_BODY] = xIniGetInt(ini, "SB.model_index.shadow_body", 13);

	xIniGetFloatList(globals.player.patrick.MoveSpeed, ini, "patrick.MoveSpeed", (0.6f, 3.0f, 7.0f, 0.1f, 0.5f, 1.0f));
	xIniGetFloatList(globals.player.patrick.AnimSneak, ini, "patrick.AnimSneak", (1.5f, 0.5f, 1.5f));
	xIniGetFloatList(globals.player.patrick.AnimWalk, ini, "patrick.AnimWalk", (3.0f, 0.5f, 2.5f));
	xIniGetFloatList(globals.player.patrick.AnimRun, ini, "patrick.AnimRun", (3.0f, 0.5f, 2.5f));

	globals.player.patrick.JumpGravity = xIniGetFloat(ini, "patrick.JumpGravity", 7.0f);
	globals.player.patrick.GravSmooth = xIniGetFloat(ini, "patrick.GravSmooth", 0.25f);
	globals.player.patrick.FloatSpeed = xIniGetFloat(ini, "patrick.FloatSpeed", 1.0f);
	globals.player.patrick.ButtsmashSpeed = xIniGetFloat(ini, "patrick.ButtsmashSpeed", 5.0f);

	globals.player.patrick.ledge.animGrab = xIniGetFloat(ini, "patrick.ledge.animGrab", 3.0f);

	zLedgeAdjust(&globals.player.patrick.ledge);

	xIniGetFloatList((float32*)&globals.player.patrick.Jump, ini, "patrick.Jump", (2.0f, 0.7f, 0.35f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.patrick.Double, ini, "patrick.Double", (1.0f, 0.7f, 0.35f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.patrick.Bounce, ini, "patrick.Bounce", (1.5f, 0.2f, 0.2f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.patrick.Spring, ini, "patrick.Spring", (3.0f, 0.2f, 0.2f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.patrick.Wall, ini, "patrick.Wall", (0.0f, 0.0f, 0.0f, 0.0f));

	globals.player.s = &globals.player.sb;

	CalcJumpImpulse(&globals.player.patrick.Jump, NULL);
	CalcJumpImpulse(&globals.player.patrick.Double, NULL);
	CalcJumpImpulse(&globals.player.patrick.Bounce, NULL);
	CalcJumpImpulse(&globals.player.patrick.Spring, NULL);
	CalcJumpImpulse(&globals.player.patrick.Wall, NULL);

	globals.player.patrick.WallJumpVelocity = xIniGetFloat(ini, "patrick.WallJumpVelocity", 0.0f);

	xIniGetFloatList(globals.player.sandy.MoveSpeed, ini, "sandy.MoveSpeed", (0.6f, 3.0f, 7.0f, 0.1f, 0.5f, 1.0f));
	xIniGetFloatList(globals.player.sandy.AnimSneak, ini, "sandy.AnimSneak", (1.5f, 0.5f, 1.5f));
	xIniGetFloatList(globals.player.sandy.AnimWalk, ini, "sandy.AnimWalk", (3.0f, 0.5f, 2.5f));
	xIniGetFloatList(globals.player.sandy.AnimRun, ini, "sandy.AnimRun", (3.0f, 0.5f, 2.5f));

	globals.player.sandy.JumpGravity = xIniGetFloat(ini, "sandy.JumpGravity", 7.0f);
	globals.player.sandy.GravSmooth = xIniGetFloat(ini, "sandy.GravSmooth", 0.25f);
	globals.player.sandy.FloatSpeed = xIniGetFloat(ini, "sandy.FloatSpeed", 1.0f);
	globals.player.sandy.ButtsmashSpeed = xIniGetFloat(ini, "sandy.ButtsmashSpeed", 5.0f);

	globals.player.sandy.ledge.animGrab = xIniGetFloat(ini, "sandy.ledge.animGrab", 3.0f);

	zLedgeAdjust(&globals.player.sandy.ledge);

	xIniGetFloatList((float32*)&globals.player.sandy.Jump, ini, "sandy.Jump", (2.0f, 0.7f, 0.35f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sandy.Double, ini, "sandy.Double", (1.0f, 0.7f, 0.35f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sandy.Bounce, ini, "sandy.Bounce", (1.5f, 0.2f, 0.2f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sandy.Spring, ini, "sandy.Spring", (3.0f, 0.2f, 0.2f, 0.0f));
	xIniGetFloatList((float32*)&globals.player.sandy.Wall, ini, "sandy.Wall", (0.0f, 0.0f, 0.0f, 0.0f));

	globals.player.s = &globals.player.sb;

	CalcJumpImpulse(&globals.player.sandy.Jump, NULL);
	CalcJumpImpulse(&globals.player.sandy.Double, NULL);
	CalcJumpImpulse(&globals.player.sandy.Bounce, NULL);
	CalcJumpImpulse(&globals.player.sandy.Spring, NULL);
	CalcJumpImpulse(&globals.player.sandy.Wall, NULL);

	globals.player.sandy.WallJumpVelocity = xIniGetFloat(ini, "sandy.WallJumpVelocity", 0.0f);

	sShowMenuOnBoot = xIniGetInt(ini, "ShowMenuOnBoot", TRUE);
	gGameSfxReport = xIniGetInt(ini, "SFXReport", 0);

	globals.player.s = &globals.player.sb;

	globals.player.sb.pcType = ePlayer_SB;
	globals.player.patrick.pcType = ePlayer_Patrick;
	globals.player.sandy.pcType = ePlayer_Sandy;
}

static void zMainMemLvlChkCB()
{
	zSceneMemLvlChkCB();
}

STUB void zMainShowProgressBar()
{

}

NONMATCH static void zMainLoop()
{
	iTime t;

	zMainShowProgressBar();
	
	xMemPushBase();

	LOAD_SCENE('BOOT', XST_HIP, true);

	zMainShowProgressBar();

	LOAD_SCENE('PLAT', XST_HIP, false);

	zMainShowProgressBar();

	iTimeGet();
	xShadowSimple_Init();

	globals.pickupTable = (zAssetPickupTable*)xSTFindAssetByType('PICK', 0);

	zPickupTableInit();

	xMemPushBase();

	LOAD_SCENE('MNU4', XST_HIP, true);

	zMainShowProgressBar();

	xMemPushBase();

	LOAD_SCENE('MNU5', XST_HIP, true);

	zMainShowProgressBar();

	xModelInit();
	xModelPoolInit(32, 64);
	xModelPoolInit(40, 8);
	xModelPoolInit(56, 1);

	static uint32 preinit_bubble_matfx[] =
	{
		0xDBD033BC,
		0x452279A2,
		0xC17F4BCC,
		0x0CF9267A,
		0x5C009D14,
		0
	};

	uint32* preinit = preinit_bubble_matfx;

	while (*preinit != 0)
	{
		RpAtomic* modl = (RpAtomic*)xSTFindAsset(*preinit);

		if (modl)
		{
			xFXPreAllocMatFX(RpAtomicGetClump(modl));
		}

		preinit++;
	}

#if 0
	static uint32 menuModeID = 'MNU3';
#else
	static uint32 menuModeID = 'TL01';
#endif
	static uint32 gameSceneID =
		globals.sceneStart[0] << 24 |
		globals.sceneStart[1] << 16 |
		globals.sceneStart[2] << 8 |
		globals.sceneStart[3];

	uint32 newGameSceneID = gameSceneID;

	XPRINTF("gameSceneID = %s\n", xUtil_idtag2string(gameSceneID));

	xMemPushBase();

	zMainShowProgressBar();

	zMainMemCardSpaceQuery();

	while (!iSystemShouldQuit())
	{
		zMainShowProgressBar();

		xSerialWipeMainBuffer();
		zCutSceneNamesTable_clearAll();

		zMainShowProgressBar();

		if (sShowMenuOnBoot)
		{
			zMenuInit(menuModeID);

			zMainShowProgressBar();
			zMainShowProgressBar();

			zMenuSetup();

			xFX_SceneEnter(globals.sceneCur->env->geom->world);

			newGameSceneID = zMenuLoop();

			zMenuExit();
		}
		else
		{
			sShowMenuOnBoot = TRUE;
			globals.firstStartPressed = true;

			zGameModeSwitch(eGameMode_Game);
			zGameStateSwitch(eGameState_FirstTime);
		}

		if (newGameSceneID == 0)
		{
			zVarNewGame();
			iTimeSetGame(0.0f);

			gameSceneID =
				globals.sceneStart[0] << 24 |
				globals.sceneStart[1] << 16 |
				globals.sceneStart[2] << 8 |
				globals.sceneStart[3];
		}
		else
		{
			gameSceneID = newGameSceneID;

			iTimeSetGame(0.0f);
		}

		zGameInit(gameSceneID);
		zGameSetup();

		iProfileClear(gameSceneID);

		zGameLoop();
		zGameExit();
	}
}

NONMATCH static void zMainReadINI()
{
	char* str;
	void* buf;
	uint32 size;
	xIniFile* ini;

	buf = iFileLoad("SB.INI", NULL, &size);

	if (buf)
	{
		ini = xIniParse((char*)buf, size);

		str = xIniGetString(ini, "PATH");

		if (str)
		{
			iFileSetPath(str);
		}

		str = xIniGetString(ini, "BOOT");

		if (str)
		{
			strcpy(globals.sceneStart, "BOOT");

			if (strlen(str) == 4)
			{
				strcpy(globals.sceneStart, str);

				if (xStricmp(str, "soak") == 0)
				{
					gSoak = 1;
					strcpy(globals.sceneStart, "mnu3");
				}
			}
		}

		globals.profile = xIniGetInt(ini, "Profile");

		str = xIniGetString(ini, "ProfFuncTriangle", "");
		strncpy(globals.profFunc[PROF_FUNC_TRIANGLE], str, PROF_FUNC_LEN);
		globals.profFunc[PROF_FUNC_TRIANGLE][PROF_FUNC_LEN - 1] = '\0';

		str = xIniGetString(ini, "ProfFuncSquare", "");
		strncpy(globals.profFunc[PROF_FUNC_SQUARE], str, PROF_FUNC_LEN);
		globals.profFunc[PROF_FUNC_SQUARE][PROF_FUNC_LEN - 1] = '\0';

		str = xIniGetString(ini, "ProfFuncLeft", "");
		strncpy(globals.profFunc[PROF_FUNC_LEFT], str, PROF_FUNC_LEN);
		globals.profFunc[PROF_FUNC_LEFT][PROF_FUNC_LEN - 1] = '\0';

		str = xIniGetString(ini, "ProfFuncRight", "");
		strncpy(globals.profFunc[PROF_FUNC_RIGHT], str, PROF_FUNC_LEN);
		globals.profFunc[PROF_FUNC_RIGHT][PROF_FUNC_LEN - 1] = '\0';

		str = xIniGetString(ini, "ProfFuncUp", "");
		strncpy(globals.profFunc[PROF_FUNC_UP], str, PROF_FUNC_LEN);
		globals.profFunc[PROF_FUNC_UP][PROF_FUNC_LEN - 1] = '\0';

		str = xIniGetString(ini, "ProfFuncDown", "");
		strncpy(globals.profFunc[PROF_FUNC_DOWN], str, PROF_FUNC_LEN);
		globals.profFunc[PROF_FUNC_DOWN][PROF_FUNC_LEN - 1] = '\0';

		globals.useHIPHOP = xIniGetInt(ini, "EnableHipHopLoading");
		globals.NoMusic = xIniGetInt(ini, "NoMusic") != 0;

		zMainParseINIGlobals(ini);
		zUI_ParseINI(ini);

		xIniDestroy(ini);
	}

	XPRINTF("zMainReadINI finished at time %f (tick %d)\n", iTimeDiffSec(iTimeGet()), iTimeGet());

	RwFree(buf);
}

STUB void zMainFirstScreen(int32 mode)
{

}

STUB void zMainMemCardSpaceQuery()
{

}

static void zMainLoadFontHIP()
{
	xMemPushBase();

	LOAD_SCENE('FONT', XST_HIP, true);
}