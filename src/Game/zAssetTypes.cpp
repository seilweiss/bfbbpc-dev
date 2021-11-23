#include "zAssetTypes.h"

#include "xstransvc.h"
#include "xJSP.h"
#include "iModel.h"

static void* Model_Read(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);
static void* Curve_Read(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);
static void Model_Unload(void* userdata, uint32 assetid);
static void* BSP_Read(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);
static void BSP_Unload(void* userdata, uint32 assetid);
static void* JSP_Read(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);
static void JSP_Unload(void* userdata, uint32 assetid);
static void* RWTX_Read(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);
static void TextureRW3_Unload(void* userdata, uint32 assetid);
static void* ATBL_Read(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);
static void Anim_Unload(void* userdata, uint32 assetid);
static void LightKit_Unload(void* userdata, uint32 assetid);
static void MovePoint_Unload(void* userdata, uint32 assetid);
static void* SndInfoRead(void* userdata, uint32 assetid, void* indata, uint32 insize, uint32* outsize);

#define DEF_ASSETTYPE(typetag, read, unload) { typetag, 0, 0, read, 0, 0, 0, 0, unload, 0 }

static PACKER_ASSETTYPE assetTypeHandlers[] =
{
    DEF_ASSETTYPE('BSP ', BSP_Read, BSP_Unload),
    DEF_ASSETTYPE('JSP ', JSP_Read, JSP_Unload),
    DEF_ASSETTYPE('TXD ', NULL, NULL),
    DEF_ASSETTYPE('MODL', Model_Read, Model_Unload),
    DEF_ASSETTYPE('ANIM', NULL, Anim_Unload),
    DEF_ASSETTYPE('RWTX', RWTX_Read, TextureRW3_Unload),
    DEF_ASSETTYPE('LKIT', NULL, LightKit_Unload),
    DEF_ASSETTYPE('CAM ', NULL, NULL),
    DEF_ASSETTYPE('PLYR', NULL, NULL),
    DEF_ASSETTYPE('NPC ', NULL, NULL),
    DEF_ASSETTYPE('ITEM', NULL, NULL),
    DEF_ASSETTYPE('PKUP', NULL, NULL),
    DEF_ASSETTYPE('TRIG', NULL, NULL),
    DEF_ASSETTYPE('SDF ', NULL, NULL),
    DEF_ASSETTYPE('TEX ', NULL, NULL),
    DEF_ASSETTYPE('TXT ', NULL, NULL),
    DEF_ASSETTYPE('ENV ', NULL, NULL),
    DEF_ASSETTYPE('ATBL', ATBL_Read, NULL),
    DEF_ASSETTYPE('MINF', NULL, NULL),
    DEF_ASSETTYPE('PICK', NULL, NULL),
    DEF_ASSETTYPE('PLAT', NULL, NULL),
    DEF_ASSETTYPE('PEND', NULL, NULL),
    DEF_ASSETTYPE('MRKR', NULL, NULL),
    DEF_ASSETTYPE('MVPT', NULL, MovePoint_Unload),
    DEF_ASSETTYPE('TIMR', NULL, NULL),
    DEF_ASSETTYPE('CNTR', NULL, NULL),
    DEF_ASSETTYPE('PORT', NULL, NULL),
    DEF_ASSETTYPE('SND ', NULL, NULL),
    DEF_ASSETTYPE('SNDS', NULL, NULL),
    DEF_ASSETTYPE('GRUP', NULL, NULL),
    DEF_ASSETTYPE('MPHT', NULL, NULL),
    DEF_ASSETTYPE('SFX ', NULL, NULL),
    DEF_ASSETTYPE('SNDI', SndInfoRead, NULL),
    DEF_ASSETTYPE('HANG', NULL, NULL),
    DEF_ASSETTYPE('SIMP', NULL, NULL),
    DEF_ASSETTYPE('BUTN', NULL, NULL),
    DEF_ASSETTYPE('SURF', NULL, NULL),
    DEF_ASSETTYPE('DSTR', NULL, NULL),
    DEF_ASSETTYPE('BOUL', NULL, NULL),
    DEF_ASSETTYPE('MAPR', NULL, NULL),
    DEF_ASSETTYPE('GUST', NULL, NULL),
    DEF_ASSETTYPE('VOLU', NULL, NULL),
    DEF_ASSETTYPE('UI  ', NULL, NULL),
    DEF_ASSETTYPE('UIFT', NULL, NULL),
    DEF_ASSETTYPE('TEXT', NULL, NULL),
    DEF_ASSETTYPE('COND', NULL, NULL),
    DEF_ASSETTYPE('DPAT', NULL, NULL),
    DEF_ASSETTYPE('PRJT', NULL, NULL),
    DEF_ASSETTYPE('LOBM', NULL, NULL),
    DEF_ASSETTYPE('FOG ', NULL, NULL),
    DEF_ASSETTYPE('LITE', NULL, NULL),
    DEF_ASSETTYPE('PARE', NULL, NULL),
    DEF_ASSETTYPE('PARS', NULL, NULL),
    DEF_ASSETTYPE('CSN ', NULL, NULL),
    DEF_ASSETTYPE('CTOC', NULL, NULL),
    DEF_ASSETTYPE('CSNM', NULL, NULL),
    DEF_ASSETTYPE('EGEN', NULL, NULL),
    DEF_ASSETTYPE('ALST', NULL, NULL),
    DEF_ASSETTYPE('RAW ', NULL, NULL),
    DEF_ASSETTYPE('LODT', NULL, NULL),
    DEF_ASSETTYPE('SHDW', NULL, NULL),
    DEF_ASSETTYPE('DYNA', NULL, NULL),
    DEF_ASSETTYPE('VIL ', NULL, NULL),
    DEF_ASSETTYPE('VILP', NULL, NULL),
    DEF_ASSETTYPE('COLL', NULL, NULL),
    DEF_ASSETTYPE('PARP', NULL, NULL),
    DEF_ASSETTYPE('PIPT', NULL, NULL),
    DEF_ASSETTYPE('DSCO', NULL, NULL),
    DEF_ASSETTYPE('JAW ', NULL, NULL),
    DEF_ASSETTYPE('SHRP', NULL, NULL),
    DEF_ASSETTYPE('FLY ', NULL, NULL),
    DEF_ASSETTYPE('TRCK', NULL, NULL),
    DEF_ASSETTYPE('CRV ', Curve_Read, NULL),
    DEF_ASSETTYPE('ZLIN', NULL, NULL),
    DEF_ASSETTYPE('DUPC', NULL, NULL),
    DEF_ASSETTYPE('SLID', NULL, NULL),
    DEF_ASSETTYPE('CRDT', NULL, NULL),
    0
};

struct HackModelRadius
{
    uint32 assetid;
    float32 radius;
};

static HackModelRadius hackRadiusTable[] =
{
    { 0xFA77E6FA, 20.0f },
    { 0x5BD0EDAC, 1000.0f },
    { 0xED21A1C6, 50.0f }
};

static xJSPHeader* sTempJSP = NULL;
static xJSPHeader sDummyEmptyJSP;

static void ATBL_Init();

void zAssetStartup()
{
    xSTStartup(assetTypeHandlers);
    ATBL_Init();
}

void zAssetShutdown()
{
    xSTShutdown();
}

UNCHECKED static void* Model_Read(void*, uint32 assetid, void* indata, uint32 insize, uint32* outsize)
{
    RpAtomic* model = iModelFileNew(indata, insize);

    *outsize = sizeof(RpAtomic);

    for (uint32 i = 0; i < ARRAYCOUNT(hackRadiusTable); i++)
    {
        if (assetid == hackRadiusTable[i].assetid)
        {
            RpAtomic* tmpModel = model;

            while (tmpModel)
            {
                tmpModel->boundingSphere.radius = hackRadiusTable[i].radius;
                tmpModel->boundingSphere.center.x = 0.0f;
                tmpModel->boundingSphere.center.y = 0.0f;
                tmpModel->boundingSphere.center.z = 0.0f;
                tmpModel->interpolator.flags &= rpINTERPOLATORDIRTYSPHERE;

                tmpModel = iModelFile_RWMultiAtomic(tmpModel);
            }

            break;
        }
    }

    return model;
}

STUB static void* Curve_Read(void*, uint32, void* indata, uint32 insize, uint32* outsize)
{
    return NULL;
}

STUB static void Model_Unload(void* userdata, uint32 assetid)
{
}

STUB static void* BSP_Read(void*, uint32, void* indata, uint32 insize, uint32* outsize)
{
    return NULL;
}

STUB static void BSP_Unload(void* userdata, uint32 assetid)
{
}

STUB static void jsp_shadow_hack(xJSPHeader* jsp)
{

}

UNCHECKED static void* JSP_Read(void*, uint32, void* indata, uint32 insize, uint32* outsize)
{
    xJSPHeader* retjsp = &sDummyEmptyJSP;

    *outsize = sizeof(xJSPHeader);

    xJSP_MultiStreamRead(indata, insize, &sTempJSP);

    if (sTempJSP->jspNodeList)
    {
        retjsp = sTempJSP;

        sTempJSP = NULL;
        *outsize = sizeof(xJSPHeader*);
    }

    jsp_shadow_hack(retjsp);

    return retjsp;
}

UNCHECKED static void JSP_Unload(void* userdata, uint32 assetid)
{
    if ((xJSPHeader*)userdata != &sDummyEmptyJSP)
    {
        xJSP_Destroy((xJSPHeader*)userdata);
    }
}

STUB static void* RWTX_Read(void*, uint32, void* indata, uint32 insize, uint32* outsize)
{
    return NULL;
}

STUB static void TextureRW3_Unload(void* userdata, uint32 assetid)
{
}

STUB static void ATBL_Init()
{
}

STUB void FootstepHackSceneEnter()
{

}

STUB static void* ATBL_Read(void*, uint32, void* indata, uint32 insize, uint32* outsize)
{
    return NULL;
}

STUB static void Anim_Unload(void* userdata, uint32 assetid)
{
}

STUB static void LightKit_Unload(void* userdata, uint32 assetid)
{
}

STUB static void MovePoint_Unload(void* userdata, uint32 assetid)
{
}

STUB static void* SndInfoRead(void*, uint32, void* indata, uint32 insize, uint32* outsize)
{
    return NULL;
}