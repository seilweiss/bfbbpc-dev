#ifndef IMATH3_H
#define IMATH3_H

#include "xMath3.h"

void iMath3Init();
void iBoxInitBoundVec(xBox* b, const xVec3* v);
void iBoxBoundVec(xBox* o, const xBox* b, const xVec3* v);

#endif