#ifndef XHUDMODEL_H
#define XHUDMODEL_H

#include "xHud.h"
#include "xModel.h"

namespace xhud {
	struct model_widget : widget
	{
		uint32 mid;
		xModelInstance* model;

		static void load(xBase& data, xDynAsset& asset, ulong32);
	};
}

#endif