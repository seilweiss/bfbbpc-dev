#ifndef XSCENE_H
#define XSCENE_H

#include "xEnt.h"
#include "xEnv.h"
#include "xMemMgr.h"

struct xScene;

typedef xBase* (*xSceneResolveIDCallback)(uint32);
typedef const char* (*xSceneBaseToNameCallback)(xBase*);
typedef const char* (*xSceneIDToNameCallback)(uint32);
typedef xEnt* (*xSceneEntCallback)(xEnt* ent, xScene* sc, void* data);

struct xScene
{
	uint32 sceneID;
	uint16 flags;
	uint16 num_ents;
	uint16 num_trigs;
	uint16 num_stats;
	uint16 num_dyns;
	uint16 num_npcs;
	uint16 num_act_ents;
	uint16 num_nact_ents;
	float32 gravity;
	float32 drag;
	float32 friction;
	uint16 num_ents_allocd;
	uint16 num_trigs_allocd;
	uint16 num_stats_allocd;
	uint16 num_dyns_allocd;
	uint16 num_npcs_allocd;
	xEnt** trigs;
	xEnt** stats;
	xEnt** dyns;
	xEnt** npcs;
	xEnt** act_ents;
	xEnt** nact_ents;
	xEnv* env;
	xMemPool mempool;
	xSceneResolveIDCallback resolvID;
	xSceneBaseToNameCallback base2Name;
	xSceneIDToNameCallback id2Name;
};

extern xScene* g_xSceneCur;

void xSceneInit(xScene* sc, uint16 num_trigs, uint16 num_stats, uint16 num_dyns, uint16 num_npcs);
void xSceneExit(xScene* sc);
void xSceneSave(xScene* sc, xSerial* s);
void xSceneLoad(xScene* sc, xSerial* s);
void xSceneSetup(xScene* sc);
void xSceneAddEnt(xScene* sc, xEnt* ent);
xBase* xSceneResolvID(xScene* sc, uint32 id);
const char* xSceneID2Name(xScene* sc, uint32 id);
void xSceneForAllEnts(xScene* sc, xSceneEntCallback func, void* data);
void xSceneForAllStatics(xScene* sc, xSceneEntCallback func, void* data);
void xSceneForAllDynamics(xScene* sc, xSceneEntCallback func, void* data);
void xSceneForAllNPCs(xScene* sc, xSceneEntCallback func, void* data);

#endif