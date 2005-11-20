#ifndef _SHARED_H_
#define _SHARED_H_

#define LSB_FIRST
#define ALIGN_DWORD


//#include <math.h>

#include <api.h>
#include <fs_io.h>
#include <sys_def/string.h>

#ifdef _MSC_VER
#include <stdio.h>
#include <string.h>
#ifndef __inline__
#define __inline__ __inline
#endif
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#endif
//#undef __IRAM_CODE
//#undef __IRAM_DATA
//#define __IRAM_CODE    __attribute__ ((section(".iram_code")))
//#define __IRAM_DATA    __attribute__ ((section(".iram_data")))
//#define __IRAM_CODE
//#define __IRAM_DATA

#include "types.h"
#include "z80.h"
#include "sms.h"
#include "vdp.h"
#include "render.h"
#include "system.h"

#define VERSION     "0.9.4a"
/*
#define DSP_READY 		((volatile unsigned short *)0x40100)
#define DSP_STOP 		  ((volatile unsigned short *)0x40102)
#define PSG_ENABLED 	((volatile unsigned short *)0x40104)
#define PSG_DATA 		  ((volatile unsigned short *)0x40106)
#define PSG_MODIFIED 	((volatile unsigned short *)0x40108)
#define PSG_STEREO  	((volatile unsigned short *)0x4010a)
*/
#endif /* _SHARED_H_ */
