#ifndef ZENTBUTTON_H
#define ZENTBUTTON_H

#include "zEnt.h"
#include "xEntMotion.h"

struct zEntButtonAsset
{
	uint32 modelPressedInfoID;
	uint32 actMethod;
	int32 initButtonState;
	int32 isReset;
	float32 resetDelay;
	uint32 buttonActFlags;
};

typedef struct _zEntButton : zEnt
{
	zEntButtonAsset* basset;
	xEntMotion motion;
	uint32 state;
	float32 speed;
	uint32 oldState;
	int32 oldMotState;
	float32 counter;
	xModelInstance* modelPressed;
	float32 holdTimer;
	uint32 hold;
	float32 topHeight;
} zEntButton;

void zEntButton_Init(void* ent, void* asset);
void zEntButton_Setup(zEntButton* ent, xScene*);
void zEntButton_Reset(zEntButton* ent, xScene* sc);

#endif