#ifndef XVEC3INLINES_H
#define XVEC3INLINES_H

#include "xVec3.h"

inline void xVec3Init(xVec3* v, float32 _x, float32 _y, float32 _z)
{
    v->x = _x;
    v->y = _y;
    v->z = _z;
}

inline void xVec3Inv(xVec3* o, const xVec3* v)
{
    o->x = -v->x;
    o->y = -v->y;
    o->z = -v->z;
}

#ifdef GAMECUBE
void xVec3Copy(xVec3* o, const xVec3* v);
#else
inline void xVec3Copy(xVec3* o, const xVec3* v)
{
    o->x = v->x;
    o->y = v->y;
    o->z = v->z;
}
#endif

inline void xVec3Add(xVec3* o, const xVec3* a, const xVec3* b)
{
    o->x = a->x + b->x;
    o->y = a->y + b->y;
    o->z = a->z + b->z;
}

inline void xVec3AddTo(xVec3* o, const xVec3* v)
{
    o->x += v->x;
    o->y += v->y;
    o->z += v->z;
}

inline void xVec3Sub(xVec3* o, const xVec3* a, const xVec3* b)
{
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
}

inline void xVec3SMul(xVec3* o, const xVec3* v, float32 s)
{
    o->x = v->x * s;
    o->y = v->y * s;
    o->z = v->z * s;
}

inline float32 xVec3Dot(const xVec3* a, const xVec3* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

inline float32 xVec3Length(const xVec3* v)
{
    return xsqrt(SQR(v->x) + SQR(v->y) + SQR(v->z));
}

UNCHECKED inline float32 xVec3Dist(const xVec3* a, const xVec3* b)
{
    return xsqrt(SQR(a->x - b->x) + SQR(a->y - b->y) + SQR(a->z - b->z));
}

#endif