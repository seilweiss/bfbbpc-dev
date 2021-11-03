#ifndef XHUD_H
#define XHUD_H

#include "xMath3.h"
#include "xDynAsset.h"

namespace xhud {
	struct widget;

	struct color32u
	{
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;
	};

	struct render_context
	{
		xVec3 loc;
		xVec3 size;
		xVec3 rot;
		float32 r;
		float32 g;
		float32 b;
		float32 a;
	};

	struct motive
	{
		float32* value;
		float32 delta;
		float32 start_delta;
		float32 max_offset;
		float32 offset;
		float32 accel;
		bool(*fp_update)(widget&, motive&, float32);
		void* context;
		uint8 inverse;
	};

	struct motive_node
	{
		motive m;
		motive_node* next;
	};

	struct asset : xDynAsset
	{
		xVec3 loc;
		xVec3 size;
	};

	struct widget
	{
		struct
		{
			bool visible;
			bool enabled;
		} flag;
		render_context rc;
		render_context start_rc;
		asset* a;
		enum
		{
			ACT_NONE,
			ACT_SHOW,
			ACT_HIDE,
			MAX_ACT
		} activity;
		motive_node* _motive_top;
		motive_node* _motive_temp;
		motive_node** _motive_temp_tail;
	};
}

#endif