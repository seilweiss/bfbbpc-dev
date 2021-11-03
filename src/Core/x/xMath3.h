#ifndef XMATH3_H
#define XMATH3_H

#include "xMath.h"
#include "xMathInlines.h"
#include "xVec3.h"
#include "xVec3Inlines.h"

#include <string.h>

struct xQuat
{
	xVec3 v;
	float32 s;
};

struct xRot
{
	xVec3 axis;
	float32 angle;
};

struct xMat3x3
{
	xVec3 right;
	int32 flags;
	xVec3 up;
	uint32 pad1;
	xVec3 at;
	uint32 pad2;
};

struct xMat4x3 : xMat3x3
{
	xVec3 pos;
	uint32 pad3;
};

struct xBox
{
	xVec3 upper;
	xVec3 lower;
};

struct xBBox
{
	xVec3 center;
	xBox box;
};

struct xSphere
{
	xVec3 center;
	float32 r;
};

struct xCylinder
{
	xVec3 center;
	float32 r;
	float32 h;
};

struct xParabola
{
	xVec3 initPos;
	xVec3 initVel;
	float32 gravity;
	float32 minTime;
	float32 maxTime;
};

struct xRay3
{
	xVec3 origin;
	xVec3 dir;
	float32 min_t;
	float32 max_t;
	int32 flags;
};

extern const xVec3 g_O3;
extern const xVec3 g_X3;
extern const xVec3 g_Y3;
extern const xVec3 g_Z3;
extern const xVec3 g_NX3;
extern const xVec3 g_NY3;
extern const xVec3 g_NZ3;
extern const xVec3 g_Onez;
extern xMat4x3 g_I3;
extern const xQuat g_IQ;

void xMath3Init();
void xMat3x3GetEuler(const xMat3x3* m, xVec3* a);
void xMat4x3MoveLocalRight(xMat4x3* m, float32 mag);
void xMat4x3MoveLocalUp(xMat4x3* m, float32 mag);
void xMat4x3MoveLocalAt(xMat4x3* m, float32 mag);
void xMat3x3Euler(xMat3x3* m, const xVec3* ypr);
void xMat3x3Euler(xMat3x3* m, float32 yaw, float32 pitch, float32 roll);
void xQuatFromMat(xQuat* q, const xMat3x3* m);
void xQuatToMat(const xQuat* q, xMat3x3* m);
void xQuatToAxisAngle(const xQuat* q, xVec3* a, float32* t);
float32 xQuatNormalize(xQuat* o, const xQuat* q);
void xQuatMul(xQuat* o, const xQuat* a, const xQuat* b);
void xQuatDiff(xQuat* o, const xQuat* a, const xQuat* b);

static void xMat3x3RMulVec(xVec3* o, const xMat3x3* m, const xVec3* v)
{
	float32 x = m->right.x * v->x + m->up.x * v->y + m->at.x * v->z;
	float32 y = m->right.y * v->x + m->up.y * v->y + m->at.y * v->z;
	float32 z = m->right.z * v->x + m->up.z * v->y + m->at.z * v->z;

	o->x = x;
	o->y = y;
	o->z = z;
}

inline void xMat4x3Copy(xMat4x3* o, const xMat4x3* m)
{
	memcpy(o, m, sizeof(xMat4x3));
}

inline void xMat4x3Identity(xMat4x3* m)
{
	xMat4x3Copy(m, &g_I3);
}

inline void xQuatCopy(xQuat* o, const xQuat* q)
{
	o->s = q->s;
	o->v.x = q->v.x;
	o->v.y = q->v.y;
	o->v.z = q->v.z;
}

inline void xQuatSMul(xQuat* o, const xQuat* q, float32 s)
{
	o->s = q->s * s;

	xVec3SMul(&o->v, &q->v, s);
}

inline void xQuatConj(xQuat* o, const xQuat* q)
{
	o->s = q->s;

	xVec3Inv(&o->v, &q->v);
}

inline void xQuatFlip(xQuat* o, const xQuat* q)
{
	o->s = -q->s;

	xVec3Inv(&o->v, &q->v);
}

inline float32 xQuatDot(const xQuat* a, const xQuat* b)
{
	return xVec3Dot(&a->v, &b->v) + a->s * b->s;
}

inline float32 xQuatLength2(const xQuat* q)
{
	return xQuatDot(q, q);
}

inline float32 xQuatGetAngle(const xQuat* q)
{
	if (q->s > 0.99999f)
	{
		return 0.0f;
	}

	if (q->s < -0.99999f)
	{
		return TWOPI;
	}

	return 2.0f * xacos(q->s);
}

#endif