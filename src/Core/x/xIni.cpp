#include "xIni.h"

#include "xString.h"

#include <cstring>
#include <rwcore.h>

static char* TrimWhitespace(char* str)
{
	while (*str == ' ' || *str == '\t')
	{
		str++;
	}

	if (*str == '\0')
	{
		return str;
	}

	char* end = str;
	end += strlen(str) - 1;

	while (*end == ' ' || *end == '\t')
	{
		end--;
	}

	*(end + 1) = '\0';

	return str;
}

xIniFile* xIniParse(char* buf, int32 len)
{
	int32 i;
	int32 ccr = 1;
	int32 clf = 1;
	int32 copen = 0;
	int32 lastCRLF = -1;
	char* c = buf;
	char* lastLine;
	char* line;
	char* ltoken;
	char* nextToken;
	xIniFile* ini;

	for (i = 0; i < len; i++)
	{
		switch (buf[i])
		{
		case '\n':
			lastCRLF = i;
			clf++;
			break;
		case '\r':
			lastCRLF = i;
			ccr++;
			break;
		case '[':
			copen++;
			break;
		}
	}

	if (clf > ccr)
	{
		ccr = clf;
	}

	ini = (xIniFile*)RwMalloc(
		sizeof(xIniFile) +
		ccr * sizeof(xIniValue) +
		copen * sizeof(xIniSection) +
		(len - lastCRLF),
		rwMEMHINTDUR_GLOBAL);

	ini->mem = NULL;
	ini->NumValues = 0;
	ini->NumSections = 0;
	ini->Values = (xIniValue*)(ini + 1);
	ini->Sections = (xIniSection*)(ini->Values + ccr);

	len -= lastCRLF + 1;
	lastLine = (char*)(ini->Sections + copen);

	strncpy(lastLine, buf + lastCRLF + 1, len);

	lastLine[len] = '\0';

	if (lastCRLF >= 0)
	{
		buf[lastCRLF] = '\0';
	}
	else
	{
		buf[0] = '\0';
	}

	line = xStrTok(buf, "\n\r", &nextToken);

	if (!line)
	{
		line = xStrTok(lastLine, "\n\r", &nextToken);
		lastLine = NULL;
	}

	while (line)
	{
		line = TrimWhitespace(line);

		if (*line != '#' && *line != '\0')
		{
			if (*line == '[')
			{
				ltoken = std::strstr(line, "]");

				if (ltoken)
				{
					*ltoken = '\0';
					ltoken = TrimWhitespace(line + 1);

					if (*ltoken != '\0')
					{
						ini->Sections[ini->NumSections].sec = ltoken;
						ini->Sections[ini->NumSections].first = ini->NumValues;
						ini->Sections[ini->NumSections].count = 0;
						ini->NumSections++;
					}
				}
			}
			else
			{
				ltoken = std::strstr(line, "=");

				if (ltoken)
				{
					*ltoken = '\0';
					line = TrimWhitespace(line);

					if (*line != '\0')
					{
						ltoken++;

						c = std::strstr(ltoken, "#");

						if (c)
						{
							*c = '\0';
						}

						ltoken = TrimWhitespace(ltoken);

						ini->Values[ini->NumValues].tok = line;
						ini->Values[ini->NumValues].val = ltoken;
						ini->NumValues++;

						if (ini->NumSections)
						{
							ini->Sections[ini->NumSections - 1].count++;
						}
					}
				}
			}
		}

		line = xStrTok(NULL, "\n\r", &nextToken);

		if (!line && lastLine)
		{
			line = xStrTok(lastLine, "\n\r", &nextToken);
			lastLine = NULL;
		}
	}

	return ini;
}

void xIniDestroy(xIniFile* ini)
{
	RwFree(ini->mem);
	RwFree(ini);
}

int32 xIniGetIndex(xIniFile* ini, char* tok)
{
	for (int32 i = 0; i < ini->NumValues; i++)
	{
		if (xStricmp(ini->Values[i].tok, tok) == 0)
		{
			return i;
		}
	}

	return -1;
}

int32 xIniGetInt(xIniFile* ini, char* tok, int32 def)
{
	int32 index = xIniGetIndex(ini, tok);

	if (index == -1)
	{
		return def;
	}

	return atoi(ini->Values[index].val);
}

float32 xIniGetFloat(xIniFile* ini, char* tok, float32 def)
{
	int32 index = xIniGetIndex(ini, tok);

	if (index == -1)
	{
		return def;
	}

	return atof(ini->Values[index].val);
}

char* xIniGetString(xIniFile* ini, char* tok, char* def)
{
	int32 index = xIniGetIndex(ini, tok);

	if (index == -1)
	{
		return def;
	}

	return ini->Values[index].val;
}