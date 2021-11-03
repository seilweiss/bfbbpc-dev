#include "zMenu.h"

#include "iSystem.h"
#include "zMain.h"
#include "zCamera.h"
#include "zMusic.h"
#include "xSnd.h"
#include "zGame.h"
#include "zEnv.h"
#include "xSkyDome.h"
#include "zEnt.h"
#include "xEvent.h"
#include "iColor.h"
#include "zGameState.h"
#include "zGameExtras.h"
#include "xScrFx.h"
#include "zFMV.h"
#include "xString.h"
#include "xDraw.h"
#include "iTRC.h"
#include "xDebug.h"
#include "xTRC.h"
#include "zSaveLoad.h"

#define ATTRACTMODE_TIME 48.264f
#define HOLD_TIME 10.0f

bool menu_fmv_played = false;

static bool32 sFirstBoot = TRUE;
static float32 time_elapsed = 0.01f;
static float32 time_last = 0.0f;
static float32 time_current = 0.0f;
static float32 sAttractMode_timer = 0.0f;
static int32 sInMenu = FALSE;
static float32 holdTmr = HOLD_TIME;

static bool32 zMenuIsPaused();
static bool32 zMenuLoopContinue();
static uint32 zMenuUpdateMode();

void zMenuInit(uint32 theSceneID)
{
	sInMenu = TRUE;

	xsrand(iTimeGet());
	iTimeGet();
	xrand();

	zSceneInit(theSceneID, FALSE);

	xCameraInit(&globals.camera, SCREEN_WIDTH, SCREEN_HEIGHT);
	zCameraReset(&globals.camera);
	xCameraSetScene(&globals.camera, globals.sceneCur);

	zMusicInit();
}

void zMenuExit()
{
	zMusicKill();
	xSndStopAll(~SND_CAT_UI);
	xCameraExit(&globals.camera);
	zSceneExit(FALSE);

	sInMenu = FALSE;
}

void zMenuSetup()
{
	globals.player.MaxHealth = 3;

	zSceneSetup();

	sAttractMode_timer = ATTRACTMODE_TIME;

	zGameSetupPlayer();
	zEnvStartingCamera(gCurEnv);
	xCameraUpdate(&globals.camera, -1.0f);
	xSkyDome_Setup();

	zEntEventAll(NULL, 0, eEventSceneBegin, NULL);
	zEntEventAll(NULL, 0, eEventRoomBegin, NULL);

	if (globals.updateMgr)
	{
		xUpdateCull_Update(globals.updateMgr, 100);
	}

	zEntEvent(&globals.player.ent, eEventControlOff);
}

UNCHECKED uint32 zMenuLoop()
{
	uint32 s = 0;

	if (sFirstBoot)
	{
		zGameModeSwitch(eGameMode_Intro);
		zGameStateSwitch(eIntroState_Sony);
		zMusicNotify(ZMUSIC_SITUATION_0);
	}
	else
	{
		zGameModeSwitch(eGameMode_Title);
		zGameStateSwitch(eTitleState_Start);
		zMusicNotify(ZMUSIC_SITUATION_0);
	}

	time_last = iTimeGetSecs() - ONE_FRAME;
	
	zGameExtras_NewGameReset();

	iColor black = { 0, 0, 0, 255 };
	iColor clear = { 0, 0, 0, 0 };

	xScrFxFade(&black, &clear, 0.0f, NULL, TRUE);

	int32 ostrich_delay = 10;

	time_last = iTimeGetSecs() - ONE_FRAME;

	do
	{
		bool32 draw_black = FALSE;
		// int32 ss; // unused

		if (zGameModeGet() == eGameMode_Intro)
		{
			switch (zGameStateGet())
			{
			case eIntroState_Sony:
			{
				draw_black = TRUE;

				zGameStateSwitch(eIntroState_License);
				xScrFxFade(&clear, &black, 0.0f, NULL, TRUE);

				break;
			}
			case eIntroState_License:
			{
				draw_black = TRUE;

				zFMVPlay(zFMVFileGetName(eFMVFile_LogoNick), 1, 2.0f, true, false);
				zFMVPlay(zFMVFileGetName(eFMVFile_LogoTHQ), 1, 2.0f, true, false);
				zFMVPlay(zFMVFileGetName(eFMVFile_LogoRW), 1, 2.0f, true, false);

				holdTmr = HOLD_TIME;

				zGameModeSwitch(eGameMode_Title);
				zGameStateSwitch(eTitleState_Attract);

				xScrFxFade(&black, &clear, 0.1f, NULL, TRUE);

				time_last = iTimeGetSecs() - ONE_FRAME;

				break;
			}
			}

			time_current = iTimeGetSecs();
			time_elapsed = time_current - time_last;

			if (menu_fmv_played && time_elapsed > ONE_FRAME)
			{
				time_last = time_current - ONE_FRAME;
				time_elapsed = ONE_FRAME;
			}

			menu_fmv_played = false;

			if (time_elapsed < 0.0f)
			{
				time_elapsed = ONE_FRAME;
			}

			if (zGameModeGet() == eGameMode_Title)
			{
				sAttractMode_timer -= time_elapsed;

				if (sAttractMode_timer < 0.0f)
				{
					xEnt* title1 = (xEnt*)zSceneFindObject(xStrHash("MNU3 PRESS START 02 UIF"));
					xEnt* title2 = (xEnt*)zSceneFindObject(xStrHash("MNU3 START NEWGAME UIF"));

					if ((title1 && xEntIsVisible(title1)) ||
						(title2 && xEntIsVisible(title2)))
					{
						zGameStateSwitch(eTitleState_Attract);
					}
					else
					{
						sAttractMode_timer = ATTRACTMODE_TIME;
					}
				}

				if (zGameStateGet() == eTitleState_Attract)
				{
					if (sFirstBoot)
					{
						zMenuFirstBootSet(FALSE);
					}
					else
					{
						zMenuFMVPlay(zFMVFileGetName(eFMVFile_Demo1), 0xFFFF, 0.1f, true, globals.firstStartPressed);
					}

					zGameStateSwitch(eTitleState_Start);

					sAttractMode_timer = ATTRACTMODE_TIME;
				}
				else
				{
					zMusicUpdate(time_elapsed);
				}

				if (mPad[globals.currentActivePad].pressed != 0)
				{
					sAttractMode_timer = ATTRACTMODE_TIME;
				}
			}

			time_last = time_current;

			xPadUpdate(globals.currentActivePad, time_elapsed);

			if (!globals.firstStartPressed)
			{
				xPadUpdate(1, time_elapsed);
				xPadUpdate(2, time_elapsed);
				xPadUpdate(3, time_elapsed);
			}

			xDrawBegin();

			bool32 paused = zMenuIsPaused();

			zSceneUpdate(time_elapsed);

			if (!paused)
			{
				xEnt_Update(&globals.player.ent, globals.sceneCur, time_elapsed);
			}

			xSndUpdate();
			iTRCDisk::CheckDVDAndResetState();

			zCameraUpdate(&globals.camera, time_elapsed);
			xCameraBegin(&globals.camera, TRUE);

			zSceneRender();
			xDebugUpdate();

			if (draw_black)
			{
				RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDZERO);
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);

				xScrFxDrawScreenSizeRectangle();
			}

			xDrawEnd();

			xCameraEnd(&globals.camera, time_elapsed, TRUE);
			xCameraShowRaster(&globals.camera);

			s = zMenuUpdateMode();

			if (globals.sceneCur->pendingPortal)
			{
				char* src = (char*)&globals.sceneCur->pendingPortal->passet->sceneID;
				char* tgt = (char*)&s;

				tgt[0] = src[3];
				tgt[1] = src[2];
				tgt[2] = src[1];
				tgt[3] = src[0];

				if (src[0] < '0' || src[0] > '9')
				{
					memcpy(tgt, src, 4);
				}

				zGameModeSwitch(eGameMode_Game);
				zGameStateSwitch(eGameState_FirstTime);
			}

			gFrameCount++;

			if (ostrich_delay > 0)
			{
				ostrich_delay--;
			}
			else
			{
				zGameSetOstrich(eGameOstrich_InScene);

				if (gTrcPad[0].state != TRC_PadInserted)
				{
					globals.dontShowPadMessageDuringLoadingOrCutScene = true;

					xTRCPad(gTrcPad[0].id, TRC_PadMissing);
				}
			}
		}
	} while (zMenuLoopContinue());

	zGameExtras_NewGameReset();

	return s;
}

static bool32 zMenuIsPaused()
{
	return FALSE;
}

UNCHECKED static bool32 zMenuLoopContinue()
{
	iSystemUpdate();

	return !iSystemShouldQuit() && gGameMode != eGameMode_Game;
}

UNCHECKED static uint32 zMenuUpdateMode()
{
	uint32 retVal = 0;

	if (gGameMode == eGameMode_Load)
	{
		float32 elapsed1 = (float32)iTimeGet() - time_last; // BUG: should be iTimeGetSecs()

		retVal = zSaveLoad_LoadLoop();

		if (retVal == '0000')
		{
			retVal = 0;
		}
		else if (retVal == 'MNU3')
		{
			retVal = 'HB00';
		}

		time_last = elapsed1 + (float32)iTimeGet(); // BUG: should be iTimeGetSecs()
	}
	else if (gGameMode == eGameMode_Save)
	{
		zSaveLoad_LoadLoop();

		globals.autoSaveFeature = true;

		retVal = 'HB00';
	}

	return retVal;
}

UNCHECKED void zMenuFMVPlay(char* filename, uint32 buttons, float32 time, bool skippable, bool lockController)
{
	if (filename)
	{
		zFMVPlay(filename, buttons, time, skippable, lockController);

		time_last = (float32)iTimeGet(); // BUG: should be iTimeGetSecs()
		time_current = time_last;
		sAttractMode_timer = ATTRACTMODE_TIME;
	}
}

bool32 zMenuIsFirstBoot()
{
	return sFirstBoot;
}

void zMenuFirstBootSet(bool32 firstBoot)
{
	sFirstBoot = firstBoot;
}