#ifndef XMATHINLINES_H
#define XMATHINLINES_H

#include "xMath.h"

#include <types.h>
#include <cmath>

inline float32 xasin(float32 x)
{
	return std::asinf(x);
}

inline float32 xacos(float32 x)
{
	return std::acosf(x);
}

inline float32 xatan2(float32 y, float32 x)
{
	return xAngleClampFast(std::atan2f(y, x));
}

inline float32 xsqrt(float32 x)
{
	return sqrtf(x);
}

inline float32 xfmod(float32 x, float32 y)
{
	return std::fmodf(x, y);
}

#endif