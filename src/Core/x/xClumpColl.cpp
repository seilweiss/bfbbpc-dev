#include "xClumpColl.h"

UNCHECKED xClumpCollBSPTree* xClumpColl_StaticBufferInit(void* data, uint32)
{
	uint32* header = (uint32*)data;
	uint32 numBranchNodes = header[1];
	uint32 numTriangles = header[2];
	xClumpCollBSPTree* tree = (xClumpCollBSPTree*)RwMalloc(sizeof(xClumpCollBSPTree), rwMEMHINTDUR_GLOBAL);

	if (numBranchNodes != 0)
	{
		tree->branchNodes = (xClumpCollBSPBranchNode*)(header + 3);
		tree->triangles = (xClumpCollBSPTriangle*)(tree->branchNodes + numBranchNodes);
	}
	else
	{
		tree->branchNodes = NULL;
		tree->triangles = (xClumpCollBSPTriangle*)(header + 3);
	}

	tree->numBranchNodes = numBranchNodes;
	tree->numTriangles = numTriangles;

	return tree;
}

STUB void xClumpColl_InstancePointers(xClumpCollBSPTree* tree, RpClump* clump)
{
#ifdef PS2
	// WIP
#endif
}