#ifndef XCOLLIDE_H
#define XCOLLIDE_H

#include "xModel.h"
#include "xQuickCull.h"

union xiMat4x3Union
{
	xMat4x3 xm;
	RwMatrix im;
};

struct xSweptSphere
{
	xVec3 start;
	xVec3 end;
	float32 radius;
	float32 dist;
	xiMat4x3Union basis;
	xiMat4x3Union invbasis;
	xBox box;
	xQCData qcd;
	float32 boxsize;
	float32 curdist;
	xVec3 contact;
	xVec3 polynorm;
	uint32 oid;
	void* optr;
	xModelInstance* mptr;
	int32 hitIt;
	xVec3 worldPos;
	xVec3 worldContact;
	xVec3 worldNormal;
	xVec3 worldTangent;
	xVec3 worldPolynorm;
};

struct xCollis
{
	struct tri_data
	{
		uint32 index;
		float32 r;
		float32 d;
	};

	uint32 flags;
	uint32 oid;
	void* optr;
	xModelInstance* mptr;
	float32 dist;
	xVec3 norm;
	xVec3 tohit;
	xVec3 depen;
	xVec3 hdng;

	union
	{
		struct
		{
			float32 t;
			float32 u;
			float32 v;
		} tuv;

		tri_data tri;
	};
};

struct xScene;

void xCollideInit(xScene* sc);

#endif