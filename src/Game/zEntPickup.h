#ifndef ZENTPICKUP_H
#define ZENTPICKUP_H

#include "zEnt.h"
#include "xEntDrive.h"

struct zAssetPickup;
class xPar;
struct zParEmitter;

struct zEntPickup : zEnt
{
	xShadowSimpleCache simpShadow_embedded;
	uint32 state;
	zAssetPickup* p;
	void* anim;
	float32 animTime;
	float32 timer;
	float32 shake_timer;
	xVec3 shake_pos;
	int32 snackGateInfoDisplayed;
	float32 fx_timer;
	xPar* fx_par;
	zParEmitter* fx_emit;
	float32 fx_scale;
	xVec3 grab_pos;
	uint32 flyflags;
	int32 flg_opts;
	xVec3 vel;
	xVec3 droppos;
	xBase* followTarget;
	xVec3 followOffset;
	xBase* dropParent;
	xEnt* useThisEntPos;
	xEntDrive drv;
	uint16 pickupFlags;
};

void zEntPickup_Startup();
void zEntPickupInit(void* ent, void* asset);
void zEntPickup_Setup(zEntPickup* p);
void zEntPickup_Setup();
void zEntPickup_Reset(zEntPickup* ent);
void zEntPickup_FlushGrabbed();
void zEntPickup_UpdateFlyToInterface(zEntPickup* ent, uint32 pcount, float32 dt);
void zEntPickup_SceneEnter();
void zEntPickup_RewardPostSetup();

#endif