#ifndef _SHARED_H_
#define _SHARED_H_

#define LSB_FIRST
//#define ALIGN_DWORD

#define ASM_CPU

#include "medios.h"
#include "aosms.h"

#ifndef strcasecmp
#define strcasecmp stricmp
#endif

#ifdef ASM_CPU
    #include "drz80/z80.h"
#else    
    #include "cpu/c_z80.h"
#endif

#include "types.h"
#include "sms.h"
#include "vdp.h"
#include "render.h"
#include "system.h"

#define VERSION     "0.9.4a"

extern tDspCom * dspCom;

#endif /* _SHARED_H_ */
