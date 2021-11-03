#include "iPad.h"

#include "xPad.h"
#include "xTRC.h"

#include <SDL.h>

static SDL_Joystick* mJoysticks[4];

bool32 iPadInit()
{
	bool32 success = TRUE;

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0)
	{
		success = FALSE;
	}

	return success;
}

_tagxPad* iPadEnable(_tagxPad* pad, int16 port)
{
	pad->port = port;
	pad->slot = 0;
	pad->state = ePad_Enabled;

	gTrcPad[pad->port].state = TRC_PadInserted;

	mJoysticks[pad->port] = SDL_JoystickOpen(pad->port);

	pad->flags |= 0x3;
	pad->flags |= 0x4;

	return pad;
}

void iPadKill()
{
}