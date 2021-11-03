#include "xutil.h"

#include <ctype.h>

STUB int32 xUtilStartup()
{
	return 0;
}

STUB int32 xUtilShutdown()
{
	return 0;
}

const char* xUtil_idtag2string(uint32 srctag, int32 bufidx)
{
    static char buf[6][10] = {};

    uint32 tag = srctag;
    char* strptr;
    char* uc = (char*)&tag;

    if (bufidx < 0 || bufidx >= 7)
    {
        strptr = buf[0];
    }
    else
    {
        strptr = buf[bufidx];
    }

    {
        // convert tag to system endian

        int32 l = 1;

        if ((int32)((char*)&l)[3] != 0)
        {
            char t;

            t = uc[0];
            uc[0] = uc[3];
            uc[3] = t;

            t = uc[1];
            uc[1] = uc[2];
            uc[2] = t;
        }
    }

    switch (bufidx)
    {
    case 4:
    case 5:
        strptr[0] = isprint(uc[0]) ? uc[0] : '?';
        strptr[1] = isprint(uc[1]) ? uc[1] : '?';
        strptr[2] = isprint(uc[2]) ? uc[2] : '?';
        strptr[3] = isprint(uc[3]) ? uc[3] : '?';
        break;
    case 6:
    default:
        strptr[0] = isprint(uc[3]) ? uc[3] : '?';
        strptr[1] = isprint(uc[2]) ? uc[2] : '?';
        strptr[2] = isprint(uc[1]) ? uc[1] : '?';
        strptr[3] = isprint(uc[0]) ? uc[0] : '?';
        break;
    }

    strptr[4] = '\0';

    if (bufidx == 6)
    {
        strptr[4] = '/';
        strptr[5] = isprint(uc[0]) ? uc[0] : '?';
        strptr[6] = isprint(uc[1]) ? uc[1] : '?';
        strptr[7] = isprint(uc[2]) ? uc[2] : '?';
        strptr[8] = isprint(uc[3]) ? uc[3] : '?';
        strptr[9] = '\0';
    }

    return strptr;
}