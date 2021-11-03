#include "iTime.h"

iTime sStartupTime;

static float32 sGameTime = 0.0f;

void iTimeInit()
{
#ifdef _WIN32
	sStartupTime = clock();
#endif
}

void iTimeExit()
{
}

iTime iTimeGet()
{
#ifdef _WIN32
	return clock() - sStartupTime;
#endif
}

float32 iTimeDiffSec(iTime time)
{
	return iTimeTicksToSecs(time);
}

float32 iTimeDiffSec(iTime t0, iTime t1)
{
	return iTimeDiffSec(t1 - t0);
}

void iTimeGameAdvance(float32 elapsed)
{
	sGameTime += elapsed;
}

void iTimeSetGame(float32 time)
{
	sGameTime = time;
}

void iProfileClear(uint32 sceneID)
{
}

void iFuncProfileDump()
{
}

void iFuncProfileParse(char*, int32)
{
}