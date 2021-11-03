#include "xEnt.h"

#include "iMath3.h"
#include "xMemMgr.h"

static xBox all_ents_box;
static bool32 all_ents_box_init = FALSE;

STUB void xEntSceneInit()
{

}

void xEntSceneExit()
{
}

UNCHECKED void xEntInit(xEnt* ent, xEntAsset* asset)
{
    xBaseInit(ent, asset);

    ent->asset = asset;
    ent->update = xEntUpdate;
    ent->bupdate = xEntDefaultBoundUpdate;
    ent->render = xEntRender;
    ent->move = NULL;
    ent->transl = xEntDefaultTranslate;
    ent->flags = asset->flags;
    ent->miscflags = 0;
    ent->moreFlags = asset->moreFlags;
    ent->subType = asset->subtype;
    ent->pflags = asset->pflags;
    ent->ffx = NULL;
    ent->num_ffx = 0;
    ent->driver = NULL;
    ent->model = NULL;
    ent->collModel = NULL;
    ent->camcollModel = NULL;
    ent->frame = NULL;
    ent->collis = NULL;
    ent->lightKit = NULL;
    ent->simpShadow = NULL;
    ent->entShadow = NULL;
    ent->baseFlags |= XBASE_IS_ENT;

    xGridBoundInit(&ent->gridb, ent);

    ent->anim_coll = NULL;

    if (all_ents_box_init)
    {
        iBoxInitBoundVec(&all_ents_box, &asset->pos);
        all_ents_box_init = FALSE;
    }
    else
    {
        iBoxBoundVec(&all_ents_box, &all_ents_box, &asset->pos);
    }
}

UNCHECKED void xEntInitForType(xEnt* ent)
{
    ent->update = xEntUpdate;
    ent->render = xEntRender;

    if (ent->collType == XENT_COLLTYPE_TRIG)
    {
        ent->pflags &= ~0x3;
        ent->chkby = XENT_COLLTYPE_PLYR | XENT_COLLTYPE_NPC;
        ent->penby = 0;
    }
    else if (ent->collType == XENT_COLLTYPE_STAT)
    {
        ent->pflags &= ~0x3;
        ent->chkby = XENT_COLLTYPE_PLYR | XENT_COLLTYPE_NPC;
        ent->penby = XENT_COLLTYPE_PLYR | XENT_COLLTYPE_NPC;
    }
    else if (ent->collType == XENT_COLLTYPE_DYN)
    {
        ent->pflags |= 0x1;
        ent->move = NULL;
        ent->frame = (xEntFrame*)xMEMALLOC(sizeof(xEntFrame));

        memset(ent->frame, 0, sizeof(xEntFrame));

        ent->pflags &= ~0x2;
        ent->chkby = XENT_COLLTYPE_PLYR | XENT_COLLTYPE_NPC;
        ent->penby = XENT_COLLTYPE_PLYR | XENT_COLLTYPE_NPC;
    }
    else if (ent->collType == XENT_COLLTYPE_NPC)
    {
        ent->pflags |= 0x1;
        ent->move = NULL;
        ent->pflags |= 0x2;
        ent->chkby = XENT_COLLTYPE_PLYR;
        ent->penby = XENT_COLLTYPE_PLYR;
    }
    else if (ent->collType == XENT_COLLTYPE_PLYR)
    {
        ent->pflags |= 0x1;
        ent->move = NULL;
        ent->frame = (xEntFrame*)xMEMALLOC(sizeof(xEntFrame));

        memset(ent->frame, 0, sizeof(xEntFrame));

        ent->pflags |= 0x2;
        ent->chkby = 0;
        ent->penby = 0;
        ent->collis = (xEntCollis*)xMEMALLOC(sizeof(xEntCollis));
        ent->collis->chk = 0x2F;
        ent->collis->pen = 0x2E;
        ent->collis->post = 0;
        ent->collis->depenq = 0;
    }

    if (((ent->moreFlags & 0x20) || (ent->flags & 0x2)) && !ent->frame)
    {
        ent->frame = (xEntFrame*)xMEMALLOC(sizeof(xEntFrame));

        memset(ent->frame, 0, sizeof(xEntFrame));
    }

    ent->baseFlags |= XBASE_IS_ENT;
}

UNCHECKED xModelInstance* xEntLoadModel(xEnt* ent, RpAtomic* imodel)
{
    xModelInstance* model = xModelInstanceAlloc(imodel, ent, 0, 0, NULL);

    while (imodel = iModelFile_RWMultiAtomic(imodel))
    {
        xModelInstanceAttach(xModelInstanceAlloc(imodel, ent, 0x8, 0, NULL), model);
    }

    if (ent)
    {
        ent->model = model;
    }

    return model;
}

STUB void xEntRender(xEnt* ent)
{

}

STUB void xEntUpdate(xEnt* ent, xScene* sc, float32 dt)
{

}

STUB void xEntDefaultBoundUpdate(xEnt* ent, xVec3* pos)
{

}

STUB void xEntDefaultTranslate(xEnt* ent, xVec3* dpos, xMat4x3* dmat)
{

}

STUB xBox* xEntGetAllEntsBox()
{
    return NULL;
}

STUB void xEntInitShadow(xEnt& ent, xEntShadow& shadow)
{

}