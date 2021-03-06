#ifndef ZUIFONT_H
#define ZUIFONT_H

#include "zUI.h"

struct zUIFontAsset : zUIAsset
{
	uint16 uiFontFlags;
	uint8 mode;
	uint8 fontID;
	uint32 textAssetID;
	uint8 bcolor[4];
	uint8 color[4];
	uint16 inset[4];
	uint16 space[2];
	uint16 cdim[2];
	uint32 max_height;
};

struct zUIFont : zUI
{
	zUIFontAsset* fasset;
	uint16 uiFontFlags;
	uint16 uiFontHackFlags;
	uint32 text_id;
	float32 yscroll;
	uint32 text_index;
	char* text;
};

void zUIFontInit();
void zUIFont_Init(void* ent, void* asset);

#endif