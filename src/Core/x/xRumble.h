#ifndef XRUMBLE_H
#define XRUMBLE_H

#include <types.h>
#include <rwcore.h>

typedef enum _tagRumbleType
{
	eRumble_Off,
	eRumble_Hi,
	eRumble_VeryLightHi,
	eRumble_VeryLight,
	eRumble_LightHi,
	eRumble_Light,
	eRumble_MediumHi,
	eRumble_Medium,
	eRumble_HeavyHi,
	eRumble_Heavy,
	eRumble_VeryHeavyHi,
	eRumble_VeryHeavy,
	eRumble_Total,
	eRumbleForceU32 = RWFORCEENUMSIZEINT
} RumbleType;

typedef struct _tagxRumble
{
	RumbleType type;
	float32 seconds;
	_tagxRumble* next;
	int16 active;
	uint16 fxflags;
} xRumble;

#endif