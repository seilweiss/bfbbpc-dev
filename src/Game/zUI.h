#ifndef ZUI_H
#define ZUI_H

#include "zEnt.h"
#include "xIni.h"
#include "xMgr.h"

struct zScene;

struct zUIAsset : xEntAsset
{
	uint32 uiFlags;
	uint16 dim[2];
	uint32 textureID;
	float32 uva[2];
	float32 uvb[2];
	float32 uvc[2];
	float32 uvd[2];
};

typedef struct _zUI : zEnt
{
	zUIAsset* sasset;
	uint32 uiFlags;
	uint32 uiButton;
	uint16 preUpdateIndex;
	uint16 updateIndex;
} zUI;

class zUIMgr : public xMgr
{
public:
	void Setup(zScene* s);

private:
	int32 m_preUpdateStart;
	int32 m_preUpdateEnd;
	uint32 m_preUpdateMax;
	zUI** m_preUpdate;
	int32 m_updateStart;
	int32 m_updateEnd;
	uint32 m_updateMax;
	zUI** m_update;
};

extern zUIMgr gUIMgr;

void zUI_Init();
void zUI_Init(void* ent, void* asset);
void zUI_ParseINI(xIniFile* ini);
void zUI_ScenePortalInit(zScene* zsc);

#endif