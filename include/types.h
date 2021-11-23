#ifndef BFBB_TYPES_H
#define BFBB_TYPES_H

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

typedef long long32;
typedef unsigned long ulong32;

typedef int32 bool32;

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

#define STUB // work-in-progress
#define NONMATCH // not matching with decomp
#define UNCHECKED // not checked to match with decomp

#ifdef GAMECUBE
#define BIG_ENDIAN
#else
#define LITTLE_ENDIAN
#endif

#define ARRAYCOUNT(a) (sizeof(a) / sizeof(a[0]))

#endif