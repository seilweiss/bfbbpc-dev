#include "xMath.h"

#include "xMathInlines.h"

#include <stdlib.h>

static bool32 xmath_inited = FALSE;
static bool32 xmath_exited = FALSE;
static uint32 rndseed;

void xMathInit()
{
	if (!xmath_inited)
	{
		xmath_inited = TRUE;
		rndseed = 0;
	}
}

void xMathExit()
{
	if (!xmath_exited)
	{
		xmath_exited = TRUE;
	}
}

float32 xatof(const char* x)
{
	return atof(x);
}

void xsrand(uint32 seed)
{
	rndseed = seed;
}

uint32 xrand()
{
	rndseed = rndseed * 1103515245 + 12345;
	return rndseed;
}

float32 xurand()
{
	return xrand() * (1.0f / 0xFFFFFFFF);
}

float32 xAngleClampFast(float32 a)
{
	if (a < 0.0f)
	{
		return a + TWOPI;
	}
	
	if (a >= TWOPI)
	{
		return a - TWOPI;
	}

	return a;
}

float32 xDangleClamp(float32 a)
{
	a = xfmod(a, TWOPI);

	if (a >= PI)
	{
		return a - TWOPI;
	}

	if (a < -PI)
	{
		return a + TWOPI;
	}
}