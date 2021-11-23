#include "zEntPlayer.h"

#include "zMain.h"
#include "xstransvc.h"
#include "xString.h"
#include "zEntCruiseBubble.h"
#include "xFFX.h"
#include "zGust.h"
#include "zEntPlayerBungeeState.h"
#include "zEntPlayerOOBState.h"
#include "xEntBoulder.h"

static bool32 sPlayerDiedLastTime;

static zPlayerLassoInfo* sLassoInfo;
static zLasso* sLasso;
static xEnt* sHitch[32];
static int32 sNumHitches;
static float32 sHitchAngle;

xEntBoulder* boulderVehicle;

static zParEmitter* sEmitSpinBubbles;
static zParEmitter* sEmitSundae;
static zParEmitter* sEmitStankBreath;
static xModelTag sStankTag[3];

zGustData gust_data;

CurrentPlayer gCurrentPlayer;
CurrentPlayer lastgCurrentPlayer = eCurrentPlayerCount;

static float32 bbash_vel;

zParEmitter* gEmitBFX;

static xModelTag sSandyLFoot;
static xModelTag sSandyRFoot;
static xModelTag sSandyLHand;
static xModelTag sSandyRHand;
static xModelTag sSandyLKnee;
static xModelTag sSandyRKnee;
static xModelTag sSandyLElbow;
static xModelTag sSandyRElbow;
static xModelTag sSpongeBobLKnee;
static xModelTag sSpongeBobRKnee;
static xModelTag sSpongeBobLElbow;
static xModelTag sSpongeBobRElbow;
static xModelTag sSpongeBobLFoot;
static xModelTag sSpongeBobRFoot;
static xModelTag sSpongeBobLHand;
static xModelTag sSpongeBobRHand;
static xModelTag sPatrickLFoot;
static xModelTag sPatrickRFoot;
static xModelTag sPatrickLHand;
static xModelTag sPatrickRHand;
static xModelTag sPatrickLKnee;
static xModelTag sPatrickRKnee;
static xModelTag sPatrickLElbow;
static xModelTag sPatrickRElbow;
static xModelTag sPatrickMelee;

static bool32 BoulderVEventCB(xBase* from, xBase* to, uint32 toEvent, const float32* toParam, xBase* toParamWidget);
static void PlayerRotMatchUpdateEnt(xEnt* ent, xScene*, float32 dt, void* fdata);
static void zEntPlayer_SNDInit();
static void PlayerHackFixBbashMiss(xModelInstance* model);

STUB static void load_player_ini()
{

}

UNCHECKED void zEntPlayer_Init(xEnt* ent, xEntAsset* asset)
{
    zEntInit((zEnt*)ent, asset, 'PLYR');

    xEntInitShadow(*ent, globals.player.entShadow_embedded);
    ent->simpShadow = &globals.player.simpShadow_embedded;
    xShadowSimple_CacheInit(ent->simpShadow, ent, 80);

    if (zEntPlayer_GetLightKitID(ent, asset) != 0)
    {
        ent->lightKit = (xLightKit*)xSTFindAsset(zEntPlayer_GetLightKitID(ent, asset));

        if (ent->lightKit && ent->lightKit->tagID != 'TIKL')
        {
            ent->lightKit = NULL;
        }
    }

    globals.player.model_spongebob = ent->model;

    memset(globals.player.sb_models, 0, sizeof(globals.player.sb_models));

    {
        uint8 index = 0;
        xModelInstance* m = globals.player.model_spongebob;

        while (m)
        {
            for (int32 i = 0; i < SB_MODEL_COUNT; i++)
            {
                if (globals.player.sb_model_indices[i] == index)
                {
                    globals.player.sb_models[i] = m;
                }
            }

            m = m->Next;
            index++;
        }
    }

    PlayerHackFixBbashMiss(globals.player.model_spongebob);

    globals.player.sb_models[SB_MODEL_SHADOW_BODY]->Parent = globals.player.model_spongebob;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_L]->Parent = globals.player.model_spongebob;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_R]->Parent = globals.player.model_spongebob;
    globals.player.sb_models[SB_MODEL_SHADOW_WAND]->Parent = globals.player.model_spongebob;

    globals.player.sb_models[SB_MODEL_SHADOW_BODY]->Flags = 0x2000;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_L]->Flags = 0x2000;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_R]->Flags = 0x2000;
    globals.player.sb_models[SB_MODEL_SHADOW_WAND]->Flags = 0x2000;

    globals.player.sb_models[SB_MODEL_SHADOW_BODY]->Mat = globals.player.model_spongebob->Mat;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_L]->Mat = globals.player.model_spongebob->Mat;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_R]->Mat = globals.player.model_spongebob->Mat;
    globals.player.sb_models[SB_MODEL_SHADOW_WAND]->Mat = globals.player.model_spongebob->Mat;

    globals.player.model_wand = globals.player.sb_models[SB_MODEL_WAND];

    iModelTagSetup(&globals.player.BubbleWandTag[0], globals.player.model_wand->Data, -0.604f, 0.46f, 0.63f);
    iModelTagSetup(&globals.player.BubbleWandTag[1], globals.player.model_wand->Data, -0.563f, 0.427f, 0.294f);

    iModelTagSetup(&sSpongeBobLFoot, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_L]->Data, 0.119f, 0.043f, -0.032f);
    iModelTagSetup(&sSpongeBobRFoot, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_R]->Data, -0.119f, 0.043f, -0.032f);
    iModelTagSetup(&sSpongeBobLKnee, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_L]->Data, 0.119f, 0.161f, 0.024f);
    iModelTagSetup(&sSpongeBobRKnee, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_R]->Data, -0.118f, 0.161f, 0.024f);
    iModelTagSetup(&sSpongeBobLHand, globals.player.sb_models[SB_MODEL_ARM_L]->Data, 0.55f, 0.496f, 0.02f);
    iModelTagSetup(&sSpongeBobRHand, globals.player.sb_models[SB_MODEL_ARM_R]->Data, -0.55f, 0.496f, -0.011f);
    iModelTagSetup(&sSpongeBobRElbow, globals.player.sb_models[SB_MODEL_ARM_R]->Data, -0.442f, 0.458f, -0.007f);
    iModelTagSetup(&sSpongeBobLElbow, globals.player.sb_models[SB_MODEL_ARM_L]->Data, 0.43f, 0.458f, -0.004f);

    iModelTagSetup(&sStankTag[0], globals.player.ent.model->Data, -0.014f, 0.546f, 0.168f);

    sEmitSpinBubbles = zParEmitterFind("PAREMIT_GRAB_BUBBLES");
    sEmitSundae = zParEmitterFind("PAREMIT_CLOUD");
    sEmitStankBreath = zParEmitterFind("PAREMIT_STANK");
    gEmitBFX = zParEmitterFind("PAREMIT_BFX");

    sLassoInfo = &globals.player.lassoInfo;
    sLasso = &sLassoInfo->lasso;

    {
        float32 bbncvtm = globals.player.g.BBashTime - globals.player.g.BBashCVTime;

        bbash_vel = (0.5f * globals.player.g.Gravity * SQR(bbncvtm) + globals.player.g.BBashHeight) / globals.player.g.BBashTime;
    }

    {
        uint32 bufsize;
        void* info;

        info = xSTFindAsset(0x791025AC, &bufsize);

        if (info)
        {
            globals.player.model_patrick = zEntRecurseModelInfo(info, ent);

            iModelTagSetup(&sPatrickMelee, globals.player.model_patrick->Data, 0.0f, 0.475f, 0.252f);
            iModelTagSetup(&sPatrickLFoot, globals.player.model_patrick->Data, 0.187f, 0.0f, -0.068f);
            iModelTagSetup(&sPatrickRFoot, globals.player.model_patrick->Data, -0.187f, 0.0f, -0.068f);
            iModelTagSetup(&sPatrickLKnee, globals.player.model_patrick->Data, 0.19f, 0.099f, -0.138f);
            iModelTagSetup(&sPatrickRKnee, globals.player.model_patrick->Data, -0.19f, 0.099f, -0.138f);
            iModelTagSetup(&sPatrickRHand, globals.player.model_patrick->Data, -0.632f, 0.711f, -0.235f);
            iModelTagSetup(&sPatrickLHand, globals.player.model_patrick->Data, 0.684f, 0.694f, -0.215f);
            iModelTagSetup(&sPatrickRElbow, globals.player.model_patrick->Data, -0.475f, 0.733f, -0.269f);
            iModelTagSetup(&sPatrickLElbow, globals.player.model_patrick->Data, 0.475f, 0.733f, -0.269f);
        }
        else
        {
            globals.player.model_patrick = NULL;
        }

        info = xSTFindAsset(0xC0E34B23, &bufsize);

        if (info)
        {
            globals.player.model_sandy = zEntRecurseModelInfo(info, ent);

            zLasso_Init(sLasso, globals.player.model_sandy, -0.599f, 0.645f, 0.051f);

            iModelTagSetup(&sSandyLFoot, globals.player.model_sandy->Data, 0.159f, 0.0f, 0.045f);
            iModelTagSetup(&sSandyRFoot, globals.player.model_sandy->Data, -0.012f, 0.0f, 0.258f);
            iModelTagSetup(&sSandyLKnee, globals.player.model_sandy->Data, 0.129f, 0.287f, 0.089f);
            iModelTagSetup(&sSandyRKnee, globals.player.model_sandy->Data, -0.071f, -0.071f, 0.089f);
            iModelTagSetup(&sSandyRHand, globals.player.model_sandy->Data, -0.642f, 0.747f, 0.006f);
            iModelTagSetup(&sSandyLHand, globals.player.model_sandy->Data, 0.641f, 0.747f, 0.006f);
            iModelTagSetup(&sSandyRElbow, globals.player.model_sandy->Data, -0.37f, 0.661f, 0.086f);
            iModelTagSetup(&sSandyLElbow, globals.player.model_sandy->Data, 0.37f, 0.661f, 0.086f);
        }
        else
        {
            globals.player.model_sandy = NULL;
        }
    }

    static void** drybob_chgData[64];
    static void* drybob_oldData[64];
    static float32* drybob_chgTime[64];
    static float32 drybob_oldTime[64];
    static int32 drybob_anim_count = 0;

    for (int32 i = 0; i < drybob_anim_count; i++)
    {
        *drybob_chgData[i] = drybob_oldData[i];
        *drybob_chgTime[i] = drybob_oldTime[i];
    }

    drybob_anim_count = 0;

    {
        RpAtomic* armL = (RpAtomic*)xSTFindAsset(xStrHash("spongebob_bind_treedome.dff"));

        if (armL)
        {
            RpAtomic* armR = iModelFile_RWMultiAtomic(armL);
            RpAtomic* body = iModelFile_RWMultiAtomic(iModelFile_RWMultiAtomic(armR));

            globals.player.sb_models[SB_MODEL_ARM_L]->Data = armL;
            globals.player.sb_models[SB_MODEL_ARM_R]->Data = armR;
            globals.player.sb_models[SB_MODEL_BODY]->Data = body;

            xAnimTable* wettbl = (xAnimTable*)xSTFindAsset(xStrHash("spongebob_bind.ATBL"));
            xAnimTable* drytbl = (xAnimTable*)xSTFindAsset(xStrHash("spongebob_bind_treedome.ATBL"));

            if (wettbl && drytbl)
            {
                xAnimState* wetstate;
                xAnimState* drystate = drytbl->StateList;

                while (drystate)
                {
                    if (!(drystate->UserFlags & 0x40000000))
                    {
                        wetstate = xAnimTableGetState(wettbl, drystate->Name);

                        xAnimFile* wetfile = wetstate->Data;
                        xAnimFile* dryfile = drystate->Data;

                        int32 aa;
                        int32 numa = wetfile->NumAnims[0] * wetfile->NumAnims[1];

                        for (aa = 0; aa < numa; aa++)
                        {
                            drybob_chgData[drybob_anim_count] = &wetfile->RawData[aa];
                            drybob_oldData[drybob_anim_count] = wetfile->RawData[aa];
                            drybob_chgTime[drybob_anim_count] = &wetfile->Duration;
                            drybob_oldTime[drybob_anim_count] = wetfile->Duration;

                            wetfile->RawData[aa] = dryfile->RawData[aa];

                            drybob_anim_count++;
                        }

                        wetfile->Duration = dryfile->Duration;
                    }

                    drystate = drystate->Next;
                }
            }
        }
    }

    cruise_bubble::init();
    load_player_ini();

    xEntEnable(ent);
    xEntShow(ent);

    globals.player.Visible = 1;
    globals.player.AutoMoveSpeed = 0;

    ent->pflags &= ~0x4;
    ent->collis->chk &= ~0x1;
    ent->update = zEntPlayer_Update;
    ent->move = zEntPlayer_Move;
    ent->render = (xEntRenderCallback)zEntPlayer_Render;
    ent->eventFunc = zEntPlayerEventCB;

    if (ent->linkCount != 0)
    {
        ent->link = (xLinkAsset*)(asset + 1);
    }
    else
    {
        ent->link = NULL;
    }

    if (globals.sceneCur->baseCount[eBaseTypeGust] != 0)
    {
        xFFXAddEffect(ent, zGustUpdateEnt, &gust_data);
    }

    xFFXRotMatchState* rms = xFFXRotMatchAlloc();

    if (rms)
    {
        rms->max_decl = globals.player.g.RotMatchMaxAngle;
        rms->tmatch = globals.player.g.RotMatchMatchTime;
        rms->trelax = globals.player.g.RotMatchRelaxTime;
        rms->tmr = 0.0f;

        xFFXAddEffect(ent, PlayerRotMatchUpdateEnt, rms);
    }

    bungee_state::init();
    oob_state::init();

    boulderVehicle = NULL;
    boulderVehicle = (xEntBoulder*)zSceneFindObject(xStrHash("BOULDER_VEHICLE"));

    if (boulderVehicle)
    {
        boulderVehicle->eventFunc = BoulderVEventCB;
    }

    sNumHitches = 0;
    sHitchAngle = 0.0f;

    {
        uint32 trailerHash = xStrHash("trailer_hitch");

        for (uint32 i = 0; i < globals.sceneCur->num_ents; i++)
        {
            xEnt* hitch = globals.sceneCur->ents[i];

            if ((hitch->baseType == eBaseTypeDestructObj ||
                 hitch->baseType == eBaseTypePlatform ||
                 hitch->baseType == eBaseTypeStatic) &&
                hitch->asset->modelInfoID == trailerHash)
            {
                sHitch[sNumHitches] = hitch;
                sNumHitches++;
            }
        }
    }

    lastgCurrentPlayer = eCurrentPlayerCount;

    zEntPlayerPreReset();
    zEntPlayerReset(ent);
    zEntPlayer_SNDInit();

    sPlayerDiedLastTime = FALSE;

    zEntPlayer_RestoreSounds();
}

STUB void zEntPlayer_RestoreSounds()
{

}

STUB static bool32 BoulderVEventCB(xBase* from, xBase* to, uint32 toEvent, const float32* toParam, xBase* toParamWidget)
{
    return TRUE;
}

STUB void zEntPlayer_Update(xEnt* ent, xScene* sc, float32 dt)
{

}

STUB void zEntPlayer_Render(zEnt* ent)
{

}

STUB void zEntPlayer_Move(xEnt* ent, xScene*, float32 dt, xEntFrame* frame)
{

}

STUB void CalcJumpImpulse(zJumpParam* param, const zPlayerSettings* settings)
{

}

STUB void zEntPlayerExit(xEnt*)
{

}

STUB void zEntPlayerPreReset()
{

}

STUB void zEntPlayerReset(xEnt* ent)
{

}

STUB bool32 zEntPlayerEventCB(xBase* from, xBase* to, uint32 toEvent, const float32* toParam, xBase* toParamWidget)
{
    return TRUE;
}

STUB static void PlayerRotMatchUpdateEnt(xEnt* ent, xScene*, float32 dt, void* fdata)
{

}

STUB void zEntPlayer_StoreCheckPoint(xVec3* pos, float32 rot, uint32 initCamID)
{

}

STUB void zEntPlayer_SNDInit()
{

}

STUB static void PlayerHackFixBbashMiss(xModelInstance* model)
{

}

STUB void zEntPlayer_LoadSounds()
{

}

STUB void zEntPlayer_UnloadSounds()
{

}