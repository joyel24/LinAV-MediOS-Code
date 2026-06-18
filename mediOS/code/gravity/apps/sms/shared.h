#ifndef _SHARED_H_
#define _SHARED_H_

#define LSB_FIRST
#define ALIGN_DWORD

//#define __IRAM_DATA 
//#define __IRAM_CODE

#include <api.h>
#include <fs_io.h>
#include <sys_def/string.h>


//#include <string.h>
//#include <math.h>

#define VERSION     "0.9.4a"

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

#include "types.h"
#include "cpu/z80.h"
#include "sms.h"
#include "vdp.h"
#include "sound/emu2413.h"
#include "render.h"
#include "sound/sn76496.h"
#include "system.h"

#endif /* _SHARED_H_ */
