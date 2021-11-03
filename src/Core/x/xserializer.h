#ifndef XSERIALIZER_H
#define XSERIALIZER_H

#include <types.h>

typedef struct st_SERIAL_PERCID_SIZE
{
	uint32 idtag;
	int32 needsize;
} SERIAL_PERCID_SIZE;

typedef struct st_SERIAL_CLIENTINFO
{
	uint32 idtag;
	int32* membuf;
	int32 trueoff;
	int32 actsize;
} SERIAL_CLIENTINFO;

struct xSerial
{
	uint32 idtag;
	int32 baseoff;
	st_SERIAL_CLIENTINFO* ctxtdata;
	int32 warned;
	int32 curele;
	int32 bitidx;
	int32 bittally;

	int32 Write_b1(int32 bits);
	int32 Read_b1(int32* bits);
};

typedef int32(*xSerialTraverseCallback)(uint32 clientID, xSerial* xser);

int32 xSerialStartup(int32 count, SERIAL_PERCID_SIZE* sizeinfo);
int32 xSerialShutdown();
void xSerialTraverse(xSerialTraverseCallback func);
void xSerialWipeMainBuffer();

#endif