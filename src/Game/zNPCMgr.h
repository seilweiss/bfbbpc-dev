#ifndef ZNPCMGR_H
#define ZNPCMGR_H

#include "xEnt.h"

void zNPCMgr_Startup();
void zNPCMgr_Shutdown();
void zNPCMgr_scenePrepare(int32 npccnt);
void zNPCMgr_sceneFinish();
void zNPCMgr_scenePostInit();
void zNPCMgr_scenePostSetup();
xEnt* zNPCMgr_createNPCInst(int32, xEntAsset* assdat);

#endif