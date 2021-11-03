#include "xVec3.h"

float32 xVec3Normalize(xVec3* o, const xVec3* v)
{
	float32 len, len2, len_inv;

	len2 = SQR(v->x) + SQR(v->y) + SQR(v->z);

	if (APPROX_EQUAL(len2, 1.0f))
	{
		o->x = v->x;
		o->y = v->y;
		o->z = v->z;

		len = 1.0f;
	}
	else if (APPROX_EQUAL(len2, 0.0f))
	{
		o->x = 0.0f;
		o->y = 0.0f;
		o->z = 0.0f;

		len = 0.0f;
	}
	else
	{
		len = xsqrt(len2);
		len_inv = 1.0f / len;

		o->x = v->x * len_inv;
		o->y = v->y * len_inv;
		o->z = v->z * len_inv;
	}

	return len;
}