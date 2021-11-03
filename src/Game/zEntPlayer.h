#ifndef ZENTPLAYER_H
#define ZENTPLAYER_H

#include "zEnt.h"
#include "zGlobalSettings.h"
#include "zLasso.h"
#include "xEntDrive.h"
#include "zCheckPoint.h"

struct zJumpParam
{
	float32 PeakHeight;
	float32 TimeGravChange;
	float32 TimeHold;
	float32 ImpulseVel;
};

struct zLedgeGrabParams
{
	float32 animGrab;
	float32 zdist;
	xVec3 tranTable[60];
	int32 tranCount;
	xEnt* optr;
	xMat4x3 omat;
	float32 y0det;
	float32 dydet;
	float32 r0det;
	float32 drdet;
	float32 thdet;
	float32 rtime;
	float32 ttime;
	float32 tmr;
	xVec3 spos;
	xVec3 epos;
	xVec3 tpos;
	int32 nrays;
	int32 rrand;
	float32 startrot;
	float32 endrot;
};

typedef enum _zPlayerType
{
	ePlayer_SB,
	ePlayer_Patrick,
	ePlayer_Sandy,
	ePlayer_MAXTYPES
} zPlayerType;

typedef enum _CurrentPlayer
{
	eCurrentPlayerSpongeBob,
	eCurrentPlayerPatrick,
	eCurrentPlayerSandy,
	eCurrentPlayerCount
} CurrentPlayer;

typedef enum _zPlayerWallJumpState
{
	k_WALLJUMP_NOT,
	k_WALLJUMP_LAUNCH,
	k_WALLJUMP_FLIGHT,
	k_WALLJUMP_LAND
} zPlayerWallJumpState;

struct zPlayerSettings
{
	zPlayerType pcType;
	float32 MoveSpeed[6];
	float32 AnimSneak[3];
	float32 AnimWalk[3];
	float32 AnimRun[3];
	float32 JumpGravity;
	float32 GravSmooth;
	float32 FloatSpeed;
	float32 ButtsmashSpeed;
	zJumpParam Jump;
	zJumpParam Bounce;
	zJumpParam Spring;
	zJumpParam Wall;
	zJumpParam Double;
	zJumpParam SlideDouble;
	zJumpParam SlideJump;
	float32 WallJumpVelocity;
	zLedgeGrabParams ledge;
	float32 spin_damp_xz;
	float32 spin_damp_y;
	uint8 talk_anims;
	uint8 talk_filter_size;
	uint8 talk_filter[4];
};

struct zPlayerCarryInfo
{
	xEnt* grabbed;
	uint32 grabbedModelID;
	xMat4x3 spin;
	xEnt* throwTarget;
	xEnt* flyingToTarget;
	float32 minDist;
	float32 maxDist;
	float32 minHeight;
	float32 maxHeight;
	float32 maxCosAngle;
	float32 throwMinDist;
	float32 throwMaxDist;
	float32 throwMinHeight;
	float32 throwMaxHeight;
	float32 throwMaxStack;
	float32 throwMaxCosAngle;
	float32 throwTargetRotRate;
	float32 targetRot;
	uint32 grabTarget;
	xVec3 grabOffset;
	float32 grabLerpMin;
	float32 grabLerpMax;
	float32 grabLerpLast;
	uint32 grabYclear;
	float32 throwGravity;
	float32 throwHeight;
	float32 throwDistance;
	float32 fruitFloorDecayMin;
	float32 fruitFloorDecayMax;
	float32 fruitFloorBounce;
	float32 fruitFloorFriction;
	float32 fruitCeilingBounce;
	float32 fruitWallBounce;
	float32 fruitLifetime;
	xEnt* patLauncher;
};

struct zPlayerLassoInfo
{
	xEnt* target;
	float32 dist;
	bool destroy;
	bool targetGuide;
	float32 lassoRot;
	xEnt* swingTarget;
	xEnt* releasedSwing;
	float32 copterTime;
	bool32 canCopter;
	zLasso lasso;
	xAnimState* zeroAnim;
};

struct xEntBoulder;
struct zEntHangable;
struct zPlatform;

#define SB_MODEL_BODY 0
#define SB_MODEL_ARM_L 1
#define SB_MODEL_ARM_R 2
#define SB_MODEL_ASS 3
#define SB_MODEL_UNDERWEAR 4
#define SB_MODEL_WAND 5
#define SB_MODEL_TONGUE 6
#define SB_MODEL_BUBBLE_HELMET 7
#define SB_MODEL_BUBBLE_SHOE_L 8
#define SB_MODEL_BUBBLE_SHOE_R 9
#define SB_MODEL_SHADOW_BODY 10
#define SB_MODEL_SHADOW_ARM_L 11
#define SB_MODEL_SHADOW_ARM_R 12
#define SB_MODEL_SHADOW_WAND 13
#define SB_MODEL_COUNT 14

struct zPlayerGlobals
{
	zEnt ent;
	xEntShadow entShadow_embedded;
	xShadowSimpleCache simpShadow_embedded;
	zGlobalSettings g;
	zPlayerSettings* s;
	zPlayerSettings sb;
	zPlayerSettings patrick;
	zPlayerSettings sandy;
	xModelInstance* model_spongebob;
	xModelInstance* model_patrick;
	xModelInstance* model_sandy;
	uint32 Visible;
	uint32 Health;
	int32 Speed;
	float32 SpeedMult;
	int32 Sneak;
	int32 Teeter;
	float32 SlipFadeTimer;
	int32 Slide;
	float32 SlideTimer;
	int32 Stepping;
	int32 JumpState;
	int32 LastJumpState;
	float32 JumpTimer;
	float32 LookAroundTimer;
	uint32 LookAroundRand;
	uint32 LastProjectile;
	float32 DecelRun;
	float32 DecelRunSpeed;
	float32 HotsauceTimer;
	float32 LeanLerp;
	float32 ScareTimer;
	xBase* ScareSource;
	float32 CowerTimer;
	float32 DamageTimer;
	float32 SundaeTimer;
	float32 ControlOffTimer;
	float32 HelmetTimer;
	uint32 WorldDisguise;
	uint32 Bounced;
	float32 FallDeathTimer;
	float32 HeadbuttVel;
	float32 HeadbuttTimer;
	uint32 SpecialReceived;
	xEnt* MountChimney;
	float32 MountChimOldY;
	uint32 MaxHealth;
	uint32 DoMeleeCheck;
	float32 VictoryTimer;
	float32 BadGuyNearTimer;
	float32 ForceSlipperyTimer;
	float32 ForceSlipperyFriction;
	float32 ShockRadius;
	float32 ShockRadiusOld;
	float32 Face_ScareTimer;
	uint32 Face_ScareRandom;
	uint32 Face_Event;
	float32 Face_EventTimer;
	float32 Face_PantTimer;
	uint32 Face_AnimSpecific;
	uint32 IdleRand;
	float32 IdleMinorTimer;
	float32 IdleMajorTimer;
	float32 IdleSitTimer;
	bool32 Transparent;
	zEnt* FireTarget;
	uint32 ControlOff;
	uint32 ControlOnEvent;
	uint32 AutoMoveSpeed;
	float32 AutoMoveDist;
	xVec3 AutoMoveTarget;
	xBase* AutoMoveObject;
	zEnt* Diggable;
	float32 DigTimer;
	zPlayerCarryInfo carry;
	zPlayerLassoInfo lassoInfo;
	xModelTag BubbleWandTag[2];
	xModelInstance* model_wand;
	xEntBoulder* bubblebowl;
	float32 bbowlInitVel;
	zEntHangable* HangFound;
	zEntHangable* HangEnt;
	zEntHangable* HangEntLast;
	xVec3 HangPivot;
	xVec3 HangVel;
	float32 HangLength;
	xVec3 HangStartPos;
	float32 HangStartLerp;
	xModelTag HangPawTag[4];
	float32 HangPawOffset;
	float32 HangElapsed;
	float32 Jump_CurrGravity;
	float32 Jump_HoldTimer;
	float32 Jump_ChangeTimer;
	bool32 Jump_CanDouble;
	bool32 Jump_CanFloat;
	bool32 Jump_SpringboardStart;
	zPlatform* Jump_Springboard;
	bool32 CanJump;
	bool32 CanBubbleSpin;
	bool32 CanBubbleBounce;
	bool32 CanBubbleBash;
	bool32 IsJumping;
	bool32 IsDJumping;
	bool32 IsBubbleSpinning;
	bool32 IsBubbleBouncing;
	bool32 IsBubbleBashing;
	bool32 IsBubbleBowling;
	bool32 WasDJumping;
	bool32 IsCoptering;
	zPlayerWallJumpState WallJumpState;
	bool32 cheat_mode;
	uint32 Inv_Shiny;
	uint32 Inv_Spatula;
	uint32 Inv_PatsSock[15];
	uint32 Inv_PatsSock_Max[15];
	uint32 Inv_PatsSock_CurrentLevel;
	uint32 Inv_LevelPickups[15];
	uint32 Inv_LevelPickups_CurrentLevel;
	uint32 Inv_PatsSock_Total;
	xModelTag BubbleTag;
	xEntDrive drv;
	xSurface* floor_surf;
	xVec3 floor_norm;
	int32 slope;
	xCollis earc_coll;
	xSphere head_sph;
	xModelTag center_tag;
	xModelTag head_tag;
	uint32 TongueFlags[2];
	xVec3 RootUp;
	xVec3 RootUpTarget;
	zCheckPoint cp;
	uint32 SlideTrackSliding;
	uint32 SlideTrackCount;
	xEnt* SlideTrackEnt[111];
	uint32 SlideNotGroundedSinceSlide;
	xVec3 SlideTrackDir;
	xVec3 SlideTrackVel;
	float32 SlideTrackDecay;
	float32 SlideTrackLean;
	float32 SlideTrackLand;
	uint8 sb_model_indices[SB_MODEL_COUNT];
	xModelInstance* sb_models[SB_MODEL_COUNT];
	uint32 currentPlayer;
	xVec3 PredictRotate;
	xVec3 PredictTranslate;
	float32 PredictAngV;
	xVec3 PredictCurrDir;
	float32 PredictCurrVel;
	float32 KnockBackTimer;
	float32 KnockIntoAirTimer;
};

#define zEntPlayer_GetLightKitID(ent, asset) (*(uint32*)((xLinkAsset*)(asset + 1) + ent->linkCount))

extern xEntBoulder* boulderVehicle;
extern zGustData gust_data;
extern CurrentPlayer gCurrentPlayer;
extern CurrentPlayer lastgCurrentPlayer;
extern zParEmitter* gEmitBFX;

void zEntPlayer_Init(xEnt* ent, xEntAsset* asset);
void zEntPlayer_RestoreSounds();
void zEntPlayer_Update(xEnt* ent, xScene* sc, float32 dt);
void zEntPlayer_Render(zEnt* ent);
void zEntPlayer_Move(xEnt* ent, xScene*, float32 dt, xEntFrame* frame);
void CalcJumpImpulse(zJumpParam* param, const zPlayerSettings* settings);
void zEntPlayerExit(xEnt*);
void zEntPlayerPreReset();
void zEntPlayerReset(xEnt* ent);
bool32 zEntPlayerEventCB(xBase* from, xBase* to, uint32 toEvent, const float32* toParam, xBase* toParamWidget);
void zEntPlayer_StoreCheckPoint(xVec3* pos, float32 rot, uint32 initCamID);
void zEntPlayer_LoadSounds();
void zEntPlayer_UnloadSounds();

#endif