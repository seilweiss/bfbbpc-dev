#ifndef ZGAMESTATE_H
#define ZGAMESTATE_H

#include <types.h>

typedef enum _GameOstrich
{
	eGameOstrich_Loading,
	eGameOstrich_PlayingMovie,
	eGameOstrich_InScene,
	eGameOstrich_Total
} GameOstrich;

enum eGameMode
{
	eGameMode_Boot,
	eGameMode_Intro,
	eGameMode_Title,
	eGameMode_Start,
	eGameMode_Load,
	eGameMode_Options,
	eGameMode_Save,
	eGameMode_Pause,
	eGameMode_Stall,
	eGameMode_WorldMap,
	eGameMode_MonsterGallery,
	eGameMode_ConceptArtGallery,
	eGameMode_Game,
	eGameMode_Count
};

// These enums are fake, but they're likely similar to how they were in the original code
// Enums are way easier to use than a bunch of macros in this case

enum eGameState
{
    eGameState_FirstTime,
    eGameState_Play,
    eGameState_LoseChance,
    eGameState_GameOver,
    eGameState_GameStats,
    eGameState_SceneSwitch,
    eGameState_Dead,
    eGameState_Exit,
    eGameState_Count
};

enum ePauseState
{
    ePauseState_Pause,
    ePauseState_Options,
    ePauseState_Count
};

enum eSaveState
{
    eSaveState_SelectMemCard,
    eSaveState_SelectSlot,
    eSaveState_Saving,
    eSaveState_Count
};

enum eOptionsState
{
    eOptionsState_Options,
    eOptionsState_Count
};

enum eLoadState
{
    eLoadState_SelectMemCard,
    eLoadState_SelectSlot,
    eLoadState_Loading,
    eLoadState_Count
};

enum eTitleState
{
    eTitleState_Start,
    eTitleState_Attract,
    eTitleState_Count
};

enum eIntroState
{
    eIntroState_Sony,
    eIntroState_Publisher,
    eIntroState_Developer,
    eIntroState_License,
    eIntroState_Count
};

extern int32 gGameState;
extern eGameMode gGameMode;
extern GameOstrich gGameOstrich;

int32 zGameStateGet();
int32 zGameModeGet();
GameOstrich zGameGetOstrich();
void zGameSetOstrich(GameOstrich o);
bool32 zGameStateFindEvent(uint32* events, int32 count, int32 mode, int32 event, int32* new_mode, int32* new_state);
void zGameStateSwitchEvent(int32 event);
void zGameStateSwitch(int32 theNewState);
void zGameModeSwitch(eGameMode modeNew);

#endif