#ifndef IPAD_H
#define IPAD_H

#include <types.h>

typedef struct _tagiPad
{
	int32 port;
} iPad;

struct _tagxPad;

bool32 iPadInit();
_tagxPad* iPadEnable(_tagxPad* pad, int16 port);
void iPadKill();

#endif