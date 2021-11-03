#ifndef ITRC_H
#define ITRC_H

#include <types.h>

typedef enum _tagPadInit
{
	ePadInit_Open1,
	ePadInit_WaitStable2,
	ePadInit_EnableAnalog3,
	ePadInit_EnableAnalog3LetsAllPissOffChris,
	ePadInit_EnableRumble4,
	ePadInit_EnableRumbleTest5,
	ePadInit_PressureS6,
	ePadInit_PressureSTest7,
	ePadInit_Complete8a,
	ePadInit_Complete8b,
	ePadInit_Finished9
} PadInit;

typedef struct _tagiTRCPadInfo
{
	PadInit pad_init;
} iTRCPadInfo;

namespace iTRCDisk {
	void CheckDVDAndResetState();
}

#endif