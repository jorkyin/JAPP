
#ifndef __RT_P2P_TYPES_H__
#define __RT_P2P_TYPES_H__

typedef int                RT_APP_INT32;
typedef unsigned int       RT_APP_UINT32;

typedef short              RT_APP_INT16;
typedef unsigned short     RT_APP_UINT16;

typedef char               RT_APP_CHAR;
typedef unsigned char      RT_APP_UCHAR;

typedef void 		       RT_APP_VOID;

typedef long               RT_APP_LONG;
typedef unsigned long      RT_APP_ULONG;
typedef long long          RT_APP_LONG64;
typedef unsigned long long RT_APP_ULONG64;

#if defined RT_TARGET_64
typedef unsigned long      RT_APP_UPTR;
#else
typedef unsigned int       RT_APP_UPTR;
#endif


#endif  //__RT_P2P_TYPES_H__
