#include "xScene.h"

#include "xAnim.h"
#include "xModel.h"
#include "xCollide.h"
#include "xCollideFast.h"

xScene* g_xSceneCur = NULL;

void xSceneInit(xScene* sc, uint16 num_trigs, uint16 num_stats, uint16 num_dyns, uint16 num_npcs)
{
    sc->flags = 0;

    sc->num_trigs_allocd = num_trigs;
    sc->trigs = (xEnt**)xMEMALLOC(num_trigs * sizeof(xEnt*));

    sc->num_stats_allocd = num_stats;
    sc->stats = (xEnt**)xMEMALLOC(num_stats * sizeof(xEnt*));

    sc->num_dyns_allocd = num_dyns;
    sc->dyns = (xEnt**)xMEMALLOC(num_dyns * sizeof(xEnt*));

    sc->num_npcs_allocd = num_npcs;
    sc->npcs = (xEnt**)xMEMALLOC(num_npcs * sizeof(xEnt*));

    sc->num_ents_allocd = (uint32)num_trigs + (uint32)num_stats + (uint32)num_dyns + (uint32)num_npcs;
    
    sc->num_act_ents = 0;
    sc->act_ents = (xEnt**)xMEMALLOC(sc->num_ents_allocd * sizeof(xEnt*));
    
    sc->num_nact_ents = 0;
    sc->nact_ents = (xEnt**)xMEMALLOC(sc->num_ents_allocd * sizeof(xEnt*));
    
    sc->num_ents = 0;
    sc->num_trigs = 0;
    sc->num_stats = 0;
    sc->num_dyns = 0;
    sc->num_npcs = 0;

    sc->resolvID = NULL;

    xAnimInit();
    xModelInit();
    xAnimPoolInit(&sc->mempool, 50, 1, 0x1, 4);
    xModelPoolInit(49, 64);
    xModelPoolInit(74, 8);
    xModelPoolInit(164, 1);
    xAnimTempTransitionInit(16);
    xCollideInit(sc);
    xCollideFastInit(sc);
}

void xSceneExit(xScene* sc)
{
}

void xSceneSave(xScene* sc, xSerial* s)
{
}

void xSceneLoad(xScene* sc, xSerial* s)
{
}

void xSceneSetup(xScene* sc)
{
    xEnvSetup(sc->env);
}

void xSceneAddEnt(xScene* sc, xEnt* ent)
{
    if (ent->collType == XENT_COLLTYPE_TRIG)
    {
        sc->trigs[sc->num_trigs++] = ent;
    }
    else if (ent->collType == XENT_COLLTYPE_STAT)
    {
        sc->stats[sc->num_stats++] = ent;
    }
    else if (ent->collType == XENT_COLLTYPE_DYN)
    {
        sc->dyns[sc->num_dyns++] = ent;
    }
    else if (ent->collType == XENT_COLLTYPE_NPC)
    {
        sc->npcs[sc->num_npcs++] = ent;
    }

    sc->act_ents[sc->num_act_ents++] = ent;
}

xBase* xSceneResolvID(xScene* sc, uint32 id)
{
    if (sc->resolvID)
    {
        return sc->resolvID(id);
    }

    return NULL;
}

const char* xSceneID2Name(xScene* sc, uint32 id)
{
    if (sc->id2Name)
    {
        return sc->id2Name(id);
    }

    return NULL;
}

void xSceneForAllEnts(xScene* sc, xSceneEntCallback func, void* data)
{
    for (uint16 i = 0; i < sc->num_act_ents; i++)
    {
        if (!func(sc->act_ents[i], sc, data))
        {
            break;
        }
    }
}

void xSceneForAllStatics(xScene* sc, xSceneEntCallback func, void* data)
{
    for (uint16 i = 0; i < sc->num_stats; i++)
    {
        if (!func(sc->stats[i], sc, data))
        {
            break;
        }
    }
}

void xSceneForAllDynamics(xScene* sc, xSceneEntCallback func, void* data)
{
    for (uint16 i = 0; i < sc->num_dyns; i++)
    {
        if (!func(sc->dyns[i], sc, data))
        {
            break;
        }
    }
}

void xSceneForAllNPCs(xScene* sc, xSceneEntCallback func, void* data)
{
    for (uint16 i = 0; i < sc->num_npcs; i++)
    {
        if (!func(sc->npcs[i], sc, data))
        {
            break;
        }
    }
}