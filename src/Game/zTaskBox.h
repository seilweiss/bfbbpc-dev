#ifndef ZTASKBOX_H
#define ZTASKBOX_H

#include "xDynAsset.h"

struct ztaskbox : xBase
{
	struct asset_type : xDynAsset
	{
		bool persistent : 8;
		bool loop : 8;
		bool enable : 8;
		bool retry : 8;
		uint32 talk_box;
		uint32 next_task;
		uint32 stages[6];
	};

	enum state_enum
	{
		STATE_INVALID = -1,
		STATE_BEGIN = 0,
		STATE_DESCRIPTION,
		STATE_REMINDER,
		STATE_SUCCESS,
		STATE_FAILURE,
		STATE_END,
		MAX_STATE
	};

	struct callback
	{
	};

	struct
	{
		bool enabled : 8;
		uint32 dummy : 24;
	} flag;
	asset_type* asset;
	state_enum state;
	callback* cb;
	ztaskbox* current;

	static void init();
	static void load(xBase& data, xDynAsset& asset, ulong32);
};

#endif