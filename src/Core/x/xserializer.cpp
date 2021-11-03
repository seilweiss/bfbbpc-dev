#include "xserializer.h"

STUB int32 xSerialStartup(int32 count, SERIAL_PERCID_SIZE* sizeinfo)
{
	return 0;
}

STUB int32 xSerialShutdown()
{
	return 0;
}

STUB void xSerialTraverse(xSerialTraverseCallback func)
{

}

STUB void xSerialWipeMainBuffer()
{

}

STUB int32 xSerial::Write_b1(int32 bits)
{
	return 0;
}

STUB int32 xSerial::Read_b1(int32* bits)
{
	return 0;
}