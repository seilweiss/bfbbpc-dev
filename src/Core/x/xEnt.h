#ifndef XENT_H
#define XENT_H

#include "xBase.h"
#include "xModel.h"
#include "xGrid.h"
#include "xBound.h"
#include "xShadowSimple.h"
#include "xCollide.h"

struct xEntAsset : xBaseAsset
{
	uint8 flags;
	uint8 subtype;
	uint8 pflags;
	uint8 moreFlags;
	uint8 pad;
	uint32 surfaceID;
	xVec3 ang;
	xVec3 pos;
	xVec3 scale;
	float32 redMult;
	float32 greenMult;
	float32 blueMult;
	float32 seeThru;
	float32 seeThruSpeed;
	uint32 modelInfoID;
	uint32 animListID;
};

struct xEntFrame
{
	xMat4x3 mat;
	xMat4x3 oldmat;
	xVec3 oldvel;
	xRot oldrot;
	xRot drot;
	xRot rot;
	xVec3 dpos;
	xVec3 dvel;
	xVec3 vel;
	uint32 mode;
};

struct xEntShadow
{
	xVec3 pos;
	xVec3 vec;
	RpAtomic* shadowModel;
	float32 dst_cast;
	float32 radius[2];
};

struct xEnt;
struct xScene;

typedef void(*xEntUpdateCallback)(xEnt* ent, xScene* sc, float32 dt);
typedef void(*xEntEndUpdateCallback)(xEnt*, xScene*, float32);
typedef void(*xEntBoundUpdateCallback)(xEnt* ent, xVec3* pos);
typedef void(*xEntMoveCallback)(xEnt*, xScene*, float32, xEntFrame*);
typedef void(*xEntRenderCallback)(xEnt* ent);
typedef void(*xEntTranslateCallback)(xEnt* ent, xVec3* dpos, xMat4x3* dmat);

struct xEntCollis;
struct xFFX;

struct xEnt : xBase
{
	struct anim_coll_data
	{
		uint32 flags;
		uint32 bones;
		xMat4x3 old_mat;
		xMat4x3 new_mat;
		uint32 verts_size;
		xVec3* verts;
		xVec3* normals;
	};

	xEntAsset* asset;
	uint16 idx;
	uint16 num_updates;
	uint8 flags;
	uint8 miscflags;
	uint8 subType;
	uint8 pflags;
	uint8 moreFlags;
	uint8 isCulled;
	uint8 driving_count;
	uint8 num_ffx;
	uint8 collType;
	uint8 collLev;
	uint8 chkby;
	uint8 penby;
	xModelInstance* model;
	xModelInstance* collModel;
	xModelInstance* camcollModel;
	xLightKit* lightKit;
	xEntUpdateCallback update;
	xEntEndUpdateCallback endUpdate;
	xEntBoundUpdateCallback bupdate;
	xEntMoveCallback move;
	xEntRenderCallback render;
	xEntFrame* frame;
	xEntCollis* collis;
	xGridBound gridb;
	xBound bound;
	xEntTranslateCallback transl;
	xFFX* ffx;
	xEnt* driver;
	int32 driveMode;
	xShadowSimpleCache* simpShadow;
	xEntShadow* entShadow;
	anim_coll_data* anim_coll;
	void* user_data;
};

// collision types
#define XENT_COLLTYPE_NONE 0x0
#define XENT_COLLTYPE_TRIG 0x1 // trigger (TRIG)
#define XENT_COLLTYPE_STAT 0x2 // static (SIMP)
#define XENT_COLLTYPE_DYN 0x4 // dynamic (PLAT)
#define XENT_COLLTYPE_NPC 0x8 // npc/enemy (VIL)
#define XENT_COLLTYPE_PLYR 0x10 // player (PLYR)

typedef void(*xEntCollisPostCallback)(xEnt*, xScene*, float32, xEntCollis*);
typedef uint32(*xEntCollisDepenqCallback)(xEnt*, xEnt*, xScene*, float32, xCollis*);

struct xEntCollis
{
	uint8 chk;
	uint8 pen;
	uint8 env_sidx;
	uint8 env_eidx;
	uint8 npc_sidx;
	uint8 npc_eidx;
	uint8 dyn_sidx;
	uint8 dyn_eidx;
	uint8 stat_sidx;
	uint8 stat_eidx;
	uint8 idx;
	xCollis colls[18];
	xEntCollisPostCallback post;
	xEntCollisDepenqCallback depenq;
};

#define xEnt_Update(ent, sc, dt) ((ent)->update((ent), (sc), (dt)))

void xEntSceneInit();
void xEntSceneExit();
void xEntInit(xEnt* ent, xEntAsset* asset);
void xEntInitForType(xEnt* ent);
xModelInstance* xEntLoadModel(xEnt* ent, RpAtomic* imodel);
void xEntRender(xEnt* ent);
void xEntUpdate(xEnt* ent, xScene* sc, float32 dt);
void xEntDefaultBoundUpdate(xEnt* ent, xVec3* pos);
void xEntDefaultTranslate(xEnt* ent, xVec3* dpos, xMat4x3* dmat);
xBox* xEntGetAllEntsBox();
void xEntInitShadow(xEnt& ent, xEntShadow& shadow);

inline void xEntEnable(xEnt* ent)
{
	xBaseEnable(ent);
}

inline void xEntShow(xEnt* ent)
{
	ent->flags |= 0x1;
}

UNCHECKED inline uint32 xEntIsVisible(const xEnt* ent)
{
	return (ent->flags & 0x81) == 0x1;
}

inline xMat4x3* xEntGetFrame(const xEnt* ent)
{
	return xModelGetFrame(ent->model);
}

#endif