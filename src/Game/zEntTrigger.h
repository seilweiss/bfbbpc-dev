#ifndef ZENTTRIGGER_H
#define ZENTTRIGGER_H

#include "zEnt.h"

struct zEntTrigger : zEnt
{
	xMat4x3 triggerMatrix;
	xBox triggerBox;
	uint32 entered;
};

void zEntTriggerInit(void* ent, void* asset);

#endif