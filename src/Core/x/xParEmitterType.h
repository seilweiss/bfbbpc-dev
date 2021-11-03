#ifndef XPAREMITTERTYPE_H
#define XPAREMITTERTYPE_H

#include "xMath3.h"

struct xPECircle
{
	float32 radius;
	float32 deflection;
	xVec3 dir;
};

typedef struct _tagEmitSphere
{
	float32 radius;
} EmitSphere;

typedef struct _tagEmitRect
{
	float32 x_len;
	float32 z_len;
} EmitRect;

typedef struct _tagEmitLine
{
	xVec3 pos1;
	xVec3 pos2;
	float32 radius;
} EmitLine;

typedef struct _tagEmitVolume
{
	uint32 emit_volumeID;
} EmitVolume;

typedef struct _tagEmitOffsetPoint
{
	xVec3 offset;
} EmitOffsetPoint;

struct xPEVCyl
{
	float32 height;
	float32 radius;
	float32 deflection;
};

struct xPEEntBone
{
	uint8 flags;
	uint8 type;
	uint8 bone;
	uint8 pad1;
	xVec3 offset;
	float32 radius;
	float32 deflection;
};

struct xPEEntBound
{
	uint8 flags;
	uint8 type;
	uint8 pad1;
	uint8 pad2;
	float32 expand;
	float32 deflection;
};

#endif