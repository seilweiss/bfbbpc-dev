#include "iMath3.h"

void iMath3Init()
{
}

UNCHECKED void iBoxInitBoundVec(xBox* b, const xVec3* v)
{
    xVec3Copy(&b->lower, v);
    xVec3Copy(&b->upper, v);
}

UNCHECKED void iBoxBoundVec(xBox* o, const xBox* b, const xVec3* v)
{
    xVec3Init(&o->lower, MIN(v->x, b->lower.x), MIN(v->y, b->lower.y), MIN(v->z, b->lower.z));
    xVec3Init(&o->upper, MAX(v->x, b->upper.x), MAX(v->y, b->upper.y), MAX(v->z, b->upper.z));
}