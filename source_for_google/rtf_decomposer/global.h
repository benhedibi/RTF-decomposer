#pragma once

typedef signed char         int8;
typedef unsigned char       uchar;
typedef unsigned char       uint8;
typedef unsigned char       byte;
typedef unsigned char       boolean;
typedef signed short        int16;
typedef unsigned short      ushort;
typedef unsigned short      uint16;
typedef unsigned short      word;
typedef signed long         int32;
typedef unsigned int        uint;
typedef unsigned long       uint32;
typedef unsigned long       ulong;
typedef unsigned long       dword;
typedef long long           longlong;
typedef long long           long64;
typedef signed long long    int64;
typedef unsigned long long  ulonglong;
typedef unsigned long long  dwordlong;
typedef unsigned long long  ulong64;
typedef unsigned long long  dword64;
typedef unsigned long long  uint64;
typedef unsigned long long  qword;

typedef char      *pchar;
typedef int8      *pint8;
typedef uchar     *puchar, *puint8, *pbyte;
typedef boolean   *pboolean;
typedef short     *pshort;
typedef int16     *pint16;
typedef ushort    *pushort, *puint16, *pword;
typedef int       *pint;
typedef int32     *pint32;
typedef uint      *puint, *puint32;
typedef long      *plong;
typedef ulong     *pulong, *pdword;
typedef longlong  *plonglong, *plong64;
typedef int64     *pint64;
typedef ulonglong *pulonglong, *pdwordlong, *pulong64, *pdword64, *puint64, *pqword;
typedef void *opaque_ptr;

typedef puchar *p_puchar;

#define true  1
#define false 0

#ifdef _WIN64
typedef int64 signed_int;
typedef uint64 unsigned_int;
typedef double float_int;
#else
typedef int32 signed_int;
typedef uint32 unsigned_int;
typedef float float_int;
#endif

typedef signed_int *psigned_int;
typedef unsigned_int *punsigned_int;

#define IO_ERROR                    ((unsigned_int)-1)
#define API_ERROR                   ((unsigned_int)-2)
#define MEM_ALLOC_ERROR             ((unsigned_int)-3)
#define RTF_INVALID_SIGNATURE_ERROR ((unsigned_int)-4)
#define BUFFER_TOO_SMALL_ERROR      ((unsigned_int)-5)
#define PATH_TOO_LONG_ERROR         ((unsigned_int)-6)
#define SOURCE_FILE_ERROR           ((unsigned_int)-7)
#define TARGET_FOLDER_ERROR         ((unsigned_int)-8)
#define INVALID_PARAM_ERROR         ((unsigned_int)-9)
#define FILE_TOO_BIG_ERROR          ((unsigned_int)-10)
#define UNPACK_ERROR                ((unsigned_int)-11)
#define EMPTY_FILE_ERROR            ((unsigned_int)-12)
#define NO_DATA_ERROR               ((unsigned_int)-13)


#define BSWAP(x) (((x>>24)&0x000000FFUL) | ((x<<24)&0xFF000000UL) | ((x >> 8) & 0x0000FF00UL) | ((x << 8) & 0x00FF0000UL))

#define bswap32(x) ((uint32)((((uint32) (x) & 0xff000000) >> 24) | (((uint32)(x) & 0x00ff0000) >> 8) | (((uint32)(x) & 0x0000ff00) << 8) | (((uint32)(x) & 0x000000ff) << 24)))

#define MACRO_MAX_VALUE(a,b) ((a) > (b) ? (a) : (b)) // a ^ ((a ^ b) & -(a < b))
#define MACRO_MIN_VALUE(a,b) ((a) < (b) ? (a) : (b)) // b ^ ((a ^ b) & -(a < b))

#define CHECK_ATTRIBUTE(source,attribute) (((source) & (attribute)) == (attribute))
#define TEST_ATTRIBUTE(source,attribute) (((source) & (attribute)))
