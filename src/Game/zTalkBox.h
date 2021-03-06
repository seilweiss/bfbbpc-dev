#ifndef ZTALKBOX_H
#define ZTALKBOX_H

#include "zTextBox.h"
#include "xIni.h"

struct zNPCCommon;

struct ztalkbox
{
	struct asset_type : xDynAsset
	{
		uint32 dialog_box;
		uint32 prompt_box;
		uint32 quit_box;
		bool trap : 8;
		bool pause : 8;
		bool allow_quit : 8;
		bool trigger_pads : 8;
		bool page : 8;
		bool show : 8;
		bool hide : 8;
		bool audio_effect : 8;
		uint32 teleport;
		struct
		{
			struct
			{
				bool time : 8;
				bool prompt : 8;
				bool sound : 8;
				bool event : 8;
			} type;
			float32 delay;
			int32 which_event;
		} auto_wait;
		struct
		{
			uint32 skip;
			uint32 noskip;
			uint32 quit;
			uint32 noquit;
			uint32 yesno;
		} prompt;
	};

	struct
	{
		bool visible : 1;
	} flag;
	asset_type* asset;
	ztextbox* dialog_box;
	ztextbox* prompt_box;
	ztextbox* quit_box;
	struct
	{
		char* skip;
		char* noskip;
		char* quit;
		char* noquit;
		char* yesno;
	} prompt;
	zNPCCommon* npc;

	static void load_settings(xIniFile& ini);
	static void init();
	static void load(xBase& data, xDynAsset& asset, ulong32);
};

#endif