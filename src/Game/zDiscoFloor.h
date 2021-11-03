#ifndef ZDISCOFLOOR_H
#define ZDISCOFLOOR_H

#include "xBase.h"
#include "zEntSimpleObj.h"

struct z_disco_floor_asset : xBaseAsset
{
	uint32 flags;
	struct
	{
		float32 transition;
		float32 state;
	} interval;
	struct
	{
		uint32 off;
		uint32 transition;
		uint32 on;
	} prefix_offset;
	uint32 state_mask_size;
	uint32 states_offset;
	uint32 states_size;
};

struct z_disco_floor : xBase
{
	struct tile_data
	{
		bool culled;
		zEntSimpleObj* ent;
		xSphere sphere;
	};

	struct
	{
		bool enabled;
		bool forward;
		bool culled;
		bool glow_culled;
	} flag;
	z_disco_floor_asset* asset;
	uint8** state_masks;
	uint8* active_state_mask;
	uint8* next_state_mask;
	char* prefix[3];
	tile_data* tiles[3];
	uint32 tiles_size;
	uint32 min_state;
	uint32 max_state;
	uint32 state;
	uint32 next_state;
	uint32 state_counter;
	float32 transition_delay;
	float32 state_delay;
	float32 transition_time;
	float32 state_time;
	xSphere bound;
	float32 pulse_time;
	float32 pulse_glow[3];
	float32 cull_dist_glow;
	float32 cull_dist_update;
	float32 glow_fade;
	float32 sound_delay;
	int32 curr_note;

	static void init();
	static void post_setup();
	static void destroy();
	static void init(void* ent, void* asset);

	void setup();
};

#endif