#include "xString.h"

#include "xMath.h"

#include <string.h>

uint32 xStrHash(const char* str)
{
    uint32 value = 0;

    for (uint32 i = 0; str[i] != '\0'; i++)
    {
        char c = str[i] - (str[i] & (str[i] >> 1) & 0x20);
        value = c + value * 131;
    }

    return value;
}

uint32 xStrHash(const char* s, ulong32 size)
{
    uint32 value = 0;

    for (uint32 i = 0; i < size && s[i] != '\0'; i++)
    {
        char c = s[i] - (s[i] & (s[i] >> 1) & 0x20);
        value = c + value * 131;
    }

    return value;
}

char* xStrTok(char* string, const char* control, char** nextoken)
{
    uint8* str;
    uint8* ctrl;
    uint8 map[32];
    int32 count;

    map[0] = 0;
    map[1] = 0;
    map[2] = 0;
    map[3] = 0;
    map[4] = 0;
    map[5] = 0;
    map[6] = 0;
    map[7] = 0;
    map[8] = 0;
    map[9] = 0;
    map[10] = 0;
    map[11] = 0;
    map[12] = 0;
    map[13] = 0;
    map[14] = 0;
    map[15] = 0;
    map[16] = 0;
    map[17] = 0;
    map[18] = 0;
    map[19] = 0;
    map[20] = 0;
    map[21] = 0;
    map[22] = 0;
    map[23] = 0;
    map[24] = 0;
    map[25] = 0;
    map[26] = 0;
    map[27] = 0;
    map[28] = 0;
    map[29] = 0;
    map[30] = 0;
    map[31] = 0;

    ctrl = (uint8*)control;

    do
    {
        map[*ctrl >> 3] |= 1 << (*ctrl & 0x7);
    } while (*ctrl++ != '\0');

    str = (string) ? (uint8*)string : (uint8*)*nextoken;

    while (map[(*str >> 3) & 0x1F] & (1 << (*str & 0x7)) && *str != '\0')
    {
        str++;
    }

    string = (char*)str;

    while (*str != '\0')
    {
        if (map[(*str >> 3) & 0x1F] & (1 << (*str & 0x7)))
        {
            *str = '\0';
            str++;
            break;
        }

        str++;
    }

    *nextoken = (char*)str;

    if (string == (char*)str)
    {
        string = NULL;
    }

    return string;
}

STUB int32 xStricmp(const char* string1, const char* string2)
{
#if 1
    return stricmp(string1, string2);
#endif
}

char* xStrupr(char* string)
{
    char* p = string;

    while (*p != '\0')
    {
        *p = (*p >= 'a' && *p <= 'z' ? *p - 32 : *p);

        p++;
    }

    return string;
}

int32 xStrParseFloatList(float32* dest, const char* strbuf, int32 max)
{
    char* str = (char*)strbuf;
    int32 index;
    int32 digits;
    bool32 negate;
    char* numstart;
    char savech;

    if (!str)
    {
        return 0;
    }

    for (index = 0; *str != '\0' && index < max; index++)
    {
        while (*str == '\t' ||
            *str == ' ' ||
            *str == '[' ||
            *str == ']' ||
            *str == '{' ||
            *str == '}' ||
            *str == '(' ||
            *str == ')' ||
            *str == '+' ||
            *str == ',' ||
            *str == ':' ||
            *str == ';')
        {
            str++;
        }

        if (*str == '\0')
        {
            return index;
        }

        if (*str == '-')
        {
            negate = TRUE;
            str++;

            while (*str == '\t' || *str == ' ')
            {
                str++;
            }
        }
        else
        {
            negate = FALSE;
        }

        numstart = str;
        digits = 0;

        while ((*str >= '0' && *str <= '9') ||
            *str == '.' ||
            *str == 'E' ||
            *str == 'e' ||
            *str == 'f')
        {
            if (*str >= '0' && *str <= '9')
            {
                digits++;
            }

            str++;
        }

        if (digits == 0)
        {
            return index;
        }

        savech = *str;

        *str = '\0';
        *dest = xatof(numstart);

        if (negate)
        {
            *dest = -*dest;
        }

        *str = savech;
        dest++;
    }

    return index;
}