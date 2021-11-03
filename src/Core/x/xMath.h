#ifndef XMATH_H
#define XMATH_H

#include "iMath.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define CLAMP(x, a, b) (MAX((a), MIN((x), (b))))

#define SQR(x) ((x) * (x))

#define ALIGN(x, a) ((x) + ((a)-1) & ~((a)-1))

#define PI 3.14159265359f
#define TWOPI (2.0f*PI)

#define DEG2RAD(x) (float32)(PI * (x) / 180.0f)
#define RAD2DEG(x) (float32)(180.0f * (x) / PI)

void xMathInit();
void xMathExit();
float32 xatof(const char* x);
void xsrand(uint32 seed);
uint32 xrand();
float32 xurand();
float32 xAngleClampFast(float32 a);
float32 xDangleClamp(float32 a);

#endif