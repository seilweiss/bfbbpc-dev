#include "zCamera.h"

#include "xMath.h"

float32 zcam_pad_pyaw_scale = 0.18124573f;
float32 zcam_pad_pitch_scale = 0.01923077f;
float32 zcam_near_d = 3.0f;
float32 zcam_near_h = 1.8f;
float32 zcam_near_pitch = DEG2RAD(10.0f);
float32 zcam_far_d = 5.0f;
float32 zcam_far_h = 3.0f;
float32 zcam_far_pitch = DEG2RAD(15.0f);
float32 zcam_wall_d = 7.5f;
float32 zcam_wall_h = 2.0f;
float32 zcam_wall_pitch = DEG2RAD(18.0f);
float32 zcam_above_d = 0.2f;
float32 zcam_above_h = 2.2f;
float32 zcam_above_pitch = DEG2RAD(70.0f);
float32 zcam_below_d = 0.6f;
float32 zcam_below_h = 0.2f;
float32 zcam_below_pitch = DEG2RAD(-70.0f);
float32 zcam_highbounce_d = 0.2f;
float32 zcam_highbounce_h = 5.0f;
float32 zcam_highbounce_pitch = DEG2RAD(89.0f);
float32 zcam_overrot_min = DEG2RAD(25.0f);
float32 zcam_overrot_mid = DEG2RAD(90.0f);
float32 zcam_overrot_max = DEG2RAD(170.0f);
float32 zcam_overrot_rate = 0.1f;
float32 zcam_overrot_tstart = 1.5f;
float32 zcam_overrot_tend = 2.5f;
float32 zcam_overrot_velmin = 3.0f;
float32 zcam_overrot_velmax = 5.0f;
float32 zcam_overrot_tmanual = 1.5f;

STUB void zCameraReset(xCamera* cam)
{

}

STUB void zCameraUpdate(xCamera* cam, float32 dt)
{

}