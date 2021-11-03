#include "xCamera.h"

#include "xstransvc.h"
#include "xScrFx.h"
#include "xScene.h"

#include <rwcore.h>
#include <rpworld.h>

struct cameraFXShake
{
    float32 magnitude;
    xVec3 dir;
    float32 cycleTime;
    float32 cycleMax;
    float32 dampen;
    float32 dampenRate;
    float32 rotate_magnitude;
    float32 radius;
    xVec3* epicenterP;
    xVec3 epicenter;
    xVec3* player;
};

struct cameraFXZoom
{
    float32 holdTime;
    float32 vel;
    float32 accel;
    float32 distance;
    uint32 mode;
    float32 velCur;
    float32 distanceCur;
    float32 holdTimeCur;
};

#define CAMERAFX_ZOOM_MODE_0 0
#define CAMERAFX_ZOOM_MODE_1 1
#define CAMERAFX_ZOOM_MODE_2 2
#define CAMERAFX_ZOOM_MODE_3 3

struct cameraFX
{
    int32 type;
    int32 flags;
    float32 elapsedTime;
    float32 maxTime;
    union
    {
        cameraFXShake shake;
        cameraFXZoom zoom;
    };
};

#define CAMERAFX_TYPE_0 0
#define CAMERAFX_TYPE_ZOOM 1
#define CAMERAFX_TYPE_SHAKE 2
#define CAMERAFX_TYPE_COUNT 3

void xCameraFXZoomUpdate(cameraFX* f, float32 dt, const xMat4x3*, xMat4x3* m);
void xCameraFXShakeUpdate(cameraFX* f, float32 dt, const xMat4x3*, xMat4x3* m);

struct cameraFXTableEntry
{
    int32 type;
    void (*func)(cameraFX*, float32, const xMat4x3*, xMat4x3*);
    void (*funcKill)(cameraFX*);
};

static bool32 sCamCollis = FALSE;

int32 xcam_collis_owner_disable = 0;
bool32 xcam_do_collis = TRUE;
float32 xcam_collis_radius = 0.4f;
float32 xcam_collis_stiffness = 0.3f;

static RpAtomic* sInvisWallHack = NULL;

float32 gCameraLastFov = 0.0f;

static xMat4x3 sCameraFXMatOld;

cameraFX sCameraFX[10];

cameraFXTableEntry sCameraFXTable[CAMERAFX_TYPE_COUNT] =
{
    { CAMERAFX_TYPE_0, NULL, NULL },
    { CAMERAFX_TYPE_ZOOM, xCameraFXZoomUpdate, NULL },
    { CAMERAFX_TYPE_SHAKE, xCameraFXShakeUpdate, NULL }
};

static void xCameraFXInit();

inline void add_camera_tweaks()
{
}

void xCameraInit(xCamera* cam, uint32 width, uint32 height)
{
    xCameraFXInit();

    cam->lo_cam = iCameraCreate(width, height, 1);

    xCameraSetFOV(cam, 75.0f);

    cam->bound.sph.center.x = 0.0f;
    cam->bound.sph.center.y = 0.0f;
    cam->bound.sph.center.z = 0.0f;
    cam->bound.sph.r = 0.5f;
    cam->tgt_mat = NULL;
    cam->tgt_omat = NULL;
    cam->tgt_bound = NULL;
    cam->sc = NULL;
    cam->tran_accum.x = 0.0f;
    cam->tran_accum.y = 0.0f;
    cam->tran_accum.z = 0.0f;

    add_camera_tweaks();
}

void xCameraExit(xCamera* cam)
{
    if (cam->lo_cam)
    {
        iCameraDestroy(cam->lo_cam);
        cam->lo_cam = NULL;
    }
}

void xCameraReset(xCamera* cam, float32 d, float32 h, float32 pitch)
{
    sInvisWallHack = (RpAtomic*)xSTFindAsset(0xB8895D14, NULL);

    xMat4x3Identity(&cam->mat);

    cam->omat = cam->mat;

    cam->focus.x = 0.0f;
    cam->focus.y = 0.0f;
    cam->focus.z = 10.0f;

    cam->tran_accum.x = 0.0f;
    cam->tran_accum.y = 0.0f;
    cam->tran_accum.z = 0.0f;

    cam->flags = 0;

    float32 goal_p = PI;

    if (cam->tgt_mat)
    {
        goal_p += xatan2(cam->tgt_mat->at.x, cam->tgt_mat->at.z);
    }

    xCameraMove(cam, 0x2E, d, h, goal_p, 0.0f, 2.0f/3.0f, 2.0f/3.0f);

    cam->pitch_goal = pitch;
    cam->pitch_cur = pitch;
    cam->roll_cur = 0.0f;

    xMat3x3Euler(&cam->mat, cam->yaw_cur, cam->pitch_cur, cam->roll_cur);

    cam->omat = cam->mat;

    cam->yaw_ct = 1.0f;
    cam->yaw_cd = 1.0f;
    cam->yaw_ccv = 0.65f;
    cam->yaw_csv = 1.0f;

    cam->pitch_ct = 1.0f;
    cam->pitch_cd = 1.0f;
    cam->pitch_ccv = 0.7f;
    cam->pitch_csv = 1.0f;

    cam->roll_ct = 1.0f;
    cam->roll_cd = 1.0f;
    cam->roll_ccv = 0.7f;
    cam->roll_csv = 1.0f;

    cam->flags |= 0x80;

    xcam_do_collis = TRUE;
    xcam_collis_owner_disable = 0;
}

static void xCam_buildbasis(xCamera* cam)
{
    if (cam->tgt_mat)
    {
        float32 d2d;
        float32 dx__ = cam->mat.pos.x - cam->tgt_mat->pos.x;
        float32 dz__ = cam->mat.pos.z - cam->tgt_mat->pos.z;

        float32 dist2 = SQR(dx__) + SQR(dz__);
        float32 dist_inv;

        if (APPROX_EQUAL(dist2, 1.0f))
        {
            cam->mbasis.at.x = dx__;
            cam->mbasis.at.z = dz__;

            d2d = 1.0f;
        }
        else if (APPROX_EQUAL(dist2, 0.0f))
        {
            cam->mbasis.at.x = 0.0f;
            cam->mbasis.at.z = 0.0f;

            d2d = 0.0f;
        }
        else
        {
            d2d = xsqrt(dist2);
            dist_inv = 1.0f / d2d;

            cam->mbasis.at.x = dx__ * dist_inv;
            cam->mbasis.at.z = dz__ * dist_inv;
        }

        if (d2d < EPSILON)
        {
            cam->mbasis.at.x = cam->mat.at.x;
            cam->mbasis.at.z = cam->mat.at.z;

            dist2 = xsqrt(SQR(cam->mbasis.at.x) + SQR(cam->mbasis.at.z));

            if (dist2 > 0.001f)
            {
                dist_inv = 1.0f / dist2;

                cam->mbasis.at.x *= dist_inv;
                cam->mbasis.at.z *= dist_inv;
            }
            else
            {
                cam->mbasis.at.x = isin(cam->pcur);
                cam->mbasis.at.z = icos(cam->pcur);
            }
        }

        cam->mbasis.at.y = 0.0f;
        cam->mbasis.up.x = 0.0f;
        cam->mbasis.up.y = 1.0f;
        cam->mbasis.up.z = 0.0f;
        cam->mbasis.right.x = cam->mbasis.at.z;
        cam->mbasis.right.y = 0.0f;
        cam->mbasis.right.z = -cam->mbasis.at.x;
    }
}

static void xCam_cyltoworld(xVec3* v, const xMat4x3* tgt_mat, float32 d, float32 h, float32 p, uint32 flags)
{
    if (flags & 0x10)
    {
        v->y = h;
    }
    else
    {
        v->y = h + tgt_mat->pos.y;
    }

    if (flags & 0x20)
    {
        v->x = d * isin(p) + tgt_mat->pos.x;
        v->z = d * icos(p) + tgt_mat->pos.z;
    }
    else
    {
        p += xatan2(tgt_mat->at.x, tgt_mat->at.z);

        v->x = d * isin(p) + tgt_mat->pos.x;
        v->z = d * icos(p) + tgt_mat->pos.z;
    }
}

static void xCam_worldtocyl(float32& d, float32& h, float32& p, const xMat4x3* tgt_mat, const xVec3* v, uint32 flags)
{
    float32 lx, lz;

    float32 dx__ = v->x - tgt_mat->pos.x;
    float32 dz__ = v->z - tgt_mat->pos.z;

    float32 dist2 = SQR(dx__) + SQR(dz__);
    float32 dist_inv;

    if (APPROX_EQUAL(dist2, 1.0f))
    {
        lx = dx__;
        lz = dz__;
        d = 1.0f;
    }
    else if (APPROX_EQUAL(dist2, 0.0f))
    {
        lx = 0.0f;
        lz = 0.0f;
        d = 0.0f;
    }
    else
    {
        d = xsqrt(dist2);

        dist_inv = 1.0f / d;

        lx = dx__ * dist_inv;
        lz = dz__ * dist_inv;
    }

    if (flags & 0x10)
    {
        h = v->y;
    }
    else
    {
        h = v->y - tgt_mat->pos.y;
    }

    p = xatan2(lx, lz);

    if (!(flags & 0x20))
    {
        float32 tgt_p = p - xatan2(tgt_mat->at.x, tgt_mat->at.z);

        p = xDangleClamp(tgt_p);
    }
}

static void xCam_CorrectD(xCamera* r3, float32 f1, float32 f2, float32 f3)
{
    float32 tmp1, tmp2, tmp3;

    tmp1 = (10.0f/7.0f) * (2.0f * f1 - f2 * f3);
    tmp1 -= f2;
    tmp1 *= f3;

    tmp2 = r3->mbasis.at.x * tmp1;
    tmp3 = r3->mbasis.at.z * tmp1;

    r3->mat.pos.x += tmp2;
    r3->mat.pos.z += tmp3;
}

static void xCam_CorrectH(xCamera* r3, float32 f1, float32 f2, float32 f3)
{
    float32 tmp;

    f2 = 0.15f * f2;

    tmp = (10.0f / 7.0f) * (f1 - f2 * f3);
    tmp -= f2;
    tmp *= f3;

    r3->mat.pos.y += tmp;
}

static void xCam_CorrectP(xCamera* r3, float32 f1, float32 f2, float32 f3)
{
    float32 tmp1, tmp2, tmp3;

    f2 = 0.15f * f2;

    tmp1 = 2.5f * (2.0f * f1 - f2 * f3);
    tmp1 -= f2;
    tmp1 *= f3;

    tmp2 = r3->mbasis.right.x * tmp1;
    tmp3 = r3->mbasis.right.z * tmp1;

    r3->mat.pos.x += tmp2;
    r3->mat.pos.z += tmp3;
}

static void xCam_DampP(xCamera* r3, float32 f1, float32 f2)
{
    float32 tmp1, tmp2;

    f1 = -6.0f * f1;
    f2 = f1 * f2 * f2;

    tmp1 = r3->mbasis.right.x * f2;
    tmp2 = r3->mbasis.right.z * f2;

    r3->mat.pos.x += tmp1;
    r3->mat.pos.z += tmp2;
}

static void xCam_CorrectYaw(xCamera* r3, float32 f1, float32 f2, float32 f3)
{
    float32 tmp1, tmp2;

    tmp1 = 1.0f / r3->yaw_ct;

    tmp2 = 2.0f * r3->yaw_cd * f1 - f2 * f3;
    tmp2 = tmp1 * tmp2;
    tmp2 -= f2;
    tmp2 *= r3->yaw_csv * f3;

    r3->yaw_cur += tmp2;
}

static void xCam_CorrectPitch(xCamera* r3, float32 f1, float32 f2, float32 f3)
{
    float32 tmp1, tmp2;

    tmp1 = 1.0f / r3->pitch_ct;

    tmp2 = 2.0f * r3->pitch_cd * f1 - f2 * f3;
    tmp2 = tmp1 * tmp2;
    tmp2 -= f2;
    tmp2 *= r3->pitch_csv * f3;

    r3->pitch_cur += tmp2;
}

static void xCam_CorrectRoll(xCamera* r3, float32 f1, float32 f2, float32 f3)
{
    float32 tmp1, tmp2;

    tmp1 = 1.0f / r3->roll_ct;

    tmp2 = 2.0f * r3->roll_cd * f1 - f2 * f3;
    tmp2 = tmp1 * tmp2;
    tmp2 -= f2;
    tmp2 *= r3->roll_csv * f3;

    r3->roll_cur += tmp2;
}

STUB static void _xCameraUpdate(xCamera* cam, float32 dt)
{
#if 0
    if (!cam->tgt_mat)
    {
        return;
    }

    static float32 last_dt = 1.0f / 60.0f;

    xCam_worldtocyl(cam->dcur, cam->hcur, cam->pcur, cam->tgt_mat, &cam->mat.pos, cam->flags);

    float32 wcvx, wcvy, wcvz;
    float32 m = 1.0f / last_dt; // f29

    wcvx = (cam->mat.pos.x - cam->omat.pos.x) * m; // f26
    wcvy = (cam->mat.pos.y - cam->omat.pos.y) * m; // f27
    wcvz = (cam->mat.pos.z - cam->omat.pos.z) * m; // f28

    cam->omat.pos = cam->mat.pos;

    xCam_buildbasis(cam);

    float32 dcv = wcvx * cam->mbasis.at.x + wcvz * cam->mbasis.at.z; // f30
    float32 hcv = wcvy; // f25
    float32 pcv = wcvx * cam->mbasis.right.x + wcvz * cam->mbasis.right.z; // f24

    cam->mat.pos.x += wcvx * dt;
    cam->mat.pos.y += wcvy * dt;
    cam->mat.pos.z += wcvz * dt;

    if (cam->flags & 0x1)
    {
        float32 tnext = cam->tmr - dt; // f26

        if (tnext <= 0.0f)
        {
            cam->flags &= ~0x1;
            cam->tmr = 0.0f;
            cam->omat.pos = cam->mat.pos;
        }
        else
        {
            float32 dtg = cam->dgoal - cam->dcur; // f27
            float32 htg = cam->hgoal - cam->hcur; // f28
            float32 ptg = 0.5f * xDangleClamp(cam->pgoal - cam->pcur) * (cam->dgoal + cam->dcur); // f1

            float32 dsv, hsv, psv;

            if (tnext <= cam->tm_dec)
            {
                float32 T_inv = 1.0f / cam->tmr;

                dsv = T_inv * (2.0f * dtg - dcv * dt);
                hsv = T_inv * (2.0f * htg - hcv * dt);
                psv = T_inv * (2.0f * ptg - pcv * dt);
            }
            else if (tnext <= cam->tm_acc)
            {
                float32 T_inv = 1.0f / (2.0f * cam->tmr - dt - cam->tm_dec);

                dsv = T_inv * (2.0f * dtg - dcv * dt);
                hsv = T_inv * (2.0f * htg - hcv * dt);
                psv = T_inv * (2.0f * ptg - pcv * dt);
            }
            else
            {
                float32 it = 2.0f / (cam->tmr + cam->tm_acc - cam->tm_dec);
                float32 ot = cam->tmr - dt + cam->tm_acc - cam->tm_dec;
                float32 T_inv = 1.0f / (cam->tmr - cam->tm_acc);

                dsv = T_inv * (2.0f * dtg - ((cam->depv + dtg * it) * 0.5f * ot) - dcv * dt);
                hsv = T_inv * (2.0f * htg - ((cam->hepv + htg * it) * 0.5f * ot) - hcv * dt);
                psv = T_inv * (2.0f * ptg - ((cam->pepv + ptg * it) * 0.5f * ot) - pcv * dt);
            }

            float32 dpv = dsv - dcv;
            float32 hpv = hsv - hcv;
            float32 ppv = psv - pcv;
            float32 vax = cam->mbasis.right.x * ppv + cam->mbasis.at.x * dpv;
            float32 vay = cam->mbasis.right.y * ppv + hsv * hcv;
            float32 vaz = cam->mbasis.right.z * ppv + cam->mbasis.at.z * dpv;

            cam->mat.pos.x += vax * dt;
            cam->mat.pos.y += vay * dt;
            cam->mat.pos.z += vaz * dt;
            cam->tmr = tnext;
        }
    }
    else
    {
        //float32 s; // unused

        if (cam->flags & 0x2)
        {
            if (!APPROX_EQUAL(cam->dcur / cam->dgoal, 1.0f))
            {
                float32 dtg = cam->dcur / cam->dgoal;
                xCam_CorrectD(cam, dtg, dcv, dt);
            }
        }
        else if (cam->dmax > cam->dmin)
        {
            if (cam->dcur < cam->dmin)
            {
                float32 dtg = cam->dmin - cam->dcur;
                xCam_CorrectD(cam, dtg, dcv, dt);
            }
            else if (cam->dcur > cam->dmax)
            {
                float32 dtg = cam->dmax - cam->dcur;
                xCam_CorrectD(cam, dtg, dcv, dt);
            }
        }

        if (cam->flags & 0x4)
        {
            if (!APPROX_EQUAL(cam->hcur / cam->hgoal, 1.0f))
            {
                float32 htg = cam->hcur / cam->hgoal;
                xCam_CorrectH(cam, htg, hcv, dt);
            }
        }
        else if (cam->hmax > cam->hmin)
        {
            if (cam->hcur < cam->hmin)
            {
                float32 htg = cam->hmin - cam->hcur;
                xCam_CorrectH(cam, htg, hcv, dt);
            }
            else if (cam->hcur > cam->hmax)
            {
                float32 htg = cam->hmax - cam->hcur;
                xCam_CorrectH(cam, htg, hcv, dt);
            }
        }

        if (cam->flags & 0x8)
        {
            if (!APPROX_EQUAL(cam->pcur / cam->pgoal, 1.0f))
            {
                float32 ptg = cam->dcur * xDangleClamp(cam->pgoal - cam->pcur);
                xCam_CorrectP(cam, ptg, pcv, dt);
            }
        }
        else if (cam->pmax > cam->pmin)
        {
            float32 dphi = xDangleClamp(cam->pmax - cam->pcur);
            float32 dplo = xDangleClamp(cam->pmin - cam->pcur);

            if (dplo > 0.0f && (dphi > 0.0f || iabs(dplo) <= iabs(dphi)))
            {
                float32 ptg = (0.00001f + dplo) * cam->dcur;
                xCam_CorrectP(cam, ptg, pcv, dt);
            }
            else if (dphi < 0.0f)
            {
                float32 ptg = (dphi - 0.00001f) * cam->dcur;
                xCam_CorrectP(cam, ptg, pcv, dt);
            }
            else
            {
                xCam_DampP(cam, pcv, dt);
            }
        }
        else
        {
            xCam_DampP(cam, pcv, dt);
        }
    }

    if (cam->flags & 0x80)
    {
        xVec3 oeu, eu;

        xMat3x3GetEuler(&cam->mat, &eu);
        xMat3x3GetEuler(&cam->omat, &oeu);

        float32 m = 1.0f / last_dt;

        float32 ycv = m * xDangleClamp(eu.x - oeu.x); // f24
        float32 pcv = m * xDangleClamp(eu.y - oeu.y); // f25
        float32 rcv = m * xDangleClamp(eu.z - oeu.z); // f23

        ycv *= cam->yaw_ccv;
        pcv *= cam->pitch_ccv;
        rcv *= cam->roll_ccv;

        cam->omat = cam->mat;

        cam->yaw_cur += ycv * dt;
        cam->pitch_cur += pcv * dt;
        cam->roll_cur += rcv * dt;

        if (cam->flags & 0x40)
        {
            float32 tnext = cam->ltmr - dt;

            if (tnext <= 0.0f)
            {
                cam->flags &= ~0x40;
                cam->ltmr = 0.0f;
            }
            else
            {
                float32 ytg = xDangleClamp(cam->yaw_goal - cam->yaw_cur); // f29
                float32 ptg = xDangleClamp(cam->pitch_goal - cam->pitch_cur); // f30
                float32 rtg = xDangleClamp(cam->roll_goal - cam->roll_cur); // f1

                float32 ysv, psv, rsv;

                if (tnext <= cam->ltm_dec)
                {
                    float32 T_inv = 1.0f / cam->ltmr;

                    ysv = T_inv * (2.0f * ytg - ycv * dt);
                    psv = T_inv * (2.0f * ptg - pcv * dt);
                    rsv = T_inv * (2.0f * rtg - rcv * dt);
                }
                else if (tnext <= cam->ltm_acc)
                {
                    float32 T_inv = 1.0f / (2.0f * cam->ltmr - dt - cam->ltm_dec);

                    ysv = T_inv * (2.0f * ytg - ycv * dt);
                    psv = T_inv * (2.0f * ptg - pcv * dt);
                    rsv = T_inv * (2.0f * rtg - rcv * dt);
                }
                else
                {
                    float32 it = 2.0f / (cam->ltmr + cam->ltm_acc - cam->ltm_dec);
                    float32 ot = cam->ltmr - dt + cam->ltm_acc - cam->ltm_dec;
                    float32 T_inv = 1.0f / (cam->ltmr - cam->ltm_acc);

                    ysv = T_inv * (2.0f * ytg - ((cam->yaw_epv + ytg * it) * 0.5f * ot) - ycv * dt);
                    psv = T_inv * (2.0f * ptg - ((cam->pitch_epv + ptg * it) * 0.5f * ot) - pcv * dt);
                    rsv = T_inv * (2.0f * rtg - ((cam->roll_epv + rtg * it) * 0.5f * ot) - rcv * dt);
                }

                float32 ypv = ysv - ycv;
                float32 ppv = psv - pcv;
                float32 rpv = rsv - rcv;
                float32 vax = ypv;
                float32 vay = ppv;
                float32 vaz = rpv;

                cam->yaw_cur += vax * dt;
                cam->pitch_cur += vay * dt;
                cam->roll_cur += vaz * dt;

                xMat3x3Euler(&cam->mat, cam->yaw_cur, cam->pitch_cur, cam->roll_cur);

                cam->ltmr = tnext;
            }
        }
        else
        {
            if (!APPROX_EQUAL(cam->yaw_cur, cam->yaw_goal))
            {
                float32 ytg = xDangleClamp(cam->yaw_goal - cam->yaw_cur);
                xCam_CorrectYaw(cam, ytg, ycv, dt);
            }

            if (!APPROX_EQUAL(cam->pitch_cur, cam->pitch_goal))
            {
                float32 ptg = xDangleClamp(cam->pitch_goal - cam->pitch_cur);
                xCam_CorrectPitch(cam, ptg, pcv, dt);
            }

            if (!APPROX_EQUAL(cam->roll_cur, cam->roll_goal))
            {
                float32 rtg = xDangleClamp(cam->roll_cur - cam->roll_goal);
                xCam_CorrectRoll(cam, rtg, rcv, dt);
            }

            xMat3x3Euler(&cam->mat, cam->yaw_cur, cam->pitch_cur, cam->roll_cur);
        }
    }
    else
    {
        xQuatFromMat(&cam->orn_cur, &cam->mat);

        xQuat oq;
        xQuatFromMat(&oq, &cam->omat);

        xQuat qdiff_o_c;
        xQuatDiff(&qdiff_o_c, &oq, &cam->orn_cur);

        xRot rot_cv;
        xQuatToAxisAngle(&qdiff_o_c, &rot_cv.axis, &rot_cv.angle);

        rot_cv.angle *= m;
        rot_cv.angle = 0.0f; // ???

        cam->omat = cam->mat;

        xVec3 f;
        xMat3x3RMulVec(&f, cam->tgt_mat, &cam->focus);
        xVec3AddTo(&f, &cam->tgt_mat->pos);

        float32 atx, aty, atz;
        float32 dist;
        float32 dx__ = cam->tgt_mat->pos.x - cam->mat.pos.x;
        float32 dz__ = cam->tgt_mat->pos.z - cam->mat.pos.z;
        float32 dist2 = SQR(dx__) + SQR(dz__);
        float32 dist_inv;

        if (APPROX_EQUAL(dist2, 1.0f))
        {
            atx = dx__;
            atz = dz__;
        }
        else if (APPROX_EQUAL(dist, 0.0f))
        {
            atx = 0.0f;
            atz = 0.0f;
        }
        else
        {
            dist = xsqrt(dist2);
            dist_inv = 1.0f / dist;

            atx = dx__ * dist_inv;
            atz = dz__ * dist_inv;
        }

        aty = 0.0f;

        dist2 =
            cam->tgt_mat->at.x * atx +
            cam->tgt_mat->at.y * aty +
            cam->tgt_mat->at.z * atz;

        if (dist2 < 0.0f)
        {
            // WIP

        }
    }
    
    float32 mpx;
    float32 mpy;
    float32 mpz;
    float32 s;
    xMat3x3 des_mat;
    xMat3x3 latgt;
    float32 ang_dist;
    xQuat a;
    xQuat b;
    xQuat o;
    float32 s;
    xQuat desq;
    xQuat difq;
    xQuat newq;
    xSweptSphere sws;
    xVec3 tgtpos;
    xRay3 ray;
    float32 one_len;
    float32 stopdist;
#endif
}

void xCameraUpdate(xCamera* cam, float32 dt)
{
    int32 i;
    int32 num_updates;
    float32 sdt;

    num_updates = std::ceilf(144.0f * dt);

    sdt = dt / num_updates;

    for (i = 0; i < num_updates; i++)
    {
        sCamCollis = (i == num_updates - 1);

        _xCameraUpdate(cam, sdt);
    }
}

void xCameraBegin(xCamera* cam, bool32 clear)
{
    iCameraBegin(cam->lo_cam, clear);
    iCameraFrustumPlanes(cam->lo_cam, cam->frustplane);
    iCameraUpdateFog(cam->lo_cam, 0);
}

void xCameraFXBegin(xCamera* cam)
{
    xMat4x3Identity(&sCameraFXMatOld);
    xMat4x3Copy(&sCameraFXMatOld, &cam->mat);
}

static void xCameraFXInit()
{
    memset(sCameraFX, 0, sizeof(sCameraFX));

    sCameraFX[0].flags = 0;
    sCameraFX[1].flags = 0;
    sCameraFX[2].flags = 0;
    sCameraFX[3].flags = 0;
    sCameraFX[4].flags = 0;
    sCameraFX[5].flags = 0;
    sCameraFX[6].flags = 0;
    sCameraFX[7].flags = 0;
    sCameraFX[8].flags = 0;
    sCameraFX[9].flags = 0;
}

cameraFX* xCameraFXAlloc()
{
    int32 i;
    cameraFX* f;

    for (i = 0; i < sizeof(sCameraFX) / sizeof(cameraFX); i++)
    {
        f = &sCameraFX[i];

        if (f->flags == 0)
        {
            f->flags = 0x1;
            f->elapsedTime = 0.0f;
            f->maxTime = 0.0f;

            return f;
        }
    }

    return NULL;
}

void xCameraFXZoomUpdate(cameraFX* f, float32 dt, const xMat4x3*, xMat4x3* m)
{
    switch (f->zoom.mode)
    {
    case CAMERAFX_ZOOM_MODE_0:
    {
        f->zoom.velCur += f->zoom.accel * dt;
        f->zoom.distanceCur += f->zoom.velCur * dt;

        if (f->zoom.distanceCur >= f->zoom.distance)
        {
            f->zoom.distanceCur = f->zoom.distance;
            f->zoom.mode = CAMERAFX_ZOOM_MODE_2;
            f->zoom.holdTimeCur = 0.0f;
        }

        xMat4x3MoveLocalAt(m, f->zoom.distanceCur);

        break;
    }
    case CAMERAFX_ZOOM_MODE_2:
    {
        f->zoom.holdTimeCur += dt;

        if (f->zoom.holdTimeCur > f->zoom.holdTime)
        {
            f->zoom.mode = CAMERAFX_ZOOM_MODE_1;
            f->zoom.distanceCur = f->zoom.distance;
            f->zoom.velCur = f->zoom.vel;
        }

        xMat4x3MoveLocalAt(m, f->zoom.distance);

        break;
    }
    case CAMERAFX_ZOOM_MODE_1:
    {
        f->zoom.velCur += f->zoom.accel * dt;
        f->zoom.distanceCur -= f->zoom.velCur * dt;

        if (f->zoom.distanceCur <= 0.0f)
        {
            f->zoom.distanceCur = 0.0f;
            f->zoom.mode = CAMERAFX_ZOOM_MODE_3;
            f->flags |= 0x2;
        }

        xMat4x3MoveLocalAt(m, f->zoom.distanceCur);

        break;
    }
    case CAMERAFX_ZOOM_MODE_3:
    {
        break;
    }
    }
}

void xCameraFXShake(float32 maxTime, float32 magnitude, float32 cycleMax, float32 rotate_magnitude, float32 radius, xVec3* epicenter, xVec3* player)
{
    cameraFX* f = xCameraFXAlloc();

    if (f)
    {
        f->type = CAMERAFX_TYPE_SHAKE;
        f->maxTime = maxTime;
        f->shake.magnitude = magnitude;
        f->shake.dir.x = 1.0f;
        f->shake.dir.y = 1.0f;
        f->shake.cycleMax = cycleMax;
        f->shake.cycleTime = 0.0f;
        f->shake.dampen = 0.0f;
        f->shake.dampenRate = 1.0f / maxTime;
        f->shake.rotate_magnitude = rotate_magnitude;
        f->shake.radius = radius;
        f->shake.epicenterP = epicenter;

        if (f->shake.epicenterP)
        {
            f->shake.epicenter = *f->shake.epicenterP;
        }

        f->shake.player = player;
    }
}

void xCameraFXShakeUpdate(cameraFX* f, float32 dt, const xMat4x3*, xMat4x3* m)
{
    float32 x, y, scale, noise;
    xVec3 var_4C, e;

    f->shake.cycleTime += dt;

    while (f->shake.cycleTime > f->shake.cycleMax)
    {
        f->shake.dir.x = -f->shake.dir.x;
        f->shake.dir.y = -f->shake.dir.y;
        f->shake.cycleTime -= f->shake.cycleMax;
    }

    scale = f->shake.dampenRate * (f->maxTime - f->elapsedTime);
    noise = 0.1f * (xurand() - 0.5f);

    if (f->shake.radius > 0.0f && f->shake.player)
    {
        xVec3Sub(&var_4C, f->shake.player, &f->shake.epicenter);

        float32 f1 = var_4C.length();

        if (f1 > f->shake.radius)
        {
            scale = 0.0f;
        }
        else
        {
            scale *= icos(f1 / f->shake.radius * PI * 0.5f);
        }
    }

    x = (f->shake.magnitude + noise) * f->shake.dir.x * scale / f->shake.cycleMax *
        f->shake.cycleTime * isin(f->shake.cycleTime / f->shake.cycleMax * PI);

    noise = 0.1f * (xurand() - 0.5f);

    y = (f->shake.magnitude + noise) * f->shake.dir.y * scale / f->shake.cycleMax *
        f->shake.cycleTime * isin(f->shake.cycleTime / f->shake.cycleMax * PI);

    xMat4x3MoveLocalRight(m, x);
    xMat4x3MoveLocalUp(m, y);

    xMat3x3GetEuler(m, &e);

    e.z += f->shake.cycleTime / f->shake.cycleMax * (2.0/PI) * 0.1f * scale * f->shake.rotate_magnitude;

    xMat3x3Euler(m, &e);
}

void xCameraFXUpdate(xCamera* cam, float32 dt)
{
    int32 i;
    cameraFX* f;
    cameraFXTableEntry* t;

    for (i = 0; i < 10; i++)
    {
        f = &sCameraFX[i];

        if (f->flags & 0x1)
        {
            f->elapsedTime += dt;

            if ((f->maxTime > 0.0f && f->elapsedTime > f->maxTime) || f->flags & 0x2)
            {
                f->flags = 0;

                t = &sCameraFXTable[f->type];

                if (t->funcKill)
                {
                    t->funcKill(f);
                }
            }
            else
            {
                t = &sCameraFXTable[f->type];

                if (t->func)
                {
                    t->func(f, dt, &sCameraFXMatOld, &cam->mat);
                }
            }
        }
    }

    iCameraUpdatePos(cam->lo_cam, &cam->mat);
}

void xCameraFXEnd(xCamera* cam)
{
    xMat4x3Copy(&cam->mat, &sCameraFXMatOld);
    iCameraUpdatePos(cam->lo_cam, &sCameraFXMatOld);
}

void xCameraEnd(xCamera* cam, float32 seconds, bool32 update_scrn_fx)
{
    if (update_scrn_fx)
    {
        xScrFxUpdate(cam->lo_cam, seconds);
    }

    iCameraEnd(cam->lo_cam);
}

void xCameraShowRaster(xCamera* cam)
{
    iCameraShowRaster(cam->lo_cam);
}

void xCameraSetScene(xCamera* cam, xScene* sc)
{
    cam->sc = sc;

    iCameraAssignEnv(cam->lo_cam, sc->env->geom);
}

void xCameraSetTargetMatrix(xCamera* cam, xMat4x3* mat)
{
    cam->tgt_mat = mat;
}

void xCameraSetTargetOMatrix(xCamera* cam, xMat4x3* mat)
{
    cam->tgt_omat = mat;
}

void xCameraDoCollisions(int32 do_collis, int32 owner)
{
    xcam_collis_owner_disable &= ~(1 << owner);
    xcam_collis_owner_disable |= !do_collis << owner;

    xcam_do_collis = (xcam_collis_owner_disable == 0);
}

void xCameraMove(xCamera* cam, uint32 flags, float32 dgoal, float32 hgoal, float32 pgoal,
    float32 tm, float32 tm_acc, float32 tm_dec)
{
    cam->flags = (cam->flags & ~0x3E) | (flags & 0x3E);
    cam->dgoal = dgoal;
    cam->hgoal = hgoal;
    cam->pgoal = pgoal;

    if (tm <= 0.0f)
    {
        if (cam->tgt_mat)
        {
            cam->dcur = dgoal;
            cam->hcur = hgoal;
            cam->pcur = pgoal;

            xCam_cyltoworld(&cam->mat.pos, cam->tgt_mat, dgoal, hgoal, pgoal, cam->flags);

            cam->omat.pos = cam->mat.pos;
            cam->yaw_cur = cam->yaw_goal = cam->pcur + ((cam->pcur >= PI) ? -PI : PI);
        }
    }
    else
    {
        cam->flags |= 0x1;
        cam->tm_acc = tm - tm_acc;
        cam->tm_dec = tm_dec;
        cam->tmr = tm;

        float32 s = 1.0f / (tm - 0.5f * (tm_acc - tm_dec));

        cam->depv = s * (dgoal - cam->dcur);
        cam->hepv = s * (hgoal - cam->hcur);
        cam->pepv = xDangleClamp(pgoal - cam->pcur) * s * 0.5f * (dgoal + cam->dcur);
    }
}

void xCameraMove(xCamera* cam, const xVec3& loc)
{
    cam->omat.pos = cam->mat.pos = loc;
    cam->flags &= ~0x3E;
    cam->tm_acc = cam->tm_dec = cam->tmr = 0.0f;
}

void xCameraMove(xCamera* cam, const xVec3& loc, float32 maxSpeed)
{
    xVec3 var_28;
    float32 f1;

    xVec3Sub(&var_28, &loc, &cam->mat.pos);

    f1 = xVec3Length(&var_28);

    if (f1 > maxSpeed)
    {
        xVec3SMul(&var_28, &var_28, maxSpeed / f1);
        xVec3Add(&cam->mat.pos, &cam->mat.pos, &var_28);
    }
    else
    {
        cam->mat.pos = loc;
    }

    cam->omat.pos = cam->mat.pos;
    cam->flags &= ~0x3E;
    cam->tm_acc = cam->tm_dec = cam->tmr = 0.0f;
}

void xCameraFOV(xCamera* cam, float32 fov, float32 maxSpeed, float32 dt)
{
    float32 speed = maxSpeed * dt;
    float32 currentFOV = xCameraGetFOV(cam);

    if (currentFOV != fov)
    {
        if (speed != 0.0f)
        {
            float32 len = fov - currentFOV;

            if (iabs(len) > speed)
            {
                len *= speed / len;
                xCameraSetFOV(cam, currentFOV + len);
            }
            else
            {
                xCameraSetFOV(cam, fov);
            }
        }
        else
        {
            xCameraSetFOV(cam, fov);
        }
    }
}

void xCameraLook(xCamera* cam, uint32 flags, const xQuat* orn_goal, float32 tm, float32 tm_acc, float32 tm_dec)
{
    cam->flags = (cam->flags & ~0xF80) | (flags & 0xF80);
    cam->orn_goal = *orn_goal;

    if (tm <= 0.0f)
    {
        if (cam->tgt_mat)
        {
            xQuatToMat(orn_goal, &cam->mat);

            *(xMat3x3*)&cam->omat = *(xMat3x3*)&cam->mat;
        }
    }
    else
    {
        cam->flags |= 0x40;
        cam->ltm_acc = tm - tm_acc;
        cam->ltm_dec = tm_dec;
        cam->ltmr = tm;

        xQuatDiff(&cam->orn_diff, &cam->orn_cur, orn_goal);

        float32 s = 1.0f / (tm - 0.5f * (tm_acc - tm_dec));

        cam->orn_epv = s * xQuatGetAngle(&cam->orn_diff);
    }
}

void xCameraLookYPR(xCamera* cam, uint32 flags, float32 yaw, float32 pitch, float32 roll, float32 tm, float32 tm_acc, float32 tm_dec)
{
    cam->flags = (cam->flags & ~0xF80) | (flags & 0xF80) | 0x80;
    cam->yaw_goal = yaw;
    cam->pitch_goal = pitch;
    cam->roll_goal = roll;

    if (tm <= 0.0f)
    {
        if (cam->tgt_mat)
        {
            cam->yaw_cur = yaw;
            cam->pitch_cur = pitch;
            cam->roll_cur = roll;

            xMat3x3Euler(&cam->mat, yaw, pitch, roll);

            *(xMat3x3*)&cam->omat = *(xMat3x3*)&cam->mat;
        }
    }
    else
    {
        cam->flags |= 0x40;
        cam->ltm_acc = tm - tm_acc;
        cam->ltm_dec = tm_dec;
        cam->ltmr = tm;

        float32 s = 1.0f / (tm - 0.5f * (tm_acc - tm_dec));

        cam->yaw_epv = s * xDangleClamp(yaw - cam->yaw_cur);
        cam->pitch_epv = s * xDangleClamp(pitch - cam->pitch_cur);
        cam->roll_epv = s * xDangleClamp(roll - cam->roll_cur);
    }
}

void xCameraRotate(xCamera* cam, const xMat3x3& m, float32 time, float32 accel, float32 decl)
{
    xVec3 eu;

    cam->flags = cam->flags & ~0xF80 | 0x80;

    xMat3x3GetEuler(&m, &eu);

    cam->yaw_goal = eu.x;
    cam->pitch_goal = eu.y;
    cam->roll_goal = eu.z;

    if (0.0f == time)
    {
        cam->yaw_cur = eu.x;
        cam->pitch_cur = eu.y;
        cam->roll_cur = eu.z;
    }

    *(xMat3x3*)&cam->mat = m;

    if (0.0f == time)
    {
        *(xMat3x3*)&cam->omat = m;
    }

    if (0.0f == time)
    {
        cam->ltm_acc = cam->ltm_dec = cam->ltmr = 0.0f;
    }
    else
    {
        cam->ltm_acc = accel;
        cam->ltm_dec = decl;
        cam->ltmr = time;
    }

    cam->yaw_epv = cam->pitch_epv = cam->roll_epv = 0.0f;
}

void xCameraRotate(xCamera* cam, const xVec3& v, float32 roll, float32 time, float32 accel, float32 decl)
{
    cam->yaw_goal = xatan2(v.x, v.z);
    cam->pitch_goal = -xasin(v.y);
    cam->roll_goal = roll;

    if (0.0f == time)
    {
        cam->yaw_cur = cam->yaw_goal;
        cam->pitch_cur = cam->pitch_goal;
        cam->roll_cur = cam->roll_goal;
    }

    cam->flags = cam->flags & ~0xF80 | 0x80;

    xMat3x3Euler(&cam->mat, cam->yaw_goal, cam->pitch_goal, cam->roll_goal);

    if (0.0f == time)
    {
        *(xMat3x3*)&cam->omat = *(xMat3x3*)&cam->mat;
    }

    if (0.0f == time)
    {
        cam->ltm_acc = cam->ltm_dec = cam->ltmr = 0.0f;
    }
    else
    {
        cam->ltm_acc = accel;
        cam->ltm_dec = decl;
        cam->ltmr = time;
    }

    cam->yaw_epv = cam->pitch_epv = cam->roll_epv = 0.0f;
}