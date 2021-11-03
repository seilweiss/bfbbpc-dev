#include "xPad.h"

#include <string.h>

xRumble mRumbleList[32];
xPad mPad[4];
xPad* gDebugPad;
xPad* gPlayerPad;

bool32 xPadInit()
{
	memset(mPad, 0, sizeof(mPad));
	memset(mRumbleList, 0, sizeof(mRumbleList));

	if (!iPadInit())
	{
		return FALSE;
	}

	gPlayerPad = &mPad[0];

	return TRUE;
}

xPad* xPadEnable(int32 idx)
{
	xPad* p = &mPad[idx];

	if (p->state != ePad_Disabled)
	{
		return p;
	}

	if (idx != 0)
	{
		return p;
	}

	p = iPadEnable(p, idx);

	xPadRumbleEnable(idx, TRUE);

	return p;
}

STUB void xPadRumbleEnable(int32 idx, bool32 enable)
{

}

STUB int32 xPadUpdate(int32 idx, float32 time_passed)
{
	return 0;
}

void xPadKill()
{
	iPadKill();
}