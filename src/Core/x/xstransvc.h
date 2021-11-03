#ifndef XSTRANSVC_H
#define XSTRANSVC_H

#include "xpkrsvc.h"

typedef struct st_STRAN_SCENE
{
	uint32 scnid;
	int32 lockid;
	PACKER_READ_DATA* spkg;
	bool32 isHOP;
	void* userdata;
	char fnam[256];
} STRAN_SCENE;

typedef struct st_STRAN_DATA
{
	STRAN_SCENE hipscn[16];
	uint32 loadlock;
} STRAN_DATA;

#define XST_HIP 0x1
#define XST_HOP 0x2
#define XST_HIPHOP (XST_HIP | XST_HOP)

typedef int32(*xSTSwitchSceneProgressMonitor)(void* userdata, float32 progress);

int32 xSTStartup(PACKER_ASSETTYPE* handlers);
int32 xSTShutdown();
int32 xSTPreLoadScene(uint32 sid, void* userdata, int32 flg_hiphop);
bool32 xSTQueueSceneAssets(uint32 sid, int32 flg_hiphop);
void xSTUnLoadScene(uint32 sid, int32 flg_hiphop);
float32 xSTLoadStep(uint32 sid);
void xSTDisconnect(uint32 sid, int32 flg_hiphop);
bool32 xSTSwitchScene(uint32 sid, void* userdata, xSTSwitchSceneProgressMonitor progmon);
const char* xSTAssetName(uint32 aid);
const char* xSTAssetName(void* raw_HIP_asset);
void* xSTFindAsset(uint32 aid, uint32* size = NULL);
int32 xSTAssetCountByType(uint32 type);
void* xSTFindAssetByType(uint32 type, int32 idx = 0, uint32* size = NULL);
bool32 xSTGetAssetInfo(uint32 aid, PKR_ASSET_TOCINFO* tocainfo);
bool32 xSTGetAssetInfoByType(uint32 type, int32 idx, PKR_ASSET_TOCINFO* ainfo);
bool32 xSTGetAssetInfoInHxP(uint32 aid, PKR_ASSET_TOCINFO* tocinfo, uint32 hiphash);
const char* xST_xAssetID_HIPFullPath(uint32 aid);
const char* xST_xAssetID_HIPFullPath(uint32 aid, uint32* sceneID);

#endif