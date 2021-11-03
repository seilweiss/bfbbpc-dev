#include "xTRC.h"

#include <string.h>

TRCPadInfo gTrcPad[4];
TRCDiskInfo gTrcDisk;

void xTRCInit()
{
	memset(gTrcPad, 0, sizeof(gTrcPad));

	gTrcPad[0].id = 0;
	gTrcPad[1].id = 1;
	gTrcPad[2].id = 2;
	gTrcPad[3].id = 3;

	memset(&gTrcDisk, 0, sizeof(gTrcDisk));
}

void xTRCPad(int32 pad_id, TRCState state)
{
}