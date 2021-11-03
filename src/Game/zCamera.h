#ifndef ZCAMERA_H
#define ZCAMERA_H

#include "xCamera.h"

extern float32 zcam_pad_pyaw_scale;
extern float32 zcam_pad_pitch_scale;
extern float32 zcam_near_d;
extern float32 zcam_near_h;
extern float32 zcam_near_pitch;
extern float32 zcam_far_d;
extern float32 zcam_far_h;
extern float32 zcam_far_pitch;
extern float32 zcam_wall_d;
extern float32 zcam_wall_h;
extern float32 zcam_wall_pitch;
extern float32 zcam_above_d;
extern float32 zcam_above_h;
extern float32 zcam_above_pitch;
extern float32 zcam_below_d;
extern float32 zcam_below_h;
extern float32 zcam_below_pitch;
extern float32 zcam_highbounce_d;
extern float32 zcam_highbounce_h;
extern float32 zcam_highbounce_pitch;
extern float32 zcam_overrot_min;
extern float32 zcam_overrot_mid;
extern float32 zcam_overrot_max;
extern float32 zcam_overrot_rate;
extern float32 zcam_overrot_tstart;
extern float32 zcam_overrot_tend;
extern float32 zcam_overrot_velmin;
extern float32 zcam_overrot_velmax;
extern float32 zcam_overrot_tmanual;

void zCameraReset(xCamera* cam);
void zCameraUpdate(xCamera* cam, float32 dt);

#endif