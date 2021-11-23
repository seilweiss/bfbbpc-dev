#include "iModel.h"

#include "xModel.h"
#include "iCamera.h"
#include "iSystem.h"

#include <rphanim.h>
#include <rpskin.h>
#include <rpusrdat.h>
#include <rpmatfx.h>

#define IMODEL_GAMECUBE

#ifdef IMODEL_GAMECUBE
RpWorld* instance_world;
RwCamera* instance_camera;
#endif

static uint32 gLastAtomicCount = 0;
static RpAtomic* gLastAtomicList[256];
static RpLight* sEmptyDirectionalLight[4];
static RpLight* sEmptyAmbientLight;

UNCHECKED static RwFrame* GetChildFrameHierarchy(RwFrame* frame, void* data)
{
    RpHAnimHierarchy* hierarchy = RpHAnimFrameGetHierarchy(frame);

    if (!hierarchy)
    {
        RwFrameForAllChildren(frame, GetChildFrameHierarchy, data);
    }
    else
    {
        *(RpHAnimHierarchy**)data = hierarchy;

        return NULL;
    }

    return frame;
}

UNCHECKED static RpHAnimHierarchy* GetHierarchy(RpAtomic* model)
{
    RpHAnimHierarchy* hierarchy = NULL;

    GetChildFrameHierarchy(RpAtomicGetFrame(model), &hierarchy);

    return hierarchy;
}

UNCHECKED void iModelInit()
{
    RwFrame* frame;
    RwRGBAReal black = { 0, 0, 0, 0 };
    int32 i;

    if (!sEmptyDirectionalLight[0])
    {
        for (i = 0; i < 4; i++)
        {
            sEmptyDirectionalLight[i] = RpLightCreate(rpLIGHTDIRECTIONAL);

            RpLightSetColor(sEmptyDirectionalLight[i], &black);

            frame = RwFrameCreate();

            RpLightSetFrame(sEmptyDirectionalLight[i], frame);
        }

        sEmptyAmbientLight = RpLightCreate(rpLIGHTAMBIENT);

        RpLightSetColor(sEmptyAmbientLight, &black);
    }
}

#ifdef IMODEL_GAMECUBE
UNCHECKED static RpAtomic* FindAndInstanceAtomicCallback(RpAtomic* atomic, void* data)
{
    RpHAnimHierarchy* hierarchy = GetHierarchy(atomic);
    RpGeometry* geometry = RpAtomicGetGeometry(atomic);
    RpSkin* skin = RpSkinGeometryGetSkin(geometry);
    int32 i;
    RpMaterial* material;

    if (skin && !hierarchy)
    {
        hierarchy = RpHAnimHierarchyCreate(RpSkinGetNumBones(skin), NULL, NULL, rpHANIMHIERARCHYLOCALSPACEMATRICES, 36);

        RpHAnimFrameSetHierarchy(RpAtomicGetFrame(atomic), hierarchy);
    }

    if (hierarchy && skin)
    {
        RpSkinAtomicSetHAnimHierarchy(atomic, hierarchy);
    }

    if (hierarchy)
    {
        RpHAnimHierarchySetFlags(hierarchy, rpHANIMHIERARCHYLOCALSPACEMATRICES);
    }

    if (gLastAtomicCount < 256)
    {
        gLastAtomicList[gLastAtomicCount++] = atomic;
    }

    RwFrameGetRoot(RpAtomicGetFrame(atomic));

    for (i = 0; i < geometry->matList.numMaterials; i++)
    {
        material = rpMaterialListGetMaterial(&geometry->matList, i);

        if (material && RpMatFXMaterialGetEffects(material) != rpMATFXEFFECTNULL)
        {
            RpMatFXAtomicEnableEffects(atomic);

#ifdef _WIN32
            RpAtomicSetPipeline(atomic, RpMatFXGetD3D9Pipeline(rpMATFXD3D9ATOMICPIPELINE));
#endif

            if (RpSkinGeometryGetSkin(geometry))
            {
                RpSkinAtomicSetType(atomic, rpSKINTYPEMATFX);
            }

            break;
        }
    }

    if (gLastAtomicCount < 256)
    {
        gLastAtomicList[gLastAtomicCount++] = atomic;
    }

    return atomic;
}
#endif

UNCHECKED static RpAtomic* iModelStreamRead(RwStream* stream)
{
#ifdef IMODEL_GAMECUBE
    static uint32 num_models = 0;
#endif

    RpClump* clump;
    uint32 i;
    uint32 maxIndex;
    float32 maxRadius;
    float32 testRadius;

    if (!stream)
    {
        return NULL;
    }

    if (!RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL))
    {
        RwStreamClose(stream, NULL);
        return NULL;
    }

    clump = RpClumpStreamRead(stream);

    RwStreamClose(stream, NULL);

    if (!clump)
    {
        return NULL;
    }

#ifdef IMODEL_GAMECUBE
    RwBBox bbox = { 1000.0f, 1000.0f, 1000.0f, -1000.0f, -1000.0f, -1000.0f };

    instance_world = RpWorldCreate(&bbox);
    instance_camera = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, FALSE);

    RpWorldAddCamera(instance_world, instance_camera);

    gLastAtomicCount = 0;

    RpClumpForAllAtomics(clump, FindAndInstanceAtomicCallback, NULL);

    RpWorldRemoveCamera(instance_world, instance_camera);
    iCameraDestroy(instance_camera);
    RpWorldDestroy(instance_world);
#endif

    if (gLastAtomicCount > 1)
    {
        maxRadius = -1.0f;
        maxIndex = 0;

        for (i = 0; i < gLastAtomicCount; i++)
        {
            testRadius = gLastAtomicList[i]->boundingSphere.radius;

            if (testRadius > maxRadius)
            {
                maxRadius = testRadius;
                maxIndex = i;
            }
        }

        for (i = 0; i < gLastAtomicCount; i++)
        {
            if (i != maxIndex)
            {
                testRadius = gLastAtomicList[i]->boundingSphere.radius +
                    xVec3Dist((xVec3*)&gLastAtomicList[i]->boundingSphere.center,
                              (xVec3*)&gLastAtomicList[maxIndex]->boundingSphere.center);

                if (testRadius > maxRadius)
                {
                    maxRadius = testRadius;
                }
            }
        }

        maxRadius *= 1.05f;

        for (i = 0; i < gLastAtomicCount; i++)
        {
            if (i != maxIndex)
            {
                gLastAtomicList[i]->boundingSphere.center = gLastAtomicList[maxIndex]->boundingSphere.center;
            }

            gLastAtomicList[i]->boundingSphere.radius = maxRadius;
            gLastAtomicList[i]->interpolator.flags &= ~rpINTERPOLATORDIRTYSPHERE;
        }
    }

    return gLastAtomicList[0];
}

UNCHECKED RpAtomic* iModelFileNew(void* buffer, uint32 size)
{
    RwMemory rwmem;
    rwmem.start = (RwUInt8*)buffer;
    rwmem.length = size;

    return iModelStreamRead(RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &rwmem));
}

UNCHECKED static RpAtomic* NextAtomicCallback(RpAtomic* atomic, void* data)
{
    RpAtomic** nextModel = (RpAtomic**)data;

    if (*nextModel == atomic)
    {
        *nextModel = NULL;
    }
    else if (!*nextModel)
    {
        *nextModel = atomic;
    }

    return atomic;
}

UNCHECKED RpAtomic* iModelFile_RWMultiAtomic(RpAtomic* model)
{
    RpClump* clump;
    RpAtomic* nextModel;

    if (!model)
    {
        return NULL;
    }

    clump = RpAtomicGetClump(model);
    nextModel = model;

    RpClumpForAllAtomics(clump, NextAtomicCallback, &nextModel);

    return nextModel;
}

UNCHECKED uint32 iModelNumBones(RpAtomic* model)
{
    RpHAnimHierarchy* hierarchy = GetHierarchy(model);

    if (hierarchy)
    {
        return hierarchy->numNodes;
    }

    return 0;
}

UNCHECKED static uint32 iModelTagUserData(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z, int32 closeV)
{
    int32 i;
    int32 count;
    RpUserDataArray* array;
    RpUserDataArray* testarray;
    float32 distSqr;
    float32 closeDistSqr;
    int32 numTags;
    int32 t;
    xModelTag* tagList;

    count = RpGeometryGetUserDataArrayCount(model->geometry);
    array = NULL;

    for (i = 0; i < count; i++)
    {
        testarray = RpGeometryGetUserDataArray(model->geometry, i);

        if (strcmp(testarray->name, "HI_Tags") == 0)
        {
            array = testarray;
            break;
        }
    }

    if (!array)
    {
        memset(tag, 0, sizeof(xModelTag));
        return 0;
    }

    closeDistSqr = 1000000000.0f;
    numTags = *(int32*)array->data;
    tagList = (xModelTag*)((int32*)array->data + 1);

    if (closeV < 0 || closeV > numTags)
    {
        closeV = 0;

        for (t = 0; t < numTags; t++)
        {
            distSqr = SQR(tagList[t].v.x - x) + SQR(tagList[t].v.y - y) + SQR(tagList[t].v.z - z);

            if (distSqr < closeDistSqr)
            {
                closeDistSqr = distSqr;
                closeV = t;
            }
        }

        if (tag)
        {
            *tag = tagList[closeV];
        }
    }
    else
    {
        if (tag)
        {
            *tag = tagList[closeV];
        }
    }

    return closeV;
}

UNCHECKED static uint32 iModelTagInternal(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z, int32 closeV)
{
    RpGeometry* geom;
    RwV3d* vert;
    int32 v;
    int32 numV;
    float32 distSqr;
    float32 closeDistSqr;
    RpSkin* skin;
    const RwMatrixWeights* wt;

    geom = model->geometry;
    vert = geom->morphTarget->verts;

    if (!vert)
    {
        return iModelTagUserData(tag, model, x, y, z, closeV);
    }

    numV = geom->numVertices;
    closeDistSqr = 1000000000.0f;

    if (closeV < 0 || closeV > numV)
    {
        closeV = 0;

        for (v = 0; v < numV; v++)
        {
            distSqr = SQR(vert[v].x - x) + SQR(vert[v].y - y) + SQR(vert[v].z - z);

            if (distSqr < closeDistSqr)
            {
                closeDistSqr = distSqr;
                closeV = v;
            }
        }

        if (tag)
        {
            tag->v.x = x;
            tag->v.y = y;
            tag->v.z = z;
        }
    }
    else
    {
        if (tag)
        {
            tag->v.x = vert[closeV].x;
            tag->v.y = vert[closeV].y;
            tag->v.z = vert[closeV].z;
        }
    }

    if (tag)
    {
        skin = RpSkinGeometryGetSkin(model->geometry);

        if (skin)
        {
            wt = &RpSkinGetVertexBoneWeights(skin)[closeV];

            tag->matidx = RpSkinGetVertexBoneIndices(skin)[closeV];
            tag->wt[0] = wt->w0;
            tag->wt[1] = wt->w1;
            tag->wt[2] = wt->w2;
            tag->wt[3] = wt->w3;
        }
    }

    return closeV;
}

uint32 iModelTagSetup(xModelTag* tag, RpAtomic* model, float32 x, float32 y, float32 z)
{
    return iModelTagInternal(tag, model, x, y, z, -1);
}