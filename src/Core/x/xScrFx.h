#ifndef XSCRFX_H
#define XSCRFX_H

#include "iColor.h"

#include <rwcore.h>

void xScrFxInit();
void xScrFxUpdate(RwCamera* cam, float32 dt);
void xScrFxRender(RwCamera*);
void xScrFxDrawScreenSizeRectangle();
void xScrFxFade(iColor* base, iColor* dest, float32 seconds, void(*callback)(), bool32 hold);
void xScrFxLetterboxReset();
void xScrFxLetterBoxSetSize(float32 size);
void xScrFxLetterBoxSetAlpha(uint8 alpha);

#endif