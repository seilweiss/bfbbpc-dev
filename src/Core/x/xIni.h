#ifndef XINI_H
#define XINI_H

#include <types.h>

struct xIniValue
{
	char* tok;
	char* val;
};

struct xIniSection
{
	char* sec;
	int32 first;
	int32 count;
};

struct xIniFile
{
	int32 NumValues;
	int32 NumSections;
	xIniValue* Values;
	xIniSection* Sections;
	void* mem;
	char name[256];
	char pathname[256];
};

xIniFile* xIniParse(char* buf, int32 len);
void xIniDestroy(xIniFile* ini);
int32 xIniGetIndex(xIniFile* ini, char* tok);
int32 xIniGetInt(xIniFile* ini, char* tok, int32 def = 0);
float32 xIniGetFloat(xIniFile* ini, char* tok, float32 def = 0.0f);
char* xIniGetString(xIniFile* ini, char* tok, char* def = NULL);

#endif