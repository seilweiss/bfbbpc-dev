#include "xEnv.h"

xEnv* gCurXEnv = NULL;

UNCHECKED void xEnvLoadBsp(xEnv* env, const void* data, uint32 datasize, int32 dataType)
{
    if (dataType == XENV_TYPE_BSP)
    {
        env->geom = &env->ienv;
    }

    iEnvLoad(env->geom, data, datasize, dataType);

    gCurXEnv = env;
}

UNCHECKED void xEnvSetup(xEnv* env)
{
    iEnvDefaultLighting(env->geom);

    env->lightKit = NULL;

    gCurXEnv = env;
}