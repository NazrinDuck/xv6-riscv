#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

#ifndef __MKFS__

typedef int cpuid_t;
typedef int pid_t;

typedef uint64 tick_t;
typedef uint64 time_t;

#endif // !__MKFS__
       //
#endif // !__TYPES_H
