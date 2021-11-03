#include "zScene.h"

#include "zEntTrigger.h"
#include "zMovePoint.h"
#include "zEntPickup.h"
#include "zEntSimpleObj.h"
#include "xParSys.h"
#include "xParEmitter.h"
#include "zPlatform.h"
#include "zPendulum.h"
#include "zEntHangable.h"
#include "zEntDestructObj.h"
#include "xEntBoulder.h"
#include "zEntButton.h"
#include "xTimer.h"
#include "xCounter.h"
#include "xSFX.h"
#include "xGroup.h"
#include "zPortal.h"
#include "zCamMarker.h"
#include "xSurface.h"
#include "zGust.h"
#include "zConditional.h"
#include "zUI.h"
#include "zUIFont.h"
#include "xFog.h"
#include "zLight.h"
#include "zCutsceneMgr.h"
#include "zEGenerator.h"
#include "zScript.h"
#include "zDiscoFloor.h"
#include "zEntTeleportBox.h"
#include "zBusStop.h"
#include "zTextBox.h"
#include "zTalkBox.h"
#include "zTaskBox.h"
#include "zTaxi.h"
#include "xHudModel.h"
#include "xHudFontMeter.h"
#include "xHudUnitMeter.h"
#include "xHudText.h"
#include "zEntPlayerBungeeState.h"
#include "zCameraFly.h"
#include "zCameraTweak.h"
#include "xString.h"
#include "xstransvc.h"
#include "zNPCMgr.h"
#include "zMain.h"
#include "zDispatcher.h"
#include "xutil.h"
#include "xScrFx.h"
#include "zGame.h"
#include "zParPTank.h"
#include "zEntPlayer.h"
#include "iSnd.h"
#include "xModelBucket.h"
#include "xPtankPool.h"
#include "xDecal.h"
#include "xFX.h"
#include "zLasso.h"
#include "zAnimList.h"
#include "zGoo.h"
#include "zCollGeom.h"
#include "zSurface.h"
#include "xFFX.h"
#include "xPartition.h"
#include "zRenderState.h"
#include "zFeet.h"
#include "zLightning.h"
#include "zParCmd.h"
#include "zActionLine.h"
#include "xShadow.h"
#include "xLightKit.h"
#include "zAssetTypes.h"
#include "zGameExtras.h"
#include "xDebug.h"
#include "zParSys.h"
#include "zThrown.h"
#include "zGrid.h"
#include "zFX.h"
#include "xEvent.h"
#include "xSystem.h"
#include "zLOD.h"
#include "xNPCBasic.h"
#include "zNPCTypeBossSandy.h"
#include "zNPCTypeBossPatrick.h"
#include "zNPCTypeTiki.h"

#include <string.h>
#include <stdio.h>

uint8 HACK_BASETYPE;
static int32 bytesNeeded;
static int32 availOnDisk;
static int32 neededFiles;
static float32 offsetx;
static float32 offsety;
static uint32 enableScreenAdj;
static float32 oldOffsetx;
static float32 oldOffsety;
static int32 sMemDepthSceneStart = -1;
static int32 sMemDepthJustHIPStart = -1;

struct zSceneLevel
{
	const char* desc;
	const char* prefix;
};

static zSceneLevel sLevelTable[] =
{
	{ "Bikini Bottom", "HB" },
	{ "Jellyfish Fields", "JF"} ,
	{ "Downtown Bikini Bottom", "BB" },
	{ "Goo Lagoon", "GL" },
	{ "Poseidome", "B1" },
	{ "Rock Bottom", "RB" },
	{ "Mermalair", "BC" },
	{ "Sand Mountain", "SM" },
	{ "Industrial Park", "B2" },
	{ "Kelp Forest", "KF" },
	{ "Flying Dutchman's Graveyard", "GY" },
	{ "Spongebob's Dream", "DB" },
	{ "Chum Bucket Lab", "B3" },
	{ "PLAYGROUND", "PG" },
	{ "Start", "MN" }
};

struct zSceneObjectInstanceDesc
{
	const char* name;
	int32 baseType;
	uint32 assetType;
	uint32 sizeRuntime;
	uint32(*func)(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
	void (*objectInitFunc)(void* ent, void* asset);
	uint32(*querySubObjects)(void*);
};

static uint32 zSceneInitFunc_DefaultEnt(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Default(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_MovePoint(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_SBNPC(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Player(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Camera(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Surface(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Gust(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Volume(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_LobMaster(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);
static uint32 zSceneInitFunc_Dispatcher(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx);

static zSceneObjectInstanceDesc sInitTable[] =
{
#if 0
	{ "Trig", eBaseTypeTrigger, 'TRIG', sizeof(zEntTrigger), zSceneInitFunc_DefaultEnt, zEntTriggerInit, NULL },
	{ "Move Point", eBaseTypeMovePoint, 'MVPT', sizeof(zMovePoint), zSceneInitFunc_MovePoint, NULL, NULL },
	{ "Pickup", eBaseTypePickup, 'PKUP', sizeof(zEntPickup), zSceneInitFunc_DefaultEnt, zEntPickupInit, NULL },
	{ "Simple", eBaseTypeStatic, 'SIMP', sizeof(zEntSimpleObj), zSceneInitFunc_DefaultEnt, zEntSimpleObj_Init, NULL },
	{ "ParticleSystem", eBaseTypeParticleSystem, 'PARS', sizeof(xParSys), zSceneInitFunc_Default, xParSysInit, NULL },
	{ "ParticleEmitter", eBaseTypeParticleEmitter, 'PARE', sizeof(xParEmitter), zSceneInitFunc_Default, xParEmitterInit, NULL },
	{ "Track", eBaseTypeTrackPhysics, 'TRCK', sizeof(zEntSimpleObj), zSceneInitFunc_DefaultEnt, zEntTrackPhysics_Init, NULL },
	{ "Platform", eBaseTypePlatform, 'PLAT', sizeof(zPlatform), zSceneInitFunc_DefaultEnt, zPlatform_Init, NULL },
	{ "Pendulum", eBaseTypePendulum, 'PEND', sizeof(zPendulum), zSceneInitFunc_DefaultEnt, zPendulum_Init, NULL },
	{ "Hanger", eBaseTypeHangable, 'HANG', sizeof(zEntHangable), zSceneInitFunc_DefaultEnt, zEntHangable_Init, NULL },
	{ "DestructObj", eBaseTypeDestructObj, 'DSTR', sizeof(zEntDestructObj), zSceneInitFunc_DefaultEnt, zEntDestructObj_Init, NULL },
	{ "Boulder", eBaseTypeBoulder, 'BOUL', sizeof(xEntBoulder), zSceneInitFunc_DefaultEnt, xEntBoulder_Init, NULL },
	{ "NPC", eBaseTypeNPC, 'VIL ', 0, zSceneInitFunc_SBNPC, NULL, NULL },
	{ "Button", eBaseTypeButton, 'BUTN', sizeof(zEntButton), zSceneInitFunc_DefaultEnt, zEntButton_Init, NULL },
	{ "Player", eBaseTypePlayer, 'PLYR', sizeof(zEnt), zSceneInitFunc_Player, NULL, NULL },
	{ "Timer", eBaseTypeTimer, 'TIMR', sizeof(xTimer), zSceneInitFunc_Default, xTimerInit, NULL },
	{ "Counter", eBaseTypeCounter, 'CNTR', sizeof(xCounter), zSceneInitFunc_Default, xCounterInit, NULL },
	{ "SFX", eBaseTypeSFX, 'SFX ', sizeof(xSFX), zSceneInitFunc_Default, xSFXInit, NULL },
	{ "Group", eBaseTypeGroup, 'GRUP', sizeof(xGroup), zSceneInitFunc_Default, xGroupInit, NULL },
	{ "Portal", eBaseTypePortal, 'PORT', sizeof(zPortal), zSceneInitFunc_Default, zPortalInit, NULL },
	{ "Camera", eBaseTypeCamera, 'CAM ', sizeof(zCamMarker), zSceneInitFunc_Camera, NULL, NULL },
	{ "Surface", eBaseTypeSurface, 'SURF', sizeof(xSurface), zSceneInitFunc_Surface, NULL, NULL },
	{ "Gust", eBaseTypeGust, 'GUST', sizeof(zGust), zSceneInitFunc_Gust, NULL, NULL },
	{ "Volume", eBaseTypeVolume, 'VOLU', sizeof(zVolume), zSceneInitFunc_Volume, NULL, NULL },
	{ "Conditional", eBaseTypeCond, 'COND', sizeof(zConditional), zSceneInitFunc_Default, zConditionalInit, NULL },
	{ "Lob Master", eBaseTypeLobMaster, 'LOBM', 0, zSceneInitFunc_LobMaster, NULL, NULL },
#endif
	{ "Env", eBaseTypeEnv, 'ENV ', sizeof(zEnv), zSceneInitFunc_Default, zEnvInit, NULL },
#if 0
	{ "Dispatcher", eBaseTypeDispatcher, 'DPAT', 0, zSceneInitFunc_Dispatcher, NULL, NULL },
	{ "UI", eBaseTypeUI, 'UI  ', sizeof(zUI), zSceneInitFunc_DefaultEnt, zUI_Init, NULL },
	{ "UI Font", eBaseTypeUIFont, 'UIFT', sizeof(zUIFont), zSceneInitFunc_Default, zUIFont_Init, NULL },
	{ "Fog", eBaseTypeFog, 'FOG ', sizeof(xFog), zSceneInitFunc_Default, xFogInit, NULL },
	{ "Light", eBaseTypeLight, 'LITE', sizeof(zLight), zSceneInitFunc_Default, zLightInit, NULL },
	{ "CutSceneMgr", eBaseTypeCutsceneMgr, 'CSNM', sizeof(zCutsceneMgr), zSceneInitFunc_Default, zCutsceneMgrInit, NULL },
	{ "EGenerator", eBaseTypeEGenerator, 'EGEN', sizeof(zEGenerator), zSceneInitFunc_DefaultEnt, zEGenerator_Init, NULL },
	{ "Script", eBaseTypeScript, 'SCRP', sizeof(zScript), zSceneInitFunc_Default, zScriptInit, NULL },
	{ "Disco Floor", eBaseTypeDiscoFloor, 'DSCO', sizeof(z_disco_floor), zSceneInitFunc_Default, z_disco_floor::init, NULL },
#endif
	{ NULL }
};

static void zSceneObjHashtableInit(int32 count);
static void zSceneObjHashtableExit();
static int32 zSceneObjHashtableUsage();
static void zSceneObjHashtableAdd(uint32 id, xBase* base);
static xBase* zSceneObjHashtableGet(uint32 id);
static xBase* zSceneExitSoundIteratorCB(xBase* b, zScene*, void*);
static void zSceneAutoSave();

namespace {
	struct dynamic_type_data
	{
		const char* name;
		int32 type;
		ulong32 size;
		bool is_ent;
		void (*load)(xBase& data, xDynAsset& asset, ulong32 size);
	};

	const dynamic_type_data dynamic_types[] =
	{
		{ "game_object:Teleport", eBaseTypeTeleportBox, sizeof(zEntTeleportBox), true, zEntTeleportBox_Init },
		{ "game_object:BusStop", eBaseTypeBusStop, sizeof(zBusStop), false, zBusStop_Init },
		{ "game_object:text_box", eBaseTypeTextBox, sizeof(ztextbox), false, ztextbox::load },
		{ "game_object:talk_box", eBaseTypeTalkBox, sizeof(ztalkbox), false, ztalkbox::load },
		{ "game_object:task_box", eBaseTypeTaskBox, sizeof(ztaskbox), false, ztaskbox::load },
		{ "game_object:BoulderGenerator", eBaseTypeBoulderGenerator, sizeof(xBoulderGenerator), false, xBoulderGenerator_Init },
		{ "game_object:Taxi", eBaseTypeTaxi, sizeof(zTaxi), false, zTaxi_Init },
		{ "hud:model", eBaseTypeHUD_model, sizeof(xBase) + sizeof(xhud::model_widget), false, xhud::model_widget::load },
		{ "hud:meter:font", eBaseTypeHUD_font_meter, sizeof(xBase) + sizeof(xhud::font_meter_widget), false, xhud::font_meter_widget::load },
		{ "hud:meter:unit", eBaseTypeHUD_unit_meter, sizeof(xBase) + sizeof(xhud::unit_meter_widget), false, xhud::unit_meter_widget::load },
		{ "hud:text", eBaseTypeHUD_text, sizeof(xBase) + sizeof(xhud::text_widget), false, xhud::text_widget::load },
		{ "game_object:bungee_hook", eBaseTypeBungeeHook, sizeof(bungee_state::hook_type), false, bungee_state::load },
		{ "game_object:Flythrough", eBaseTypeCameraFly, sizeof(zCameraFly), false, zCameraFly_Init },
		{ "game_object:Camera_Tweak", eBaseTypeCameraTweak, sizeof(zCameraTweak), false, zCameraTweak_Init }
	};

	int32 count_dynamic_types(const char* name)
	{
		uint32 type = xStrHash(name);
		int32 dynaCount = xSTAssetCountByType('DYNA');
		int32 count = 0;

		for (int32 i = 0; i < dynaCount; i++)
		{
			uint32 size;
			xDynAsset* asset = (xDynAsset*)xSTFindAssetByType('DYNA', i, &size);

			if (asset && asset->type == type)
			{
				count++;
			}
		}

		return count;
	}

	void add_dynamic_types(zScene& s)
	{
		for (int32 i = 0; i < sizeof(dynamic_types) / sizeof(dynamic_types[0]); i++)
		{
			int32 count = count_dynamic_types(dynamic_types[i].name);

			s.baseCount[dynamic_types[i].type] = count;
			s.num_base += count;
		}
	}

	uint32 init_dynamic_type(zScene& s, uint32 index, const dynamic_type_data& d)
	{
		uint32 count, type;
		int32 dyn_size, i, cnt;

		s.baseList[d.type] = NULL;

		if (s.baseCount[d.type] == 0)
		{
			return index;
		}

		if (d.size)
		{
			dyn_size = s.baseCount[d.type] * d.size;
			s.baseList[d.type] = (xBase*)xMEMALLOC(dyn_size);
		}

		type = xStrHash(d.name);
		count = xSTAssetCountByType('DYNA');

		for (i = 0, cnt = 0; i < (int32)count; i++)
		{
			uint32 asset_size;
			xDynAsset* a;
			xBase* b;

			a = (xDynAsset*)xSTFindAssetByType('DYNA', i, &asset_size);

			if (a && a->type == type)
			{
				a->baseType = d.type;

				xSTAssetName(a);

				b = (xBase*)((uint8*)s.baseList[d.type] + cnt * d.size);

				zSceneSet(b, index);

				if (d.load)
				{
					d.load(*b, *a, asset_size);
				}

				if (d.is_ent)
				{
					xSceneAddEnt(&s, s.ents[index]);
				}

				zSceneObjHashtableAdd(b->id, b);

				index++;
				cnt++;
			}
		}

		return index;
	}

	uint32 init_dynamic_types(zScene& s, uint32 index)
	{
		for (int32 i = 0; i < sizeof(dynamic_types) / sizeof(dynamic_types[0]); i++)
		{
			if (dynamic_types[i].load)
			{
				index = init_dynamic_type(s, index, dynamic_types[i]);
			}
		}

		return index;
	}
}

Climate gClimate;
zEnv* gCurEnv;
uint32 gTransitionSceneID;
float32 gSceneUpdateTime;
static xVec3 sOldPosPlayer;
static xVec3 sOldPosCamera;
static uint32 sSuddenMove;

struct IDBasePair
{
	uint32 id;
	xBase* base;
};

static IDBasePair* scobj_idbps;
static int32 scobj_size = -1;
static int32 nidbps = -1;

static uint32 zSceneInitFunc_DefaultEnt(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	uint8* block;
	int32 count;
	uint32 assetSize, offset;
	xBase* b;

	block = NULL;
	assetSize = 0;
	offset = desc->sizeRuntime;
	count = s->baseCount[desc->baseType];

	if (count)
	{
		block = (uint8*)xMEMALLOC(count * offset);

		s->baseList[desc->baseType] = (xBase*)block;
	}

	for (int32 i = 0; i < count; i++)
	{
		void* asset = xSTFindAssetByType(desc->assetType, i, &assetSize);
		b = (xBase*)(block + i * offset);

		zSceneSet(b, base_idx);

		if (desc->objectInitFunc)
		{
			desc->objectInitFunc(b, asset);
		}

		xSceneAddEnt(s, s->ents[base_idx]);
		zSceneObjHashtableAdd(b->id, b);

		base_idx++;
	}

	return base_idx;
}

static uint32 zSceneInitFunc_Default(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	uint8* block;
	int32 count;
	uint32 assetSize, offset;
	xBase* b;

	block = NULL;
	assetSize = 0;
	offset = desc->sizeRuntime;
	count = s->baseCount[desc->baseType];

	if (count)
	{
		block = (uint8*)xMEMALLOC(count * offset);

		s->baseList[desc->baseType] = (xBase*)block;
	}

	for (int32 i = 0; i < count; i++)
	{
		void* asset = xSTFindAssetByType(desc->assetType, i, &assetSize);
		b = (xBase*)(block + i * offset);

		zSceneSet(b, base_idx);

		if (desc->objectInitFunc)
		{
			desc->objectInitFunc(b, asset);
		}

		zSceneObjHashtableAdd(b->id, b);

		base_idx++;
	}

	return base_idx;
}

static uint32 zSceneInitFunc_MovePoint(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;
	uint32 assetSize;
	zMovePoint* movpBlock;

	assetSize = 0;
	count = s->baseCount[desc->baseType];
	movpBlock = zMovePoint_GetMemPool(count);

	if (movpBlock)
	{
		s->baseList[desc->baseType] = movpBlock;

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = zMovePoint_GetInst(idx);
			xBaseAsset* basset = (xBaseAsset*)xSTFindAssetByType('MVPT', idx, &assetSize);

			zSceneSet(b, base_idx);
			zMovePointInit(zMovePoint_GetInst(idx), (xMovePointAsset*)basset);
			zSceneObjHashtableAdd(b->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

static uint32 zSceneInitFunc_SBNPC(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;

	count = s->baseCount[desc->baseType];

	if (count == 0)
	{
		return base_idx;
	}

	s->baseList[desc->baseType] = NULL;

	for (int32 i = 0; i < count; i++)
	{
		xEnt* ent;
		xEntAsset* assdat;

		assdat = (xEntAsset*)xSTFindAssetByType('VIL ', i, NULL);
		ent = zNPCMgr_createNPCInst(i, assdat);

		zSceneSet(ent, base_idx);
		xSceneAddEnt(s, s->ents[base_idx]);
		zSceneObjHashtableAdd(ent->id, ent);

		base_idx++;
	}

	return base_idx;
}

static uint32 zSceneInitFunc_Player(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;
	zEnt* entBlock;

	count = s->baseCount[desc->baseType];

	if (count)
	{
		entBlock = (zEnt*)xMEMALLOC(count * sizeof(zEnt));

		s->baseList[desc->baseType] = entBlock;

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = &globals.player.ent;
			xEntAsset* asset;

			zSceneSet(b, base_idx);

			if (idx == count - 1)
			{
				xSceneAddEnt(s, s->ents[base_idx]);
			}

			asset = (xEntAsset*)xSTFindAssetByType('PLYR', idx, NULL);

			globals.player.ent.id = asset->id;

			zSceneObjHashtableAdd(asset->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

static uint32 zSceneInitFunc_Camera(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;
	zCamMarker* camBlock;

	count = s->baseCount[desc->baseType];

	if (count)
	{
		camBlock = (zCamMarker*)xMEMALLOC(count * sizeof(zCamMarker));
		s->baseList[desc->baseType] = camBlock;

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = &camBlock[idx];
			xCamAsset* assetCam = (xCamAsset*)xSTFindAssetByType('CAM ', idx, NULL);

			zSceneSet(b, base_idx);
			zCamMarkerInit(b, assetCam);
			zSceneObjHashtableAdd(b->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

static uint32 zSceneInitFunc_Surface(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;

	count = s->baseCount[desc->baseType];

	if (count)
	{
		s->baseList[desc->baseType] = xSurfaceGetByIdx(0);

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = xSurfaceGetByIdx(idx);

			zSceneSet(b, base_idx);
			zSceneObjHashtableAdd(b->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

static uint32 zSceneInitFunc_Gust(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;

	count = s->baseCount[desc->baseType];

	zGustInit();

	if (count)
	{
		s->baseList[desc->baseType] = zGustGetGust(0);

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = zGustGetGust(idx);

			zSceneSet(b, base_idx);
			zSceneObjHashtableAdd(b->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

static uint32 zSceneInitFunc_Volume(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;

	count = s->baseCount[desc->baseType];

	zVolumeInit();

	if (count)
	{
		s->baseList[desc->baseType] = zVolumeGetVolume(0);

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = zVolumeGetVolume(idx);

			zLightSetVolume((zVolume*)b);

			zSceneSet(b, base_idx);
			zSceneObjHashtableAdd(b->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

static uint32 zSceneInitFunc_LobMaster(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	return base_idx;
}

static uint32 zSceneInitFunc_Dispatcher(zScene* s, zSceneObjectInstanceDesc* desc, uint32 base_idx)
{
	int32 count;

	count = s->baseCount[desc->baseType];

	if (count)
	{
		ZDISPATCH_DATA* dpat_pool = zDispatcher_memPool(count);
		s->baseList[desc->baseType] = dpat_pool;

		for (int32 idx = 0; idx < count; idx++)
		{
			xBase* b = zDispatcher_getInst(dpat_pool, idx);
			xEntAsset* asset = (xEntAsset*)xSTFindAssetByType('DPAT', idx, NULL);

			zSceneSet(b, base_idx);
			zDispatcher_Init((st_ZDISPATCH_DATA*)b, asset);
			zSceneObjHashtableAdd(b->id, b);

			base_idx++;
		}
	}

	return base_idx;
}

void zSceneSet(xBase* b, uint32 index)
{
	globals.sceneCur->base[index] = b;
}

static void PipeCountStuffCB(RpAtomic*, uint32 pipeFlags, uint32)
{
	if (pipeFlags)
	{
		xModelLookupCount++;
	}
}

static void PipeAddStuffCB(RpAtomic* data, uint32 pipeFlags, uint32)
{
	if (pipeFlags)
	{
		xModelLookupList[xModelLookupCount].model = data;
		xModelLookupList[xModelLookupCount].PipeFlags = pipeFlags;
		xModelLookupCount++;
	}
}

STUB static void PipeForAllSceneModels(void (*pipeCB)(RpAtomic* data, uint32 pipeFlags,
	uint32 subObjects))
{

}

void zSceneInitEnvironmentalSoundEffect()
{
	switch (globals.sceneCur->sceneID)
	{
	case 'DB06':
	case 'KF04':
	case 'GL02':
	case 'BB03':
	case 'BC01':
	case 'BC02':
	case 'BC03':
	case 'BC04':
	case 'BC05':
	{
		xSndSetEnvironmentalEffect(SND_EFFECT_CAVE);
		break;
	}
	default:
	{
		xSndSetEnvironmentalEffect(SND_EFFECT_NONE);
		break;
	}
	}
}

static uint32 BaseTypeNeedsUpdate(uint8 baseType)
{
	if (baseType == eBaseTypeUnknown || baseType == eBaseTypePlayer || baseType == eBaseTypeEnv ||
		baseType == eBaseTypeCamera || baseType == eBaseTypeStatic ||
		baseType == eBaseTypeMovePoint || baseType == eBaseTypeBubble ||
		baseType == eBaseTypePortal || baseType == eBaseTypeGroup || baseType == eBaseTypeSFX ||
		baseType == eBaseTypeFFX || baseType == eBaseTypeVFX || baseType == eBaseTypeCounter ||
		baseType == eBaseTypeProjectile || baseType == eBaseTypeGust ||
		baseType == eBaseTypeVolume || baseType == eBaseTypeDispatcher ||
		baseType == eBaseTypeCond || baseType == eBaseTypeUI ||
		baseType == eBaseTypeProjectileType || baseType == eBaseTypeLobMaster ||
		baseType == eBaseTypeCutsceneMgr || baseType == eBaseTypeHud ||
		baseType == eBaseTypeNPCProps || baseType == eBaseTypeParticleEmitterProps ||
		baseType == eBaseTypeCruiseBubble || baseType == eBaseTypeTextBox ||
		baseType == eBaseTypeTalkBox || baseType == eBaseTypeTaskBox ||
		baseType == eBaseTypeBoulderGenerator || baseType == eBaseTypeNPCSettings ||
		baseType == eBaseTypeTurret)
	{
		return 0;
	}

	return 1;
}

inline void add_scene_tweaks()
{
}

void zSceneInit(uint32 theSceneID, bool32 reloadInProgress)
{
	float32 pdone;
	zScene* s;
	uint32 i;
	uint8 rgba_bkgrd[4] = { 15, 15, 15, 0 };

	gTransitionSceneID = theSceneID;
	gOccludeCount = 0;

	char b[5] = {};

	sprintf(b, xUtil_idtag2string(theSceneID));
	xStrupr(b);

	theSceneID = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];

	XPRINTF("zSceneInit %s\n", xUtil_idtag2string(theSceneID));

	xFogClearFog();
	xSndSceneInit();

	if (!reloadInProgress)
	{
		sMemDepthSceneStart = xMemPushBase();
	}

	zGameScreenTransitionBegin();
	zParPTankInit();

	pdone = 2.0f;

	if (globals.useHIPHOP && !reloadInProgress)
	{
		zGameScreenTransitionUpdate(pdone, "... scene preload ...\n");

		int32 ver_hop = xSTPreLoadScene(theSceneID, NULL, XST_HOP);

		if (ver_hop >= 0x000A000F)
		{
			xSTQueueSceneAssets(theSceneID, XST_HOP);

			do
			{
				rgba_bkgrd[0] = 0;
				rgba_bkgrd[1] = 0;
				rgba_bkgrd[2] = 0;
				rgba_bkgrd[3] = 0;

				pdone += 2.0f;

				zGameScreenTransitionUpdate(pdone, "... scene loading ...\n", rgba_bkgrd);
			} while (xSTLoadStep(theSceneID) < 1.0f);

			xSTDisconnect(theSceneID, XST_HOP);
		}
	}

	sMemDepthJustHIPStart = xMemPushBase();
	
	s = (zScene*)xMEMALLOC(sizeof(zScene));

	globals.sceneCur = s;

	xSceneInit(s, 200, 2048, 2068, 250);

	s->env = (xEnv*)xMEMALLOC(sizeof(xEnv));
	s->sceneID = theSceneID;

	iTime time;
	
	time = iTimeGet();
	xUtil_idtag2string(theSceneID);
	iTimeDiffSec(time);

	xSTPreLoadScene(theSceneID, NULL, XST_HIP);

	time = iTimeGet();
	xUtil_idtag2string(theSceneID);
	iTimeDiffSec(time);

	xSTQueueSceneAssets(theSceneID, XST_HIP);

	time = iTimeGet();
	xUtil_idtag2string(theSceneID);
	iTimeDiffSec(time);

	pdone += 2.0f;

	zGameScreenTransitionUpdate(pdone, "... scene asset queue ...\n");

	time = iTimeGet();
	xUtil_idtag2string(theSceneID);
	iTimeDiffSec(time);

	do
	{
		rgba_bkgrd[0] = 0;
		rgba_bkgrd[1] = 0;
		rgba_bkgrd[2] = 0;
		rgba_bkgrd[3] = 0;

		pdone += 2.0f;

		zGameScreenTransitionUpdate(pdone, "... scene loading ...\n", rgba_bkgrd);
	} while (xSTLoadStep(theSceneID) < 1.0f);

	xSTDisconnect(theSceneID, XST_HIP);

	time = iTimeGet();
	xUtil_idtag2string(theSceneID);
	iTimeDiffSec(time);

	pdone += 2.0f;

	zGameScreenTransitionUpdate(pdone, "...initializing scene - sound\n");

	zEntPlayer_LoadSounds();
	iSndInitSceneLoaded();

	xPadRumbleEnable(globals.currentActivePad, globals.option_vibration);

	xSTSwitchScene(theSceneID, NULL, NULL);

	globals.sceneCur->resolvID = zSceneFindObject;
	globals.sceneCur->id2Name = zSceneGetName;
	globals.sceneCur->base2Name = zSceneGetName;
	
	g_xSceneCur = globals.sceneCur;

	xModelPipeNumTables = xSTAssetCountByType('PIPT');

	for (int32 i = 0; i < xModelPipeNumTables; i++)
	{
		void* data = xSTFindAssetByType('PIPT', i);

		xModelPipeCount[i] = *(int32*)data;
		xModelPipeData[i] = (xModelPipeInfo*)((int32*)data + 1);
	}

	xModelLookupCount = 0;

	PipeForAllSceneModels(PipeCountStuffCB);

	if (xModelLookupCount)
	{
		xModelLookupList = (xModelPipeLookup*)xMEMALLOC(xModelLookupCount * sizeof(xModelPipeLookup));
		xModelLookupCount = 0;

		PipeForAllSceneModels(PipeAddStuffCB);
;	}

	xModelBucket_PreCountReset();
	PipeForAllSceneModels(xModelBucket_PreCountBucket);

	xModelBucket_PreCountAlloc(256);
	PipeForAllSceneModels(xModelBucket_InsertBucket);

	xModelBucket_Init();

	add_scene_tweaks();
	xPTankPoolSceneEnter();
	zParPTankSceneEnter();
	xDecalInit();
	xFXScenePrepare();
	zLasso_scenePrepare();
	zDispatcher_scenePrepare();
	
	int32 total_npcs = xSTAssetCountByType('VIL ');

	zNPCMgr_scenePrepare(total_npcs);

	zAnimListInit();
	zGooInit(24);

	zGameScreenTransitionUpdate(100.0f, "...initializing scene - base types\n");

	for (i = 0; i < eBaseTypeCount; i++)
	{
		s->baseCount[i] = 0;
		s->baseList[i] = NULL;
	}

	zCollGeom_Init();
	zUI_Init();
	zUIFontInit();
	ztextbox::init();
	ztalkbox::init();
	ztaskbox::init();
	xCounterInit();
	zSurfaceInit();
	z_disco_floor::init();

	xModelInstStaticAlloc = TRUE;

	s->num_base = 0;

	for (i = 0; sInitTable[i].name != NULL; i++)
	{
		uint32 typeCount = xSTAssetCountByType(sInitTable[i].assetType);

		s->baseCount[sInitTable[i].baseType] = typeCount;
		s->num_base += typeCount;

		if (sInitTable[i].querySubObjects)
		{
			for (uint32 j = 0; j < typeCount; j++)
			{
				s->num_base += sInitTable[i].querySubObjects(xSTFindAssetByType(sInitTable[i].assetType, j));
			}
		}
	}

	add_dynamic_types(*s);

	if (s->num_base)
	{
		s->base = (xBase**)xMEMALLOC(s->num_base * sizeof(xBase*));
	}
	else
	{
		s->base = NULL;
	}

	zSceneObjHashtableInit(4096);
	xFFXPoolInit(12);
	xFFXShakePoolInit(3);
	xFFXRotMatchPoolInit(1);
	xEntSceneInit();
	xEntMotionDebugInit(
		s->baseCount[eBaseTypePlatform] +
		s->baseCount[eBaseTypePendulum] +
		s->baseCount[eBaseTypeButton]);
	zLightSetVolume(NULL);
	xPartitionReset();
	xFXRibbonSceneEnter();

	uint32 base_idx = 0;

	for (i = 0; sInitTable[i].name != NULL; i++)
	{
		HACK_BASETYPE = sInitTable[i].baseType;

		if (sInitTable[i].func)
		{
			base_idx = sInitTable[i].func(s, &sInitTable[i], base_idx);
		}
	}

	init_dynamic_types(*s, base_idx);

	s->num_update_base = 0;

	for (i = 0; i < s->num_base; i++)
	{
		if (BaseTypeNeedsUpdate(s->base[i]->baseType))
		{
			s->num_update_base++;
		}
	}

	s->update_base = (xBase**)xMEMALLOC(s->num_update_base * sizeof(xBase*));

	base_idx = 0;

	for (i = 0; i < s->num_base; i++)
	{
		if (BaseTypeNeedsUpdate(s->base[i]->baseType))
		{
			s->update_base[base_idx] = s->base[i];
			base_idx++;
		}
	}

	xModelInstStaticAlloc = FALSE;

	zGameScreenTransitionEnd();

	zSceneObjHashtableUsage();
	zUI_ScenePortalInit(s);
	zLightResolveLinks();
	zRenderStateInit();
	xFXStreakInit();
	xFXShineInit();
	xFXFireworksInit(
		"PAREMIT_FIREWORKS_TRAIL",
		"PAREMIT_FIREWORKS1",
		"PAREMIT_FIREWORKS2",
		"Fireworks_explode",
		"Fireworks_trail"
	);
	zFeetGetIDs();
	zLightningInit();
	zParCmdFindClipVolumes();
	zEntDestructObj_FindFX();
	zShrapnel_SceneInit(globals.sceneCur);
	zCameraTweakGlobal_Reset();
	zActionLineInit();
	xScrFxLetterboxReset();
	xShadowManager_Init(s->baseCount[eBaseTypeNPC] + 10);

	int32 lkitCount = xSTAssetCountByType('LKIT');
	void* lkitData;

	for (int32 i = 0; i < lkitCount; i++)
	{
		lkitData = xSTFindAssetByType('LKIT', i);

		xLightKit_Prepare(lkitData);
	}

	xClimateInit(&gClimate);
	zSceneInitEnvironmentalSoundEffect();

	sHackSmoothedUpdate = true;

	FootstepHackSceneEnter();
	zEntPickup_SceneEnter();
	xFXSceneInit();
	zGame_HackGalleryInit();
	iSndSuspendCD(0);
}

void zSceneExit(bool32 beginReload)
{
	zScene* s = globals.sceneCur;

	zThrown_Reset();
	zEntPickup_FlushGrabbed();
	zGridExit(s);
	zSceneForAllBase(zSceneExitSoundIteratorCB, NULL);
	xSndStopAll(~SND_CAT_UI);
	xSndUpdate();
	iSndWaitForDeadSounds();
	iSndSceneExit();
	xSFXEnvironmentalStreamSceneExit();
	iSndSuspendCD(1);
	iFuncProfileDump();

	RpWorld* world = s->env->geom->world;

	xModel_SceneExit(world);
	zFX_SceneExit(world);

	zEntEventAll(NULL, 0, eEventRoomEnd, NULL);
	zEntEventAll(NULL, 0, eEventSceneEnd, NULL);

	if (globals.sceneCur->pendingPortal)
	{
		char nextScene[8];
		char curScene[8];

		strcpy(nextScene, xUtil_idtag2string(globals.sceneCur->pendingPortal->passet->sceneID));
		strcpy(curScene, xUtil_idtag2string(globals.sceneCur->sceneID));

		XPRINTF("Taking portal from scene %s to scene %s...\n", nextScene, curScene);
	}

	gOccludeCount = 0;

	if (globals.sceneCur->sceneID != 'MNU3')
	{
		zSceneSave(s, NULL);
	}

	zEntPlayerExit(&globals.player.ent);
	zSurfaceExit();
	zLightDestroyAll();
	xEntMotionDebugExit();
	zSceneObjHashtableExit();

	if (s->baseCount[eBaseTypeParticleSystem])
	{
		zParSys* ps = (zParSys*)s->baseList[eBaseTypeParticleSystem];
		
		for (uint32 i = 0; i < s->baseCount[eBaseTypeParticleSystem]; i++)
		{
			if (xBaseIsValid(ps))
			{
				xParSysExit(ps);
			}
		}
	}
	
	xParEmitterDestroy();
	xModelBucket_Deinit();
	xFXSceneFinish();
	zGooExit();
	zParPTankExit();
	zAnimListExit();
	zNPCMgr_sceneFinish();
	zDispatcher_sceneFinish();
	z_disco_floor::destroy();
	xDecalDestroy();
	zParPTankSceneExit();
	xPTankPoolSceneExit();
	zEntPlayer_UnloadSounds();

	if (beginReload)
	{
		xSTUnLoadScene(globals.sceneCur->sceneID, XST_HIP);
		xMemPopBase(sMemDepthJustHIPStart);

		sMemDepthJustHIPStart = -1;
	}
	else
	{
		xSTUnLoadScene(globals.sceneCur->sceneID, XST_HIP);

		if (globals.useHIPHOP)
		{
			xSTUnLoadScene(globals.sceneCur->sceneID, XST_HOP);
		}

		xMemPopBase(sMemDepthSceneStart);

		sMemDepthSceneStart = -1;
		sMemDepthJustHIPStart = -1;
	}

	xSystem_GapTrackReport();

	XPRINTF("zSceneExit finished: %s\n", xUtil_idtag2string(globals.sceneCur->sceneID));

	globals.sceneCur = NULL;

	xSceneExit(s);
}

void zSceneUpdateSFXWidgets()
{
	zScene* s = globals.sceneCur;
	xSFXUpdateEnvironmentalStreamSounds((xSFX*)s->baseList[eBaseTypeSFX], s->baseCount[eBaseTypeSFX]);
}

static void HackSwapIt(char* buf, int32 size)
{
	char* end = size + buf;
	end--;

	for (int32 i = 0; i < size / 2; i++)
	{
		char tmp = *buf;
		*buf = *end;
		*end = tmp;

		buf++;
		end--;
	}
}

STUB void zSceneSwitch(zPortal* p, int32 forceSameScene)
{
}

STUB void zSceneSave(zScene* ent, xSerial* s)
{
}

STUB void zSceneLoad(zScene* ent, xSerial* s)
{
}

int32 zSceneSetup_serialTraverseCB(uint32 clientID, xSerial* xser);

STUB void zSceneReset()
{
}

static void ActivateCB(xBase* base)
{
	base->baseFlags &= (uint8)~0x40;
}

static void DeactivateCB(xBase* base)
{
	base->baseFlags |= 0x40;
}

UNCHECKED void zSceneSetup()
{
	zScene* s = globals.sceneCur;

	globals.cmgr = NULL;

	xSceneSetup(s);
	gUIMgr.Setup(s);

	s->gravity = -50.0f;
	s->drag = 0.08f;
	s->flags = 0x5;

	zNPCMgr_scenePostInit();

	if (s->baseCount[eBaseTypeGust] != 0)
	{
		zGustSetup();
	}

	if (s->baseCount[eBaseTypeVolume] != 0)
	{
		zVolumeSetup();
	}

	uint32 dontcaresize;
	xCutscene_Init(xSTFindAssetByType('CTOC', 0, &dontcaresize));

	zLOD_Setup();

	gCurEnv = NULL;

	for (uint32 i = 0; i < s->num_base; i++)
	{
		if (s->base[i])
		{
			switch (s->base[i]->baseType)
			{
			case eBaseTypeEnv:
			{
				gCurEnv = (zEnv*)s->base[i];
				zEnvSetup(gCurEnv);
				xClimateInitAsset(&gClimate, gCurEnv->easset);
				break;
			}
			case eBaseTypeNPC:
			{
				((xNPCBasic*)s->base[i])->Setup();
				break;
			}
			case eBaseTypePlatform:
			{
				zPlatform_Reset((zPlatform*)s->base[i], s);
				zPlatform_Setup((zPlatform*)s->base[i], s);
				break;
			}
			case eBaseTypeMovePoint:
			{
				zMovePointSetup((zMovePoint*)s->base[i], s);
				break;
			}
			case eBaseTypePendulum:
			{
				zPendulum_Reset((zPendulum*)s->base[i], s);
				zPendulum_Setup((zPendulum*)s->base[i], s);
				break;
			}
			case eBaseTypeButton:
			{
				zEntButton_Reset((zEntButton*)s->base[i], s);
				zEntButton_Setup((zEntButton*)s->base[i], s);
				break;
			}
			case eBaseTypeStatic:
			{
				zEntSimpleObj_Setup((zEntSimpleObj*)s->base[i]);
				break;
			}
			case eBaseTypeTrackPhysics:
			{
				zEntSimpleObj_Setup((zEntSimpleObj*)s->base[i]);
				break;
			}
			case eBaseTypeDispatcher:
			{
				zDispatcher_InitDep((ZDISPATCH_DATA*)s->base[i], s);
				break;
			}
			case eBaseTypeEGenerator:
			{
				zEGenerator_Setup((zEGenerator*)s->base[i], s);
				zEGenerator_Reset((zEGenerator*)s->base[i], s);
				break;
			}
			case eBaseTypePickup:
			{
				zEntPickup_Setup((zEntPickup*)s->base[i]);
				zEntPickup_Reset((zEntPickup*)s->base[i]);
				break;
			}
			case eBaseTypeParticleSystem:
			{
				xParSysSetup((xParSys*)s->base[i]);
				break;
			}
			case eBaseTypeSurface:
			{
				zSurfaceSetup((xSurface*)s->base[i]);
				break;
			}
			case eBaseTypeParticleEmitter:
			{
				xParEmitterSetup((xParEmitter*)s->base[i]);
				break;
			}
			case eBaseTypeGroup:
			{
				xGroupSetup((xGroup*)s->base[i]);
				break;
			}
			case eBaseTypeTeleportBox:
			{
				zEntTeleportBox_Setup((zEntTeleportBox*)s->base[i]);
				break;
			}
			case eBaseTypeBusStop:
			{
				zBusStop_Setup((zBusStop*)s->base[i]);
				break;
			}
			case eBaseTypeDiscoFloor:
			{
				((z_disco_floor*)s->base[i])->setup();
				break;
			}
			case eBaseTypeTaxi:
			{
				zTaxi_Setup((zTaxi*)s->base[i]);
				break;
			}
			case eBaseTypeCameraFly:
			{
				zCameraFly_Setup((zCameraFly*)s->base[i]);
				break;
			}
			case eBaseTypeDestructObj:
			{
				zEntDestructObj_Setup((zEntDestructObj*)s->base[i]);
				break;
			}
			case eBaseTypeBoulder:
			{
				xEntBoulder_Setup((xEntBoulder*)s->base[i]);
				break;
			}
			}
		}
	}

	zEntPickup_Setup();
	zEntTeleportBox_InitAll();
	zEntHangable_SetupFX();
	zThrown_Setup(globals.sceneCur);

	for (uint32 i = 0; i < s->num_base; i++)
	{
		if (s->base[i] && s->base[i]->baseType == eBaseTypeMovePoint)
		{
			xMovePointSplineSetup((xMovePoint*)s->base[i]);
		}
	}

	xFXSceneSetup();
	zSceneEnableVisited(s);
	xSerialTraverse(zSceneSetup_serialTraverseCB);
	xQuickCullInit(xEntGetAllEntsBox());
	zGridInit(s);
	zNPCBSandy_AddBoundEntsToGrid(s);
	zNPCBPatrick_AddBoundEntsToGrid(s);
	zNPCTiki_InitStacking(globals.sceneCur);
	zNPCTiki_InitFX(globals.sceneCur);

	enableScreenAdj = 0;

	for (uint32 i = 0; i < s->num_base; i++)
	{
		if (s->base[i] && s->base[i]->baseType == eBaseTypeNPC)
		{
			xEnt* ent = (xEnt*)s->base[i];
			zLODTable* lod = zLOD_Get(ent);

			if (lod)
			{
				RpAtomic* tryshad = NULL;

				if (lod->lodBucket[2])
				{
					tryshad = (*lod->lodBucket[2])->Data;
				}
				else if (lod->lodBucket[1])
				{
					tryshad = (*lod->lodBucket[1])->Data;
				}
				else if (lod->lodBucket[0])
				{
					tryshad = (*lod->lodBucket[0])->Data;
				}

				if (tryshad && RpClumpGetNumAtomics(RpAtomicGetClump(tryshad)) == 1)
				{
					ent->entShadow->shadowModel = tryshad;
				}
			}
		}
	}
	
	{
		int32 max_drivensort_iters = 256;
		uint32 driven_swapped;
		uint32 i, j;

		do
		{
			driven_swapped = FALSE;

			for (i = 0; i < s->num_update_base; i++)
			{
				if (xBaseIsEnt(s->update_base[i]))
				{
					xEnt* bdriven = (xEnt*)s->update_base[i];

					if (bdriven->driver)
					{
						for (j = i + 1; j < s->num_update_base; j++)
						{
							if (bdriven->driver == s->update_base[j])
							{
								xBase* btmp = s->update_base[i];

								s->update_base[i] = s->update_base[j];
								s->update_base[j] = btmp;

								driven_swapped = TRUE;
							}
						}
					}
				}
			}
		} while (--max_drivensort_iters && !driven_swapped);
	}

	{
		int32 i;
		uint32 f;

		xEnvAsset* easset = globals.sceneCur->zen->easset;

		if (easset->bspLightKit != 0)
		{
			globals.sceneCur->env->lightKit = (xLightKit*)xSTFindAsset(easset->bspLightKit);
		}

		if (easset->objectLightKit != 0)
		{
			xLightKit* objLightKit = (xLightKit*)xSTFindAsset(easset->objectLightKit);

			if (objLightKit)
			{
				zScene* zsc = globals.sceneCur;

				for (f = 0; f < zsc->num_base; f++)
				{
					if (xBaseIsEnt(zsc->base[f]))
					{
						xEnt* tgtent = (xEnt*)zsc->base[f];

						if (tgtent->model && (tgtent->model->PipeFlags & 0xC0) != 0x40)
						{
							tgtent->lightKit = objLightKit;
						}
					}
				}
			}
		}

		int32 lkitCount = xSTAssetCountByType('LKIT');

		for (i = 0; i < lkitCount; i++)
		{
			xLightKit* lkit = (xLightKit*)xSTFindAssetByType('LKIT', i);

			if (lkit->groupID != 0)
			{
				xGroup* group = (xGroup*)zSceneFindObject(lkit->groupID);

				if (group)
				{
					uint32 j;
					uint32 nitam = xGroupGetCount(group);

					for (j = 0; j < nitam; j++)
					{
						xBase* itamz = xGroupGetItemPtr(group, j);

						if (itamz && xBaseIsEnt(itamz))
						{
							xEnt* entitam = (xEnt*)itamz;

							if (entitam->model && (entitam->model->PipeFlags & 0xC0) != 0x40)
							{
								entitam->lightKit = lkit;
							}
						}
					}
				}
			}
		}
	}
	
	{
		zEntSimpleObj_MgrInit((zEntSimpleObj**)&s->act_ents[s->baseCount[eBaseTypeTrigger] + s->baseCount[eBaseTypePickup]], s->baseCount[eBaseTypeStatic]);

		xEnt** entList = &s->act_ents[s->baseCount[eBaseTypeTrigger] + s->baseCount[eBaseTypePickup]];
		uint32 entCount = s->baseCount[eBaseTypeStatic] + s->baseCount[eBaseTypePlatform] + s->baseCount[eBaseTypePendulum] + s->baseCount[eBaseTypeHangable] + s->baseCount[eBaseTypeDestructObj] + s->baseCount[eBaseTypeBoulder] + s->baseCount[eBaseTypeNPC] + s->baseCount[eBaseTypeButton];
		uint32 i, j, k;
		uint32 numPrimeMovers = 0;
		uint32 numDriven = 0;

		for (i = 0; i < s->num_base; i++)
		{
			if (xBaseIsEnt(s->base[i]))
			{
				((xEnt*)s->base[i])->isCulled = 0;
			}
		}

		for (i = 0; i < s->num_base; i++)
		{
			if (xBaseIsEnt(s->base[i]))
			{
				xEnt* ent = (xEnt*)s->base[i];

				if (ent->driver)
				{
					if (ent->isCulled == 0)
					{
						ent->isCulled = 2;
						numDriven++;
					}

					while (ent->driver)
					{
						ent = ent->driver;
					}

					if (ent->isCulled == 0)
					{
						ent->isCulled = 1;
						numPrimeMovers++;
					}
				}
			}
		}

		uint32 numGroups = 0;

		for (i = 0; i < s->num_base; i++)
		{
			if (s->base[i]->baseType == eBaseTypeGroup)
			{
				xGroup* grp = (xGroup*)s->base[i];

				for (j = 0; j < grp->linkCount; j++)
				{
					// BUG: only checking the 1st link
					// grp->link should be grp->link[j]

					if (grp->link->srcEvent == eEventSceneBegin &&
						grp->link->dstEvent == eEventGroupUpdateTogether)
					{
						numGroups++;

						uint32 gcnt = xGroupGetCount(grp);

						for (k = 0; k < gcnt; k++)
						{
							xBase* gbase = xGroupGetItemPtr(grp, k);

							if (gbase && xBaseIsEnt(gbase))
							{
								xEnt* gent = (xEnt*)gbase;

								if (gent->isCulled != 0)
								{
									if (gent->isCulled == 1)
									{
										numPrimeMovers--;
									}

									if (gent->isCulled != 1)
									{
										numDriven--;
									}

									gent->isCulled = 0;
								}
							}
						}
					}
				}
			}
		}

		xGroup* driveGroupList = NULL;

		if (numPrimeMovers != 0)
		{
			uint32 allocsize = numPrimeMovers * sizeof(xGroup) + numPrimeMovers * sizeof(xGroupAsset) + (numPrimeMovers + numDriven) * sizeof(xBase*);

			driveGroupList = (xGroup*)RwMalloc(allocsize, rwMEMHINTDUR_FUNCTION);

			memset(driveGroupList, 0, allocsize);

			xGroupAsset* grpAssetList = (xGroupAsset*)(driveGroupList + numPrimeMovers);
			xBase** grpBaseList = (xBase**)(grpAssetList + numPrimeMovers);

			for (i = 0; i < numPrimeMovers; i++)
			{
				driveGroupList[i].baseType = eBaseTypeGroup;
				driveGroupList[i].asset = &grpAssetList[i];
				driveGroupList[i].flg_group |= 0x1;
			}

			xGroup* driveGroupCurr = driveGroupList;

			for (i = 0; i < s->num_base; i++)
			{
				if (xBaseIsEnt(s->base[i]))
				{
					xEnt* ent = (xEnt*)s->base[i];

					if (ent->isCulled == 1)
					{
						xGroupAsset* gasset = driveGroupCurr->asset;

						driveGroupCurr->item = grpBaseList;

						*grpBaseList = ent;
						grpBaseList++;

						gasset->itemCount++;

						for (j = 0; j < s->num_base; j++)
						{
							if (xBaseIsEnt(s->base[j]))
							{
								xEnt* other = (xEnt*)s->base[j];

								if (other->isCulled == 2)
								{
									while (other->driver)
									{
										other = other->driver;
									}

									if (ent == other)
									{
										*grpBaseList = s->base[j];
										grpBaseList++;

										gasset->itemCount++;
									}
								}
							}
						}

						if (gasset->itemCount > 1)
						{
							numGroups++;
						}

						driveGroupCurr++;
					}
				}
			}
		}

		xGroup** tempGrpList = NULL;

		if (numGroups != 0)
		{
			tempGrpList = (xGroup**)RwMalloc(numGroups * sizeof(xGroup*), rwMEMHINTDUR_FUNCTION);

			xGroup** tempGrpCurr = tempGrpList;

			for (i = 0; i < s->num_base; i++)
			{
				if (s->base[i]->baseType == eBaseTypeGroup)
				{
					xGroup* grp = (xGroup*)s->base[i];

					for (j = 0; j < grp->linkCount; j++)
					{
						// BUG: only checking the 1st link
						// grp->link should be grp->link[j]

						if (grp->link->srcEvent == eEventSceneBegin &&
							grp->link->dstEvent == eEventGroupUpdateTogether)
						{
							*tempGrpCurr = grp;
							tempGrpCurr++;
						}
					}
				}
			}

			for (i = 0; i < numPrimeMovers; i++)
			{
				if (driveGroupList[i].asset->itemCount > 1)
				{
					*tempGrpCurr = &driveGroupList[i];
					tempGrpCurr++;
				}
			}
		}

		globals.updateMgr = xUpdateCull_Init((void**)entList, entCount, tempGrpList, numGroups);

#ifndef STUB
		globals.updateMgr->activateCB = (xUpdateCullActivateCallback)ActivateCB;
		globals.updateMgr->deactivateCB = (xUpdateCullDeactivateCallback)DeactivateCB;
#endif

		FloatAndVoid defaultDist;
		defaultDist.f = 4900.0f;

		FloatAndVoid lodDist;
		lodDist.f = 0.0f;

		for (i = 0; i < entCount; i++)
		{
			zLODTable* lod = zLOD_Get(entList[i]);

			if (lod)
			{
				if (lod->noRenderDist == 0.0f)
				{
					lod = NULL;
				}
				else
				{
					lodDist.f = SQR(10.0f + xsqrt(lod->noRenderDist));
				}
			}

			xUpdateCull_SetCB(globals.updateMgr, entList[i], xUpdateCull_DistanceSquaredCB, lod ? lodDist.v : defaultDist.v);
		}

		if (tempGrpList)
		{
			RwFree(tempGrpList);
		}

		if (driveGroupList)
		{
			RwFree(driveGroupList);
		}

		for (i = 0; i < s->num_base; i++)
		{
			if (xBaseIsEnt(s->base[i]))
			{
				((xEnt*)s->base[i])->isCulled = 0;
			}
		}
	}
	
	zNPCMgr_scenePostSetup();
	z_disco_floor::post_setup();
	zEntPickup_RewardPostSetup();
	
	iColor black = { 0, 0, 0, 255 };
	iColor clear = { 0, 0, 0, 0 };

	xScrFxFade(&black, &clear, 1.0f, NULL, FALSE);
}

STUB int32 zSceneSetup_serialTraverseCB(uint32 clientID, xSerial* xser)
{
	return 1;
}

STUB void zSceneUpdate(float32 dt)
{
}

STUB static void zSceneRenderPreFX()
{
}

STUB static void zSceneRenderPostFX()
{
}

void zSceneRender()
{
	zSceneRenderPreFX();
	xScrFxRender(globals.camera.lo_cam);
	zSceneRenderPostFX();
}

static void zSceneObjHashtableInit(int32 count)
{
	scobj_idbps = (IDBasePair*)xMEMALLOC(count * sizeof(IDBasePair));

	memset(scobj_idbps, 0, count * sizeof(IDBasePair));

	scobj_size = count;
	nidbps = 0;
}

static void zSceneObjHashtableExit()
{
	scobj_idbps = NULL;
	scobj_size = -1;
	nidbps = -1;
}

static int32 zSceneObjHashtableUsage()
{
	return nidbps;
}

static void zSceneObjHashtableAdd(uint32 id, xBase* base)
{
	int32 k, chkd;

	chkd = id & (scobj_size - 1);

	for (k = 0; k < scobj_size; k++)
	{
		IDBasePair* idbp = &scobj_idbps[chkd];

		if (idbp->id == 0)
		{
			idbp->id = id;
			idbp->base = base;
			nidbps++;
			break;
		}

		chkd++;

		if (chkd == scobj_size)
		{
			chkd = 0;
		}
	}
}

static xBase* zSceneObjHashtableGet(uint32 id)
{
	int32 k, chkd;

	chkd = id & (scobj_size - 1);

	for (k = 0; k < scobj_size; k++)
	{
		IDBasePair* idbp = &scobj_idbps[chkd];

		if (idbp->id == id)
		{
			return idbp->base;
		}

		if (idbp->id == 0)
		{
			return NULL;
		}

		chkd++;

		if (chkd == scobj_size)
		{
			chkd = 0;
		}
	}

	return NULL;
}

xBase* zSceneFindObject(uint32 gameID)
{
	return zSceneObjHashtableGet(gameID);
}

xBase* zSceneGetObject(int32 type, int32 idx)
{
	zScene* s = globals.sceneCur;

	if (s)
	{
		for (uint32 i = 0; i < s->num_base; i++)
		{
			if (s->base[i] && type == s->base[i]->baseType)
			{
				if (idx == 0)
				{
					return s->base[i];
				}
				else
				{
					idx--;
				}
			}
		}
	}

	return NULL;
}

const char* zSceneGetName(uint32 gameID)
{
	xBase* b = zSceneFindObject(gameID);

	if (b)
	{
		return zSceneGetName(b);
	}

	return "";
}

const char* zSceneGetName(xBase* b)
{
	if (b)
	{
		const char* n = xSTAssetName(b->id);

		if (n)
		{
			return n;
		}
	}

	return "";
}

void zSceneForAllBase(xBase* (*func)(xBase*, zScene*, void*), void* data)
{
	zScene* s = globals.sceneCur;

	if (s)
	{
		for (uint16 i = 0; i < s->num_base; i++)
		{
			if (!func(s->base[i], s, data))
			{
				break;
			}
		}
	}
}

void zSceneForAllBase(xBase* (*func)(xBase*, zScene*, void*), int32 baseType, void* data)
{
	zScene* s = globals.sceneCur;

	if (s)
	{
		for (uint16 i = 0; i < s->num_base; i++)
		{
			if (baseType == s->base[i]->baseType)
			{
				if (!func(s->base[i], s, data))
				{
					break;
				}
			}
		}
	}
}

static xBase* zSceneExitSoundIteratorCB(xBase* b, zScene*, void*)
{
	xSndParentDied((uint32)b);
	return b;
}

void zSceneMemLvlChkCB()
{
}

STUB uint32 zSceneLeavingLevel()
{
	return 0;
}

const char* zSceneGetLevelName(uint32 sceneID)
{
	int8 c1 = (sceneID >> 24) & 0xFF;
	int8 c2 = (sceneID >> 16) & 0xFF;

	for (int32 i = 0; i < sizeof(sLevelTable) / sizeof(sLevelTable[0]); i++)
	{
		if (c1 == sLevelTable[i].prefix[0] && c2 == sLevelTable[i].prefix[1])
		{
			return sLevelTable[i].desc;
		}
	}

	return "Level Not Found";
}

uint32 zSceneGetLevelIndex()
{
	return zSceneGetLevelIndex(globals.sceneCur->sceneID);
}

uint32 zSceneGetLevelIndex(uint32 sceneID)
{
	int8 c1 = (sceneID >> 24) & 0xFF;
	int8 c2 = (sceneID >> 16) & 0xFF;

	for (int32 i = 0; i < sizeof(sLevelTable) / sizeof(sLevelTable[0]); i++)
	{
		if (c1 == sLevelTable[i].prefix[0] && c2 == sLevelTable[i].prefix[1])
		{
			return i;
		}
	}

	return 0;
}

const char* zSceneGetLevelPrefix(uint32 index)
{
	if (index >= sizeof(sLevelTable) / sizeof(sLevelTable[0]))
	{
		return NULL;
	}

	return sLevelTable[index].prefix;
}

const char* zSceneGetAreaname(uint32)
{
	return "Temp";
}

uint32 zSceneCalcProgress()
{
	return globals.player.Inv_Spatula;
}

void zScene_UpdateFlyToInterface(float32 dt)
{
	zScene* s = globals.sceneCur;

	zEntPickup_UpdateFlyToInterface((zEntPickup*)s->baseList[eBaseTypePickup], s->baseCount[eBaseTypePickup], dt);
}

void zSceneCardCheckStartup_set(int32 needed, int32 available, int32 files)
{
	bytesNeeded = needed;
	availOnDisk = available;
	neededFiles = files;
}

void zSceneEnableVisited(zScene* s)
{
	uint32 uiNameID;
	char uiName[64];
	const char* sceneName;

	sceneName = xUtil_idtag2string(s->sceneID);

	strcpy(uiName, sceneName);

	uiName[1] = ' ';

	strcat(uiName, " VISITED");

	uiNameID = xStrHash(uiName);

	for (uint32 i = 0; i < s->num_base; i++)
	{
		if (s->base[i] && s->base[i]->baseType == eBaseTypeUI && uiNameID == s->base[i]->id)
		{
			s->base[i]->baseFlags |= 0x1;
		}
	}
}

void zSceneEnableScreenAdj(uint32 enable)
{
	enableScreenAdj = enable;
}

void zSceneSetOldScreenAdj()
{
	oldOffsetx = offsetx;
	oldOffsety = offsety;
}

uint32 zScene_ScreenAdjustMode()
{
	return enableScreenAdj;
}

xVec3 scale = { 0.2f, -0.7f, 1.5f };

STUB void zSceneSpawnRandomBubbles()
{
}

static void zSceneAutoSave()
{
}