#include "xJSP.h"

#include "zMain.h"

#include <string.h>

struct __rwMark
{
	RwUInt32 type;
	RwUInt32 length;
	RwUInt32 libraryID;
};

UNCHECKED static RpAtomic* ListAtomicCB(RpAtomic* atomic, void* data)
{
	RpAtomic*** aList = (RpAtomic***)data;

	**aList = atomic;
	(*aList)++;

	return atomic;
}

UNCHECKED STUB void xJSP_MultiStreamRead(void* data, uint32 size, xJSPHeader** jsp)
{
	if (!*jsp)
	{
		*jsp = (xJSPHeader*)RwMalloc(sizeof(xJSPHeader), rwMEMHINTDUR_GLOBAL);
		memset(*jsp, 0, sizeof(xJSPHeader));
	}

	uint32 i;
	RpClump* clump;
	xClumpCollBSPTree* colltree;
	xJSPHeader* hdr = *jsp;
	__rwMark mark;
	__rwMark* mp = &mark;
	xJSPHeader* tmphdr;

	memmove(mp, data, sizeof(__rwMark));
	RwMemNative32(mp, sizeof(__rwMark));

	if (mark.type == 0xBEEF01)
	{
		data = (char*)data + sizeof(__rwMark);
		colltree = xClumpColl_StaticBufferInit(data, mark.length);

		data = (char*)data + mark.length;
		size -= mark.length + sizeof(__rwMark);

		memmove(mp, data, sizeof(__rwMark));
		RwMemNative32(mp, sizeof(__rwMark));

		data = (char*)data + sizeof(__rwMark);
		strncpy(hdr->idtag, (char*)data, sizeof(hdr->idtag));

		hdr->version = ((uint32*)data)[1];
		hdr->jspNodeCount = ((uint32*)data)[2];
		hdr->colltree = colltree;
		hdr->jspNodeList = (xJSPNodeInfo*)((uint32*)data + 6);

		size -= mark.length + sizeof(__rwMark);

		for (i = 0; i < colltree->numTriangles; i++)
		{
			colltree->triangles[i].matIndex = hdr->jspNodeList[(hdr->jspNodeCount - 1) - colltree->triangles[i].v.i.atomIndex].originalMatIndex;
		}

#ifdef GAMECUBE
		if (size >= sizeof(__rwMark))
		{
			// WIP
		}
#endif
	}
	else
	{
		RwMemory rwmem;
		rwmem.start = (RwUInt8*)data;
		rwmem.length = size;

		RwStream* stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &rwmem);
		RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL);
		clump = RpClumpStreamRead(stream);
		RwStreamClose(stream, NULL);

		if (!hdr->clump)
		{
			hdr->clump = clump;
		}
		else
		{
			int32 i;
			int32 atomCount = RpClumpGetNumAtomics(clump);

			if (atomCount != 0)
			{
				RpAtomic** atomList = (RpAtomic**)RwMalloc(atomCount * sizeof(RpAtomic*), rwMEMHINTDUR_FUNCTION);
				RpAtomic** atomCurr = atomList;

				RpClumpForAllAtomics(clump, ListAtomicCB, &atomCurr);

				for (i = atomCount - 1; i >= 0; i--)
				{
					RpClumpRemoveAtomic(clump, atomList[i]);
					RpClumpAddAtomic(hdr->clump, atomList[i]);
					RpAtomicSetFrame(atomList[i], RpClumpGetFrame(hdr->clump));
				}

				RwFree(atomList);
			}
			
			RpClumpDestroy(clump);
		}
	}
}

UNCHECKED STUB void xJSP_Destroy(xJSPHeader* jsp)
{
	if (globals.sceneCur->env->geom->jsp == jsp &&
		globals.sceneCur->env->geom->world)
	{
		RpWorldDestroy(globals.sceneCur->env->geom->world);
		globals.sceneCur->env->geom->world = NULL;
	}

	RpClumpDestroy(jsp->clump);
	RwFree(jsp->colltree);

#ifdef GAMECUBE
	// WIP
#endif

	RwFree(jsp);
}