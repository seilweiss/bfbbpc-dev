#ifndef ZENTTELEPORTBOX_H
#define ZENTTELEPORTBOX_H

#include "zEnt.h"
#include "xDynAsset.h"

struct teleport_asset : xDynAsset
{
	uint32 marker;
	uint32 opened;
	uint32 launchAngle;
	uint32 camAngle;
	uint32 targetID;
};

typedef struct _zEntTeleportBox : zEnt
{
	uint32 status;
	teleport_asset* tasset;
	zEnt* target;
	uint32 currPlrState;
	uint32 prevPlrState;
	uint32 jumpInAnim;
	uint32 jumpOutAnim;
	uint32 JOtoOpenAnim;
	uint32 JItoOpenAnim;
	xBox trig[2];
	uint32 currPlayerIn;
	uint32 prevPlayerIn;
	uint32 currPlayerNear;
	uint32 prevPlayerNear;
	float32 lastdt;
	float32 plrCtrlTimer;
} zEntTeleportBox;

void zEntTeleportBox_Init(xBase& data, xDynAsset& asset, ulong32);
void zEntTeleportBox_InitAll();
void zEntTeleportBox_Setup(zEntTeleportBox* ent);

#endif