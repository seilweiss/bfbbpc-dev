#ifndef XTRC_H
#define XTRC_H

#include "iTRC.h"

typedef enum _tagTRCState
{
	TRC_Unknown,
	TRC_PadMissing,
	TRC_PadInserted,
	TRC_PadInvalidNoAnalog,
	TRC_PadInvalidType,
	TRC_DiskNotIdentified,
	TRC_DiskIdentified,
	TRC_DiskTrayOpen,
	TRC_DiskTrayClosed,
	TRC_DiskNoDisk,
	TRC_DiskInvalid,
	TRC_DiskRetry,
	TRC_DiskFatal,
	TRC_Total
} TRCState;

typedef struct _tagTRCPadInfo : iTRCPadInfo
{
	int32 id;
	TRCState state;
} TRCPadInfo;

typedef struct _tagTRCDiskInfo
{
	TRCState state;
	TRCState id;
} TRCDiskInfo;

extern TRCPadInfo gTrcPad[4];
extern TRCDiskInfo gTrcDisk;

void xTRCInit();
void xTRCPad(int32 pad_id, TRCState state);

#endif