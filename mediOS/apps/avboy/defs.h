


#ifndef __DEFS_H__
#define __DEFS_H__

#include <api.h>

//#define __IRAM_CODE    __attribute__ ((section(".iram_code")))
//#define __IRAM_DATA    __attribute__ ((section(".iram_data")))
//#define __IRAM_CODE
//#define __IRAM_DATA

#define IS_LITTLE_ENDIAN


#ifdef IS_LITTLE_ENDIAN
#define LO 0
#define HI 1
#else
#define LO 1
#define HI 0
#endif



typedef unsigned char byte;

typedef unsigned char un8;
typedef unsigned short un16;
typedef unsigned int un32;

typedef signed char n8;
typedef signed short n16;
typedef signed int n32;

typedef un16 word;
typedef word addr;





#endif

