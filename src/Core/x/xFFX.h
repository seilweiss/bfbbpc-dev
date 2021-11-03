#ifndef XFFX_H
#define XFFX_H

#include "xEnt.h"

typedef void(*xFFXEffectCallback)(xEnt*, xScene*, float32, void*);

struct xFFXRotMatchState
{
	int32 lgrounded;
	xVec3 lfup;
	xVec3 lfat;
	xVec3 plfat;
	float32 tmr;
	float32 mrate;
	float32 tmatch;
	float32 rrate;
	float32 trelax;
	float32 max_decl;
	xFFXRotMatchState* next;
};

void xFFXPoolInit(uint32 num_ffx);
int16 xFFXAddEffect(xEnt* ent, xFFXEffectCallback dof, void* fd);
void xFFXShakePoolInit(uint32 num);
void xFFXRotMatchPoolInit(uint32 num);
xFFXRotMatchState* xFFXRotMatchAlloc();

#endif