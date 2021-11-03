#ifndef ITIME_H
#define ITIME_H

#include <types.h>

#ifdef _WIN32
#include <time.h>

typedef clock_t iTime;

#define iTimeTicksPerSec() CLOCKS_PER_SEC
#endif

#define iTimeTicksToSecs(t) ((float)(t) / iTimeTicksPerSec())
#define iTimeSecsToTicks(s) ((iTime)(s) * iTimeTicksPerSec())
#define iTimeGetSecs() (iTimeTicksToSecs(iTimeGet()))

void iTimeInit();
void iTimeExit();
iTime iTimeGet();
float32 iTimeDiffSec(iTime time);
float32 iTimeDiffSec(iTime t0, iTime t1);
void iTimeGameAdvance(float32 elapsed);
void iTimeSetGame(float32 time);
void iProfileClear(uint32 sceneID);
void iFuncProfileDump();
void iFuncProfileParse(char*, int32);

#endif