#include "zEnv.h"

#include "zMain.h"
#include "xstransvc.h"
#include "xDebug.h"
#include "zSurface.h"
#include "zLight.h"

void zEnvInit(void* env, void* easset)
{
    zEnvInit((zEnv*)env, (xEnvAsset*)easset);
}

UNCHECKED void zEnvInit(zEnv* env, xEnvAsset* easset)
{
    uint32 bufsize;
    void* buf;

    xBaseInit(env, easset);

    env->easset = easset;
    env->eventFunc = zEnvEventCB;

    if (env->linkCount)
    {
        env->link = (xLinkAsset*)(env->easset + 1);
    }

    globals.sceneCur->zen = env;
    globals.sceneCur->zen = env;

    XPRINTF("BSP Count = %d\n", xSTAssetCountByType('BSP '));

    buf = xSTFindAsset(easset->bspAssetID, &bufsize);

    if (buf)
    {
        xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, XENV_TYPE_BSP);
    }

    zSurfaceRegisterMapper(easset->bspMapperID);

    if (easset->bspCollisionAssetID != 0)
    {
        buf = xSTFindAsset(easset->bspCollisionAssetID, &bufsize);

        if (buf)
        {
            xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, XENV_TYPE_COLLISION);
        }
    }

    if (easset->bspFXAssetID != 0)
    {
        buf = xSTFindAsset(easset->bspFXAssetID, &bufsize);

        if (buf)
        {
            xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, XENV_TYPE_FX);
        }
    }

    if (easset->bspCameraAssetID != 0)
    {
        buf = xSTFindAsset(easset->bspCameraAssetID, &bufsize);

        if (buf)
        {
            xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, XENV_TYPE_CAMERA);
        }
    }

    if (globals.sceneCur->env && globals.sceneCur->env->geom)
    {
        zLightResetAll(globals.sceneCur->env);
        iLightInit(globals.sceneCur->env->geom->world);
    }
}

UNCHECKED void zEnvSetup(zEnv* env)
{
    xBaseSetup(env);
    iEnvLightingBasics(globals.sceneCur->env->geom, env->easset);
}

UNCHECKED void zEnvStartingCamera(zEnv* env)
{
    xCamera* cam = (xCamera*)zSceneFindObject(env->easset->startCameraAssetID);

    if (cam)
    {
        zEntPlayer_StoreCheckPoint(&globals.player.ent.frame->mat.pos, globals.player.ent.frame->rot.angle, globals.camera.id);
    }
}

STUB bool32 zEnvEventCB(xBase*, xBase* to, uint32 toEvent, const float32* toParam, xBase*)
{
    return TRUE;
}