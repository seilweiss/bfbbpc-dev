#ifndef XSTRING_H
#define XSTRING_H

#include <types.h>

struct substr
{
	const char* text;
	ulong32 size;
};

uint32 xStrHash(const char* str);
uint32 xStrHash(const char* s, ulong32 size);
char* xStrTok(char* string, const char* control, char** nextoken);
int32 xStricmp(const char* string1, const char* string2);
char* xStrupr(char* string);
int32 xStrParseFloatList(float32* dest, const char* strbuf, int32 max);

#endif