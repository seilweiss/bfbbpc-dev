#ifndef XFONT_H
#define XFONT_H

#include "iColor.h"
#include "xMath2.h"
#include "xString.h"

struct xfont
{
	uint32 id;
	float32 width;
	float32 height;
	float32 space;
	iColor color;
	basic_rect<float32> clip;

	static void init();
};

struct xtextbox
{
	struct jot;

	struct callback
	{
		void(*render)(const jot&, const xtextbox&, float32, float32);
		void(*layout_update)(const jot&, xtextbox&, const xtextbox&);
		void(*render_update)(const jot&, xtextbox&, const xtextbox&);
	};

	struct split_tag
	{
		substr tag;
		substr name;
		substr action;
		substr value;
	};

	struct tag_type
	{
		substr name;
		void(*parse_tag)(jot&, const xtextbox&, const xtextbox&, const split_tag&);
		void(*reset_tag)(jot&, const xtextbox&, const xtextbox&, const split_tag&);
		void* context;
	};

	struct jot
	{
		substr s;

		struct
		{
			bool invisible : 1;
			bool ethereal : 1;
			bool merge : 1;
			bool word_break : 1;
			bool word_end : 1;
			bool line_break : 1;
			bool stop : 1;
			bool tab : 1;
			
			bool insert : 1;
			bool dynamic : 1;
			bool page_break : 1;
			bool stateful : 1;

			uint16 dummy : 4;
		} flag;

		uint16 context_size;
		void* context;
		basic_rect<float32> bounds;
		basic_rect<float32> render_bounds;
		const callback* cb;
		tag_type* tag;
	};

	xfont font;
	basic_rect<float32> bounds;
	uint32 flags;
	float32 line_space;
	float32 tab_stop;
	float32 left_indent;
	float32 right_indent;
	callback* cb;
	void* context;
	const char** texts;
	const ulong32* text_sizes;
	ulong32 texts_size;
	substr text;
	ulong32 text_hash;
};

#endif