#ifndef XCAMERA_H
#define XCAMERA_H

#include "iCamera.h"
#include "xBase.h"
#include "xBound.h"
#include "xVec4.h"

#include <rwcore.h>

struct xScene;

struct xCamera : xBase
{
	RwCamera* lo_cam;
	xMat4x3 mat;
	xMat4x3 omat;
	xMat3x3 mbasis;
	xBound bound;
	xMat4x3* tgt_mat;
	xMat4x3* tgt_omat;
	xBound* tgt_bound;
	xVec3 focus;
	xScene* sc;
	xVec3 tran_accum;
	float32 fov;
	uint32 flags;
	float32 tmr;
	float32 tm_acc;
	float32 tm_dec;
	float32 ltmr;
	float32 ltm_acc;
	float32 ltm_dec;
	float32 dmin;
	float32 dmax;
	float32 dcur;
	float32 dgoal;
	float32 hmin;
	float32 hmax;
	float32 hcur;
	float32 hgoal;
	float32 pmin;
	float32 pmax;
	float32 pcur;
	float32 pgoal;
	float32 depv;
	float32 hepv;
	float32 pepv;
	float32 orn_epv;
	float32 yaw_epv;
	float32 pitch_epv;
	float32 roll_epv;
	xQuat orn_cur;
	xQuat orn_goal;
	xQuat orn_diff;
	float32 yaw_cur;
	float32 yaw_goal;
	float32 pitch_cur;
	float32 pitch_goal;
	float32 roll_cur;
	float32 roll_goal;
	float32 dct;
	float32 dcd;
	float32 dccv;
	float32 dcsv;
	float32 hct;
	float32 hcd;
	float32 hccv;
	float32 hcsv;
	float32 pct;
	float32 pcd;
	float32 pccv;
	float32 pcsv;
	float32 orn_ct;
	float32 orn_cd;
	float32 orn_ccv;
	float32 orn_csv;
	float32 yaw_ct;
	float32 yaw_cd;
	float32 yaw_ccv;
	float32 yaw_csv;
	float32 pitch_ct;
	float32 pitch_cd;
	float32 pitch_ccv;
	float32 pitch_csv;
	float32 roll_ct;
	float32 roll_cd;
	float32 roll_ccv;
	float32 roll_csv;
	xVec4 frustplane[12];
};

extern int32 xcam_collis_owner_disable;
extern bool32 xcam_do_collis;
extern float32 xcam_collis_radius;
extern float32 xcam_collis_stiffness;

extern float32 gCameraLastFov;

void xCameraInit(xCamera* cam, uint32 width, uint32 height);
void xCameraExit(xCamera* cam);
void xCameraReset(xCamera* cam, float32 d, float32 h, float32 pitch);
void xCameraUpdate(xCamera* cam, float32 dt);
void xCameraBegin(xCamera* cam, bool32 clear);
void xCameraFXBegin(xCamera* cam);
void xCameraFXShake(float32 maxTime, float32 magnitude, float32 cycleMax, float32 rotate_magnitude, float32 radius, xVec3* epicenter, xVec3* player);
void xCameraFXUpdate(xCamera* cam, float32 dt);
void xCameraFXEnd(xCamera* cam);
void xCameraEnd(xCamera* cam, float32 seconds, bool32 update_scrn_fx);
void xCameraShowRaster(xCamera* cam);
void xCameraSetScene(xCamera* cam, xScene* sc);
void xCameraSetTargetMatrix(xCamera* cam, xMat4x3* mat);
void xCameraSetTargetOMatrix(xCamera* cam, xMat4x3* mat);
void xCameraDoCollisions(int32 do_collis, int32 owner);
void xCameraMove(xCamera* cam, uint32 flags, float32 dgoal, float32 hgoal, float32 pgoal, float32 tm, float32 tm_acc, float32 tm_dec);
void xCameraMove(xCamera* cam, const xVec3& loc);
void xCameraMove(xCamera* cam, const xVec3& loc, float32 maxSpeed);
void xCameraFOV(xCamera* cam, float32 fov, float32 maxSpeed, float32 dt);
void xCameraLook(xCamera* cam, uint32 flags, const xQuat* orn_goal, float32 tm, float32 tm_acc, float32 tm_dec);
void xCameraLookYPR(xCamera* cam, uint32 flags, float32 yaw, float32 pitch, float32 roll, float32 tm, float32 tm_acc, float32 tm_dec);
void xCameraRotate(xCamera* cam, const xMat3x3& m, float32 time, float32 accel, float32 decl);
void xCameraRotate(xCamera* cam, const xVec3& v, float32 roll, float32 time, float32 accel, float32 decl);

inline void xCameraSetFOV(xCamera* cam, float32 fov)
{
	cam->fov = fov;
	iCameraSetFOV(cam->lo_cam, fov);
}

inline float32 xCameraGetFOV(const xCamera* cam)
{
	return cam->fov;
}

#endif