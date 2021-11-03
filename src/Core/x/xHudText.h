#ifndef XHUDTEXT_H
#define XHUDTEXT_H

#include "xHud.h"
#include "xFont.h"

namespace xhud {
	struct text_widget : widget
	{
		char text[128];
		xtextbox tb;

		static void load(xBase& data, xDynAsset& asset, ulong32);
	};
}

#endif