#ifndef XJSP_H
#define XJSP_H

#include "xClumpColl.h"

#include <rwcore.h>
#include <rpworld.h>

struct xJSPNodeInfo
{
	int32 originalMatIndex;
	int32 nodeFlags;
};

struct xJSPHeader
{
	char idtag[4];
	uint32 version;
	uint32 jspNodeCount;
	RpClump* clump;
	xClumpCollBSPTree* colltree;
	xJSPNodeInfo* jspNodeList;

#ifdef GAMECUBE
	uint32 stripVecCount;
	RwV3d* stripVecList;
#endif
};

void xJSP_MultiStreamRead(void* data, uint32 size, xJSPHeader** jsp);
void xJSP_Destroy(xJSPHeader* jsp);

#endif