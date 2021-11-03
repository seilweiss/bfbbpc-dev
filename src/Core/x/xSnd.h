#ifndef XSND_H
#define XSND_H

#include <types.h>

template<int32 count> struct sound_queue
{
	uint32 _playing[count + 1];
	int32 head;
	int32 tail;
};

enum sound_effect
{
	SND_EFFECT_NONE,
	SND_EFFECT_CAVE
};

enum sound_category
{
	SND_CAT_INVALID = -1,
	SND_CAT_GAME = 0,
	SND_CAT_DIALOG,
	SND_CAT_MUSIC,
	SND_CAT_CUTSCENE,
	SND_CAT_UI,
	SND_CAT_NUM_CATEGORIES
};

void xSndInit();
void xSndSceneInit();
void xSndSetEnvironmentalEffect(sound_effect effectType);
void xSndPauseAll(uint32 pause_effects, uint32 pause_streams);
void xSndStopAll(uint32 mask);
void xSndUpdate();
void xSndExit();
void xSndParentDied(uint32 pid);

#endif