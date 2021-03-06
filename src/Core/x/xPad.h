#ifndef XPAD_H
#define XPAD_H

#include "iPad.h"
#include "xRumble.h"
#include "xVec2.h"

typedef struct _tagPadAnalog
{
	int8 x;
	int8 y;
} PadAnalog;

typedef enum _tagPadState
{
	ePad_Disabled,
	ePad_DisabledError,
	ePad_Enabled,
	ePad_Missing,
	ePad_Total
} PadState;

typedef struct _tagxPad
{
	struct analog_data
	{
		xVec2 offset;
		xVec2 dir;
		float32 mag;
		float32 ang;
	};

	uint8 value[22];
	uint8 last_value[22];
	uint32 on;
	uint32 pressed;
	uint32 released;
	PadAnalog analog1;
	PadAnalog analog2;
	PadState state;
	uint32 flags;
	xRumble rumble_head;
	int16 port;
	int16 slot;
	iPad context;
	float32 al2d_timer;
	float32 ar2d_timer;
	float32 d_timer;
	float32 up_tmr[22];
	float32 down_tmr[22];
	analog_data analog[2];
} xPad;

extern xPad mPad[4];
extern xPad* gDebugPad;

bool32 xPadInit();
xPad* xPadEnable(int32 idx);
void xPadRumbleEnable(int32 idx, bool32 enable);
int32 xPadUpdate(int32 idx, float32 time_passed);
void xPadKill();

#endif