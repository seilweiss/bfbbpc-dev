#include "iSystem.h"

#include "xDebug.h"
#include "xMemMgr.h"
#include "iFile.h"
#include "iTime.h"
#include "xPad.h"
#include "xSnd.h"
#include "xMath.h"
#include "xMath3.h"
#include "xShadow.h"
#include "xFX.h"

#include <SDL.h>
#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rpskin.h>
#include <rphanim.h>
#include <rpmatfx.h>
#include <rpusrdat.h>
#include <rpptank.h>

#ifdef _WIN32
#include <windows.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif
#endif

#define WINDOW_TITLE "SpongeBob SquarePants: Battle for Bikini Bottom"

#if 0
#define RW_ARENA_SIZE (16 << 20)
#else
#define RW_ARENA_SIZE 0
#endif

static SDL_Window* sSDLWindow;
static bool sShouldQuit;

RwVideoMode sVideoMode;

static uint32 RenderWareInit();
static void RenderWareExit();
static RwTexture* TextureRead(const char* name, const char* maskName);

RwMemoryFunctions* psGetMemoryFunctions()
{
#ifdef _WIN32
	return NULL;
#endif
}

static void iShowMessage(const char* caption, const char* message)
{
#ifdef _WIN32
	MessageBox(NULL, message, caption, MB_OK);
#endif
}

static uint32 iSDLInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return 1;
	}

	sSDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!sSDLWindow)
	{
		return 1;
	}

	sShouldQuit = false;

	return 0;
}

static void iSDLExit()
{
	SDL_DestroyWindow(sSDLWindow);
	SDL_Quit();
}

static void iSDLUpdate()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			sShouldQuit = true;
		}
	}
}

STUB static void TRCInit()
{

}

void iSystemInit(uint32 options)
{
	if (iSDLInit() != 0)
	{
		iShowMessage("Fatal Error", "Could not initalize SDL!");
		exit(1);
	}

	xDebugInit();
	xMemInit();
	iFileInit();
	iTimeInit();
	xPadInit();
	xSndInit();
	TRCInit();
	RenderWareInit();
	xMathInit();
	xMath3Init();
}

void iSystemExit()
{
	xDebugExit();
	xMathExit();
	RenderWareExit();
	xSndExit();
	xPadKill();
	iFileExit();
	iTimeExit();
	xMemExit();
	iSDLExit();
}

void iSystemUpdate()
{
	iSDLUpdate();
}

bool iSystemShouldQuit()
{
	return sShouldQuit;
}

static void RenderWareDebugHandler(RwDebugType type, const RwChar* string)
{
	printf("%s\n", string);

#ifdef _MSC_VER
	if (type == rwDEBUGERROR)
	{
		__debugbreak();
	}
#endif
}

static uint32 RWAttachPlugins()
{
	if (!RpWorldPluginAttach())
	{
		return 1;
	}

	if (!RpCollisionPluginAttach())
	{
		return 1;
	}

	if (!RpSkinPluginAttach())
	{
		return 1;
	}

	if (!RpHAnimPluginAttach())
	{
		return 1;
	}

	if (!RpMatFXPluginAttach())
	{
		return 1;
	}

	if (!RpUserDataPluginAttach())
	{
		return 1;
	}

	if (!RpPTankPluginAttach())
	{
		return 1;
	}

	return 0;
}

static uint32 RenderWareInit()
{
	if (!RwEngineInit(psGetMemoryFunctions(), 0, RW_ARENA_SIZE))
	{
		return 1;
	}

	RwResourcesSetArenaSize(RW_ARENA_SIZE);

	RwDebugSetHandler(RenderWareDebugHandler);

	if (RWAttachPlugins() != 0)
	{
		return 1;
	}

	RwEngineOpenParams openParams;

#ifdef _WIN32
	openParams.displayID = GetActiveWindow();
#endif

	if (!RwEngineOpen(&openParams))
	{
		RwEngineTerm();
		return 1;
	}

	RwEngineGetVideoModeInfo(&sVideoMode, RwEngineGetCurrentVideoMode());

	if (!RwEngineStart())
	{
		RwEngineClose();
		RwEngineTerm();
		return 1;
	}

	RwTextureSetReadCallBack(TextureRead);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);

	xShadowInit();
	xFXInit();

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	return 0;
}

static void RenderWareExit()
{
	RwEngineStop();
	RwEngineClose();
	RwEngineTerm();
}

STUB static RwTexture* TextureRead(const char* name, const char* maskName)
{
	return NULL;
}