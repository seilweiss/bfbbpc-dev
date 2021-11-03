#include "iFile.h"

#include "xFile.h"

#ifdef _WIN32
#include <windows.h>
#include <string.h>

struct AsyncRequest
{
    bool inUse;
    int32 id;
    void* buf;
    uint32 size;
    IFILE_READSECTOR_STATUS status;
    iFileReadCallBack callback;
    tag_xFile* file;
};

static char base_path[256];
static AsyncRequest requests[32];
#endif

void iFileInit()
{
#ifdef _WIN32
	//GetCurrentDirectory(256, base_path);
    strcpy(base_path, "C:\\heavyiron\\bfbb\\pc");
#endif
}

void iFileExit()
{
}

uint32* iFileLoad(char* name, uint32* buffer, uint32* size)
{
    tag_xFile file;
    int32 fileSize;
    int32 alignedSize;
    char path[256];

    iFileFullPath(name, path);
    iFileOpen(path, IFILE_OPEN_READ | IFILE_OPEN_ABSPATH, &file);

    fileSize = iFileGetSize(&file);
    alignedSize = (fileSize + 0x1f) & ~0x1f;

    if (!buffer)
    {
        buffer = (uint32*)malloc(alignedSize);
    }

    iFileRead(&file, buffer, alignedSize);

    if (size)
    {
        *size = alignedSize;
    }

    iFileClose(&file);

    return buffer;
}

uint32 iFileOpen(const char* name, int32 flags, tag_xFile* file)
{
    iFile* ps = &file->ps;

    if (flags & IFILE_OPEN_ABSPATH)
    {
        strcpy(ps->path, name);
    }
    else
    {
        iFileFullPath(name, ps->path);
    }

#ifdef _WIN32
    DWORD access = 0;

    if (flags & IFILE_OPEN_READ)
    {
        access |= GENERIC_READ;
    }

    if (flags & IFILE_OPEN_WRITE)
    {
        access |= GENERIC_WRITE;
    }

    ps->handle = CreateFile(ps->path, access, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if (ps->handle == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    ps->flags = IFILE_ISOPEN;
    ps->length = GetFileSize(ps->handle, NULL);
    ps->offset = 0;
#endif

    return 0;
}

int32 iFileSeek(tag_xFile* file, int32 offset, int32 whence)
{
    iFile* ps = &file->ps;

#ifdef _WIN32
    switch (whence)
    {
    case IFILE_SEEK_SET:
        ps->offset = offset;
        break;
    case IFILE_SEEK_CUR:
        ps->offset += offset;
        break;
    case IFILE_SEEK_END:
        ps->offset = ps->length - offset;
        break;
    }
#endif

    return ps->offset;
}

uint32 iFileRead(tag_xFile* file, void* buf, uint32 size)
{
    iFile* ps = &file->ps;

#ifdef _WIN32
    DWORD num = -1;
    BOOL status;

    ps->overlapped.Internal = 0;
    ps->overlapped.InternalHigh = 0;
    ps->overlapped.Offset = ps->offset;
    ps->overlapped.OffsetHigh = 0;
    ps->overlapped.hEvent = 0;

    status = ReadFile(ps->handle, buf, size, NULL, &ps->overlapped);

    if (status || GetLastError() == ERROR_IO_PENDING)
    {
        while (!GetOverlappedResult(ps->handle, &ps->overlapped, &num, TRUE))
        {
            DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING)
            {
                num = -1;
                break;
            }
        }
    }

    return num;
#else
    return -1;
#endif
}

#ifdef _WIN32
static void __stdcall async_cb(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred,
    LPOVERLAPPED lpOverlapped)
{
    AsyncRequest* request = &requests[(int32)lpOverlapped->hEvent];

    request->status = IFILE_RDSTAT_DONE;
}

static DWORD WINAPI iFileReadAsyncThread(LPVOID lpParam)
{
    AsyncRequest* request = (AsyncRequest*)lpParam;
    iFile* ps = &request->file->ps;
    DWORD bytesRead = 0;

    if (!ReadFileEx(ps->handle, request->buf, request->size, &ps->overlapped, async_cb))
    {
        request->inUse = false;
        return 1;
    }

    request->status = IFILE_RDSTAT_INPROG;

    while (!GetOverlappedResultEx(ps->handle, &ps->overlapped, &bytesRead, INFINITE, TRUE))
    {
        if (GetLastError() != WAIT_IO_COMPLETION)
        {
            request->inUse = false;
            return 1;
        }
    }

    return 0;
}
#endif

int32 iFileReadAsync(tag_xFile* file, void* buf, uint32 aSize, iFileReadCallBack callback, int32 priority)
{
    iFile* ps = &file->ps;
    int32 id = -1;

#ifdef _WIN32
    for (int32 i = 0; i < 32; i++)
    {
        AsyncRequest* request = &requests[i];

        if (!request->inUse || request->status == IFILE_RDSTAT_DONE)
        {
            id = i;

            request->inUse = true;
            request->id = id;
            request->buf = buf;
            request->size = aSize;
            request->status = IFILE_RDSTAT_QUEUED;
            request->callback = callback;
            request->file = file;

            ps->asynckey = id;

            ps->overlapped.Internal = 0;
            ps->overlapped.InternalHigh = 0;
            ps->overlapped.Offset = ps->offset;
            ps->overlapped.OffsetHigh = 0;
            ps->overlapped.hEvent = (HANDLE)id;

            HANDLE thread = CreateThread(NULL, 0, iFileReadAsyncThread, request, 0, NULL);

            if (!thread)
            {
                request->inUse = false;
                id = -1;
            }

            break;
        }
    }
#endif

    return id;
}

IFILE_READSECTOR_STATUS iFileReadAsyncStatus(int32 key, int32* amtToFar)
{
#ifdef _WIN32
    AsyncRequest* request = &requests[key];

    if (request->id != key)
    {
        return IFILE_RDSTAT_EXPIRED;
    }

    if (amtToFar)
    {
        *amtToFar = request->size;
    }

    return request->status;
#else
    return IFILE_RDSTAT_NOOP;
#endif
}

uint32 iFileClose(tag_xFile* file)
{
#ifdef _WIN32
    if (!CloseHandle(file->ps.handle))
    {
        return 1;
    }

    file->ps.flags = 0;

    return 0;
#else
    return 0;
#endif
}

uint32 iFileGetSize(tag_xFile* file)
{
#ifdef _WIN32
    return file->ps.length;
#else
    return 0;
#endif
}

void iFileReadStop()
{
#ifdef _WIN32
#endif
}

void iFileFullPath(const char* relname, char* fullname)
{
#ifdef _WIN32
    strcpy(fullname, base_path);
    strcat(fullname, "\\");
    strcat(fullname, relname);
#else
    strcpy(fullname, relname);
#endif
}

void iFileSetPath(char* path)
{
#ifdef _WIN32
#if 0
    strcpy(base_path, path);
#endif
#endif
}

uint32 iFileFind(const char* name, tag_xFile* file)
{
#ifdef _WIN32
    return iFileOpen(name, 0, file);
#else
    return 1;
#endif
}

void iFileGetInfo(tag_xFile* file, uint32* offset, uint32* length)
{
#ifdef _WIN32
    if (offset)
    {
        *offset = file->ps.offset;
    }

    if (length)
    {
        *length = file->ps.length;
    }
#endif
}