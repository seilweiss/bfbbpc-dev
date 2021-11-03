#ifndef IFILE_H
#define IFILE_H

#include <types.h>

#ifdef WIN32
#include <windows.h>
#endif

typedef struct tag_iFile
{
#ifdef PS2
	uint32 flags;
	char path[128];
	int32 fd;
	int32 offset;
	int32 length;
#endif

#ifdef _WIN32
	uint32 flags;
	char path[256];
	HANDLE handle;
	OVERLAPPED overlapped;
	int32 asynckey;
	int32 offset;
	int32 length;
#endif
} iFile;

enum IFILE_READSECTOR_STATUS
{
	IFILE_RDSTAT_NOOP,
	IFILE_RDSTAT_INPROG,
	IFILE_RDSTAT_DONE,
	IFILE_RDSTAT_FAIL,
	IFILE_RDSTAT_QUEUED,
	IFILE_RDSTAT_EXPIRED
};

#define IFILE_OPEN_READ 0x1
#define IFILE_OPEN_WRITE 0x2
#define IFILE_OPEN_ABSPATH 0x4

#define IFILE_SEEK_SET 0
#define IFILE_SEEK_CUR 1
#define IFILE_SEEK_END 2

#ifdef WIN32
#define IFILE_ISOPEN 0x1
#endif

struct tag_xFile;

typedef void(*iFileReadCallBack)(tag_xFile* file);

void iFileInit();
void iFileExit();
uint32* iFileLoad(char* name, uint32* buffer, uint32* size);
uint32 iFileOpen(const char* name, int32 flags, tag_xFile* file);
int32 iFileSeek(tag_xFile* file, int32 offset, int32 whence);
uint32 iFileRead(tag_xFile* file, void* buf, uint32 size);
int32 iFileReadAsync(tag_xFile* file, void* buf, uint32 aSize, iFileReadCallBack callback, int32 priority);
IFILE_READSECTOR_STATUS iFileReadAsyncStatus(int32 key, int32* amtToFar);
uint32 iFileClose(tag_xFile* file);
uint32 iFileGetSize(tag_xFile* file);
void iFileReadStop();
void iFileFullPath(const char* relname, char* fullname);
void iFileSetPath(char* path);
uint32 iFileFind(const char* name, tag_xFile* file);
void iFileGetInfo(tag_xFile* file, uint32* addr, uint32* length);

inline void iFileAsyncService()
{
}

#endif