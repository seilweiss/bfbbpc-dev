#include "xstransvc.h"

#include "xutil.h"
#include "xString.h"
#include "iTRC.h"
#include "iFile.h"

#include <string.h>

static int32 g_straninit;
static STRAN_DATA g_xstdata;
static PACKER_READ_FUNCS* g_pkrf;
static PACKER_ASSETTYPE* g_typeHandlers;

static int32 XST_PreLoadScene(STRAN_SCENE* sdata, const char* filename);
static const char* XST_translate_sid(uint32 sid, char* exten);
static const char* XST_translate_sid_path(uint32 sid, char* exten);
static void XST_reset_raw();
static STRAN_SCENE* XST_lock_next();
static void XST_unlock(STRAN_SCENE* sdata);
static void XST_unlock_all();
static STRAN_SCENE* XST_get_rawinst(int32 i);
static int32 XST_cnt_locked();
static STRAN_SCENE* XST_nth_locked(int32 n);
static STRAN_SCENE* XST_find_bySID(uint32 sid, bool32 findTheHOP);

int32 xSTStartup(PACKER_ASSETTYPE* handlers)
{
    if (g_straninit++ == 0)
    {
        g_typeHandlers = handlers;

        XST_reset_raw();
        PKRStartup();

        g_pkrf = PKRGetReadFuncs(1);
    }

    return g_straninit;
}

int32 xSTShutdown()
{
    if (--g_straninit == 0)
    {
        g_typeHandlers = NULL;

        XST_unlock_all();
        PKRShutdown();
    }

    return g_straninit;
}

int32 xSTPreLoadScene(uint32 sid, void* userdata, int32 flg_hiphop)
{
    int32 result;
    STRAN_SCENE* sdata;
    const char* sfile;
    int32 cltver = 0;

    if ((flg_hiphop & XST_HIPHOP) == XST_HOP)
    {
        sdata = XST_lock_next();
        sdata->scnid = sid;
        sdata->userdata = userdata;
        sdata->isHOP = TRUE;

        sfile = XST_translate_sid_path(sid, ".HOP");

        if (sfile)
        {
            strcpy(sdata->fnam, sfile);

            cltver = XST_PreLoadScene(sdata, sfile);
        }

        if (cltver == 0)
        {
            sfile = XST_translate_sid(sid, ".HOP");

            if (sfile)
            {
                strcpy(sdata->fnam, sfile);

                cltver = XST_PreLoadScene(sdata, sfile);
            }
        }

        if (cltver == 0)
        {
            XST_unlock(sdata);

            result = 0;
        }
        else
        {
            result = cltver;
        }
    }
    else
    {
        do
        {
            sdata = XST_lock_next();
            sdata->scnid = sid;
            sdata->userdata = userdata;
            sdata->isHOP = FALSE;

            if (sid != 'BOOT' && sid != 'FONT')
            {
                sfile = XST_translate_sid_path(sid, ".HIP");

                if (sfile)
                {
                    strcpy(sdata->fnam, sfile);

                    cltver = XST_PreLoadScene(sdata, sfile);
                }
            }

            if (cltver == 0)
            {
                sfile = XST_translate_sid(sid, ".HIP");

                if (sfile)
                {
                    strcpy(sdata->fnam, sfile);

                    cltver = XST_PreLoadScene(sdata, sfile);
                }
            }

            if (cltver == 0)
            {
                XST_unlock(sdata);

                result = 0;
            }
            else
            {
                result = cltver;
            }
        } while (cltver == 0);
    }

    return result;
}

bool32 xSTQueueSceneAssets(uint32 sid, int32 flg_hiphop)
{
    bool32 result = TRUE;
    STRAN_SCENE* sdata;
    
    sdata = XST_find_bySID(sid, (flg_hiphop & XST_HIPHOP) == XST_HOP ? TRUE : FALSE);

    if (!sdata)
    {
        result = FALSE;
    }
    else if (sdata->spkg)
    {
        g_pkrf->LoadLayer(sdata->spkg, PKR_LTYPE_ALL);
    }

    return result;
}

void xSTUnLoadScene(uint32 sid, int32 flg_hiphop)
{
    STRAN_SCENE* sdata;
    int32 cnt;
    int32 i;

    if (sid == 0)
    {
        cnt = XST_cnt_locked();

        for (i = 0; i < cnt; i++)
        {
            sdata = XST_nth_locked(i);

            if (sdata->spkg)
            {
                g_pkrf->Done(sdata->spkg);
            }

            sdata->spkg = NULL;
        }

        XST_unlock_all();
    }
    else
    {
        sdata = XST_find_bySID(sid, (flg_hiphop & XST_HIPHOP) == XST_HOP ? TRUE : FALSE);

        if (sdata)
        {
            if (sdata->spkg)
            {
                g_pkrf->Done(sdata->spkg);
            }

            sdata->spkg = NULL;
        }

        XST_unlock(sdata);
    }
}

float32 xSTLoadStep(uint32 sid)
{
    float32 pct;
    bool32 rc;

    rc = PKRLoadStep(0);

    if (rc)
    {
        pct = 0.0f;
    }
    else
    {
        pct = 1.00001f;
    }

    iTRCDisk::CheckDVDAndResetState();
    iFileAsyncService();

    return pct;
}

void xSTDisconnect(uint32 sid, int32 flg_hiphop)
{
    STRAN_SCENE* sdata;
    
    sdata = XST_find_bySID(sid, (flg_hiphop & XST_HIPHOP) == XST_HOP ? TRUE : FALSE);

    if (sdata)
    {
        g_pkrf->PkgDisconnect(sdata->spkg);
    }
}

bool32 xSTSwitchScene(uint32 sid, void* userdata, xSTSwitchSceneProgressMonitor progmon)
{
    STRAN_SCENE* sdata;
    bool32 rc = FALSE;

    for (int32 i = 1; i >= 0; i--)
    {
        sdata = XST_find_bySID(sid, i);

        if (sdata)
        {
            if (progmon)
            {
                progmon(userdata, 0.0f);
            }

            rc = g_pkrf->SetActive(sdata->spkg, PKR_LTYPE_ALL);

            if (progmon)
            {
                progmon(userdata, 1.0f);
            }
        }
    }

    return rc;
}

const char* xSTAssetName(uint32 aid)
{
    STRAN_SCENE* sdata;
    const char* aname = NULL;
    int32 scncnt;
    int32 i;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);

        if (sdata->spkg)
        {
            aname = g_pkrf->AssetName(sdata->spkg, aid);

            if (aname)
            {
                break;
            }
        }
    }

    return aname;
}

const char* xSTAssetName(void* raw_HIP_asset)
{
    STRAN_SCENE* sdata;
    const char* aname = NULL;
    uint32 aid;
    int32 scncnt;
    int32 i;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        aid = PKRAssetIDFromInst(raw_HIP_asset);
        aname = g_pkrf->AssetName(sdata->spkg, aid);

        if (aname)
        {
            break;
        }
    }

    return aname;
}

void* xSTFindAsset(uint32 aid, uint32* size)
{
    STRAN_SCENE* sdata;
    void* memloc = NULL;
    bool32 ready;
    int32 scncnt;
    int32 i;
    bool32 rc;

    if (aid == 0)
    {
        return NULL;
    }

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        rc = g_pkrf->PkgHasAsset(sdata->spkg, aid);

        if (rc)
        {
            memloc = g_pkrf->LoadAsset(sdata->spkg, aid, NULL, NULL);
            ready = g_pkrf->IsAssetReady(sdata->spkg, aid);

            if (!ready)
            {
                memloc = NULL;

                if (size)
                {
                    *size = 0;
                }
            }
            else
            {
                if (size)
                {
                    *size = g_pkrf->GetAssetSize(sdata->spkg, aid);
                }
            }

            break;
        }
    }

    return memloc;
}

int32 xSTAssetCountByType(uint32 type)
{
    STRAN_SCENE* sdata;
    int32 sum = 0;
    int32 cnt;
    int32 scncnt;
    int32 i;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        cnt = g_pkrf->AssetCount(sdata->spkg, type);
        sum += cnt;
    }

    return sum;
}

void* xSTFindAssetByType(uint32 type, int32 idx, uint32* size)
{
    STRAN_SCENE* sdata;
    void* memptr = NULL;
    int32 scncnt;
    int32 i;
    int32 sum = 0;
    int32 cnt;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        cnt = g_pkrf->AssetCount(sdata->spkg, type);

        if (idx >= sum && idx < sum + cnt)
        {
            memptr = g_pkrf->AssetByType(sdata->spkg, type, idx - sum, size);
            break;
        }
        
        sum += cnt;
    }

    return memptr;
}

bool32 xSTGetAssetInfo(uint32 aid, PKR_ASSET_TOCINFO* tocainfo)
{
    STRAN_SCENE* sdata;
    bool32 found = FALSE;
    bool32 rc;
    int32 scncnt;
    int32 i;

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        rc = g_pkrf->PkgHasAsset(sdata->spkg, aid);

        if (rc)
        {
            g_pkrf->GetBaseSector(sdata->spkg);

            rc = g_pkrf->GetAssetInfo(sdata->spkg, aid, tocainfo);

            if (rc)
            {
                found = TRUE;
                break;
            }
        }
    }

    return found;
}

bool32 xSTGetAssetInfoByType(uint32 type, int32 idx, PKR_ASSET_TOCINFO* ainfo)
{
    STRAN_SCENE* sdata;
    bool32 found = FALSE;
    PKR_ASSET_TOCINFO tocinfo = {};
    bool32 rc;
    int32 scncnt;
    int32 i;
    int32 sum = 0;
    int32 cnt;

    memset(ainfo, 0, sizeof(PKR_ASSET_TOCINFO));

    scncnt = XST_cnt_locked();

    for (i = 0; i < scncnt; i++)
    {
        sdata = XST_nth_locked(i);
        cnt = g_pkrf->AssetCount(sdata->spkg, type);

        if (idx >= cnt && idx < sum + cnt)
        {
            g_pkrf->GetBaseSector(sdata->spkg);

            rc = g_pkrf->GetAssetInfoByType(sdata->spkg, type, idx - sum, &tocinfo);

            if (rc)
            {
                ainfo->aid = tocinfo.aid;
                ainfo->sector = tocinfo.sector;
                ainfo->plus_offset = tocinfo.plus_offset;
                ainfo->size = tocinfo.size;
                ainfo->mempos = tocinfo.mempos;

                found = TRUE;
                break;
            }
        }

        sum += cnt;
    }

    return found;
}

bool32 xSTGetAssetInfoInHxP(uint32 aid, PKR_ASSET_TOCINFO* tocinfo, uint32 hiphash)
{
    STRAN_SCENE* sdata;
    int32 cnt;
    bool32 result = FALSE;
    bool32 rc;
    uint32 hash;
    int32 i;

    cnt = XST_cnt_locked();

    for (i = 0; i < cnt; i++)
    {
        sdata = XST_nth_locked(i);
        hash = xStrHash(sdata->fnam);

        if (hiphash == hash)
        {
            rc = g_pkrf->PkgHasAsset(sdata->spkg, aid);

            if (rc)
            {
                g_pkrf->GetBaseSector(sdata->spkg);
                
                rc = g_pkrf->GetAssetInfo(sdata->spkg, aid, tocinfo);

                if (rc)
                {
                    result = TRUE;
                    break;
                }
            }
        }
    }

    return result;
}

const char* xST_xAssetID_HIPFullPath(uint32 aid)
{
    return xST_xAssetID_HIPFullPath(aid, NULL);
}

const char* xST_xAssetID_HIPFullPath(uint32 aid, uint32* sceneID)
{
    STRAN_SCENE* sdata;
    int32 cnt;
    const char* da_hipname = NULL;
    bool32 rc;
    int32 i;

    cnt = XST_cnt_locked();

    for (i = 0; i < cnt; i++)
    {
        sdata = XST_nth_locked(i);
        rc = g_pkrf->PkgHasAsset(sdata->spkg, aid);

        if (rc)
        {
            da_hipname = sdata->fnam;

            if (sceneID)
            {
                *sceneID = sdata->scnid;
            }

            break;
        }
    }

    return da_hipname;
}

static int32 XST_PreLoadScene(STRAN_SCENE* sdata, const char* filename)
{
    int32 cltver = 0;

    sdata->spkg = g_pkrf->Init(sdata->userdata, (char*)filename, 0x2E, &cltver, g_typeHandlers);

    if (sdata->spkg)
    {
        return cltver;
    }

    return 0;
}

static const char* XST_translate_sid(uint32 sid, char* exten)
{
    static char fname[64] = {};

    sprintf(fname, "%s%s",
        xUtil_idtag2string(sid),
        exten);

    return fname;
}

static const char* XST_translate_sid_path(uint32 sid, char* exten)
{
    static char fname[64] = {};

    char path_delimiter[2] = "/";

    sprintf(fname, "%c%c%s%s%s",
        xUtil_idtag2string(sid)[0],
        xUtil_idtag2string(sid)[1],
        path_delimiter,
        xUtil_idtag2string(sid),
        exten);

    return fname;
}

static void XST_reset_raw()
{
    memset(&g_xstdata, 0, sizeof(g_xstdata));
}

static STRAN_SCENE* XST_lock_next()
{
    STRAN_SCENE* sdata = NULL;
    int32 i;
    int32 uselock = -1;

    for (i = 0; i < 16; i++)
    {
        if (!(g_xstdata.loadlock & (1 << i)))
        {
            g_xstdata.loadlock |= 1 << i;

            sdata = &g_xstdata.hipscn[i];

            memset(sdata, 0, sizeof(STRAN_SCENE));

            uselock = i;

            break;
        }
    }

    if (sdata)
    {
        sdata->lockid = uselock;
    }

    return sdata;
}

static void XST_unlock(STRAN_SCENE* sdata)
{
    if (sdata)
    {
        if (g_xstdata.loadlock & (1 << sdata->lockid))
        {
            g_xstdata.loadlock &= ~(1 << sdata->lockid);

            memset(sdata, 0, sizeof(STRAN_SCENE));
        }
    }
}

static void XST_unlock_all()
{
    if (g_xstdata.loadlock)
    {
        for (int32 i = 0; i < 16; i++)
        {
            if (g_xstdata.loadlock & (1 << i))
            {
                XST_unlock(XST_get_rawinst(i));
            }
        }
    }
}

static STRAN_SCENE* XST_get_rawinst(int32 i)
{
    return &g_xstdata.hipscn[i];
}

static int32 XST_cnt_locked()
{
    int32 cnt = 0;

    for (int32 i = 0; i < 16; i++)
    {
        if (g_xstdata.loadlock & (1 << i))
        {
            cnt++;
        }
    }

    return cnt;
}

static STRAN_SCENE* XST_nth_locked(int32 n)
{
    STRAN_SCENE* da_sdata = NULL;
    int32 idx = 0;

    for (int32 i = 0; i < 16; i++)
    {
        if (g_xstdata.loadlock & (1 << i))
        {
            if (idx == n)
            {
                da_sdata = &g_xstdata.hipscn[i];
                break;
            }

            idx++;
        }
    }

    return da_sdata;
}

static STRAN_SCENE* XST_find_bySID(uint32 sid, bool32 findTheHOP)
{
    STRAN_SCENE* da_sdata = NULL;
    STRAN_SCENE* sdata;

    for (int32 i = 0; i < 16; i++)
    {
        if (g_xstdata.loadlock & (1 << i))
        {
            sdata = &g_xstdata.hipscn[i];

            if (sdata->scnid == sid &&
                (findTheHOP || !sdata->isHOP) &&
                (!findTheHOP || sdata->isHOP))
            {
                da_sdata = sdata;
                break;
            }
        }
    }

    return da_sdata;
}