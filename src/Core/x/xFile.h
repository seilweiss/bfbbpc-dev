#ifndef XFILE_H
#define XFILE_H

#include "iFile.h"

typedef struct tag_xFile
{
	char relname[32];
	iFile ps;
	void* user_data;
} xFile;

enum XFILE_READSECTOR_STATUS
{
	XFILE_RDSTAT_NOOP,
	XFILE_RDSTAT_INPROG,
	XFILE_RDSTAT_DONE,
	XFILE_RDSTAT_FAIL,
	XFILE_RDSTAT_QUEUED,
	XFILE_RDSTAT_EXPIRED
};

inline void xFileSetUserData(xFile* file, void* userdata)
{
	file->user_data = userdata;
}

inline XFILE_READSECTOR_STATUS xFileReadAsyncStatus(int32 key, int32* amtToFar)
{
	return (XFILE_READSECTOR_STATUS)iFileReadAsyncStatus(key, amtToFar);
}

#endif