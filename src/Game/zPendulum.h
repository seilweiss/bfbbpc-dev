#ifndef ZPENDULUM_H
#define ZPENDULUM_H

#include "zEnt.h"
#include "xEntMotion.h"

typedef struct _zPendulum : zEnt
{
	xEntMotion motion;
	float32 lt;
	float32 q1t;
	float32 q3t;
} zPendulum;

void zPendulum_Init(void* pend, void* asset);
void zPendulum_Setup(zPendulum* pend, xScene*);
void zPendulum_Reset(zPendulum* pend, xScene* sc);

#endif