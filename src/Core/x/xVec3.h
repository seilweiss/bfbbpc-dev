#ifndef XVEC3_H
#define XVEC3_H

#include "xMath.h"
#include "xMathInlines.h"

struct xVec3
{
	float32 x;
	float32 y;
	float32 z;

	float32 length2() const
	{
		return SQR(x) + SQR(y) + SQR(z);
	}

	float32 length() const
	{
		return xsqrt(length2());
	}
};

float32 xVec3Normalize(xVec3* o, const xVec3* v);

#endif