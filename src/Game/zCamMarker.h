#ifndef ZCAMMARKER_H
#define ZCAMMARKER_H

#include "xBase.h"
#include "xMath3.h"

typedef enum _tagTransType
{
	eTransType_None,
	eTransType_Interp1,
	eTransType_Interp2,
	eTransType_Interp3,
	eTransType_Interp4,
	eTransType_Linear,
	eTransType_Interp1Rev,
	eTransType_Interp2Rev,
	eTransType_Interp3Rev,
	eTransType_Interp4Rev,
	eTransType_Total
} TransType;

typedef struct _tagxCamFollowAsset
{
	float32 rotation;
	float32 distance;
	float32 height;
	float32 rubber_band;
	float32 start_speed;
	float32 end_speed;
} xCamFollowAsset;

typedef struct _tagxCamShoulderAsset
{
	float32 distance;
	float32 height;
	float32 realign_speed;
	float32 realign_delay;
} xCamShoulderAsset;

typedef struct _tagp2CamStaticAsset
{
	uint32 unused;
} p2CamStaticAsset;

typedef struct _tagxCamPathAsset
{
	uint32 assetID;
	float32 time_end;
	float32 time_delay;
} xCamPathAsset;

typedef struct _tagp2CamStaticFollowAsset
{
	float32 rubber_band;
} p2CamStaticFollowAsset;

struct xCamAsset : xBaseAsset
{
	xVec3 pos;
	xVec3 at;
	xVec3 up;
	xVec3 right;
	xVec3 view_offset;
	int16 offset_start_frames;
	int16 offset_end_frames;
	float32 fov;
	float32 trans_time;
	TransType trans_type;
	uint32 flags;
	float32 fade_up;
	float32 fade_down;
	union
	{
		xCamFollowAsset cam_follow;
		xCamShoulderAsset cam_shoulder;
		p2CamStaticAsset cam_static;
		xCamPathAsset cam_path;
		p2CamStaticFollowAsset cam_staticFollow;
	};
	uint32 valid_flags;
	uint32 markerid[2];
	uint8 cam_type;
	uint8 pad[3];
};

struct zCamMarker : xBase
{
	xCamAsset* asset;
};

void zCamMarkerInit(xBase* b, xCamAsset* asset);

#endif