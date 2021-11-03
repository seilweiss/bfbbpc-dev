#ifndef ZPLATFORM_H
#define ZPLATFORM_H

#include "zEnt.h"
#include "xEntMotion.h"
#include "xEntDrive.h"

struct xPlatformERData
{
	int32 nodata;
};

struct xPlatformOrbitData
{
	int32 nodata;
};

struct xPlatformSplineData
{
	int32 nodata;
};

struct xPlatformMPData
{
	int32 nodata;
};

struct xPlatformMechData
{
	int32 nodata;
};

struct xPlatformPenData
{
	int32 nodata;
};

struct xPlatformConvBeltData
{
	float32 speed;
};

struct xPlatformFallingData
{
	float32 speed;
	uint32 bustModelID;
};

struct xPlatformFRData
{
	float32 fspeed;
	float32 rspeed;
	float32 ret_delay;
	float32 post_ret_delay;
};

struct xPlatformBreakawayData
{
	float32 ba_delay;
	uint32 bustModelID;
	float32 reset_delay;
	uint32 breakflags;
};

struct xPlatformSpringboardData
{
	float32 jmph[3];
	float32 jmpbounce;
	uint32 animID[3];
	xVec3 jmpdir;
	uint32 springflags;
};

struct xPlatformTeeterData
{
	float32 itilt;
	float32 maxtilt;
	float32 invmass;
};

struct xPlatformPaddleData
{
	int32 startOrient;
	int32 countOrient;
	float32 orientLoop;
	float32 orient[6];
	uint32 paddleFlags;
	float32 rotateSpeed;
	float32 accelTime;
	float32 decelTime;
	float32 hubRadius;
};

struct xPlatformFMData
{
	int32 nothingyet;
};

struct xPlatformAsset
{
	uint8 type;
	uint8 pad;
	uint16 flags;
	union
	{
		xPlatformERData er;
		xPlatformOrbitData orb;
		xPlatformSplineData spl;
		xPlatformMPData mp;
		xPlatformMechData mech;
		xPlatformPenData pen;
		xPlatformConvBeltData cb;
		xPlatformFallingData fall;
		xPlatformFRData fr;
		xPlatformBreakawayData ba;
		xPlatformSpringboardData sb;
		xPlatformTeeterData teet;
		xPlatformPaddleData paddle;
		xPlatformFMData fm;
	};
};

struct zPlatFMRunTime
{
	uint32 flags;
	float32 tmrs[12];
	float32 ttms[12];
	float32 atms[12];
	float32 dtms[12];
	float32 vms[12];
	float32 dss[12];
};

struct zPlatform : zEnt
{
	xPlatformAsset* passet;
	xEntMotion motion;
	uint16 state;
	uint16 plat_flags;
	float32 tmr;
	int32 ctr;
	xMovePoint* src;
	xModelInstance* am;
	xModelInstance* bm;
	int32 moving;
	xEntDrive drv;
	zPlatFMRunTime* fmrt;
	float32 pauseMult;
	float32 pauseDelta;
};

void zPlatform_Init(void* plat, void* asset);
void zPlatform_Setup(zPlatform* plat, xScene*);
void zPlatform_Reset(zPlatform* plat, xScene* sc);

#endif