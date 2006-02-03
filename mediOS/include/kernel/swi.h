/*
*   include/kernel/swi.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SWI_H
#define __SWI_H

#include <kernel/errors.h>


///////////////////////////////////////////////////////
///////////////////// API SECTIONS ////////////////////
#define nAPI_GFX_section_code      1
#define nAPI_SND_section_code      2
#define nAPI_FILE_section_code     3
#define nAPI_DEVICE_section_code   4
#define nAPI_MEMORY_section_code   5
#define nAPI_DSP_section_code      6
#define nAPI_FLOAT_section_code    7

#define nAPI_GFX_section     (nAPI_GFX_section_code    << 8)
#define nAPI_SND_section     (nAPI_SND_section_code    << 8)
#define nAPI_FILE_section    (nAPI_FILE_section_code   << 8)
#define nAPI_DEVICE_section  (nAPI_DEVICE_section_code << 8)
#define nAPI_MEMORY_section  (nAPI_MEMORY_section_code << 8)
#define nAPI_DSP_section     (nAPI_DSP_section_code    << 8)
#define nAPI_FLOAT_section   (nAPI_FLOAT_section_code  << 8)
///////////////////// API SECTIONS ////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// MEMORY API /////////////////////
#define nAPI_MALLOC               (nAPI_MEMORY_section+0)
#define nAPI_FREE                 (nAPI_MEMORY_section+1)
#define nAPI_MEMAVAIL             (nAPI_MEMORY_section+2)
#define nAPI_REALLOC              (nAPI_MEMORY_section+3)

///////////////////////////////////////////////////////
////////////////////// FILE API ///////////////////////
#define nAPI_FILE                 (nAPI_FILE_section + 0)
#define nAPI_RUN_GRV              (nAPI_FILE_section + 10)
////////////////////// FILE API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// DEVICE API //////////////////////
#define nAPI_TIME                 (nAPI_DEVICE_section + 0x0)
#define nAPI_GET_TICK             (nAPI_DEVICE_section + 0x1)
#define nAPI_IO_INT_TIMER         (nAPI_DEVICE_section + 0x2)
#define nAPI_UDELAY               (nAPI_DEVICE_section + 0x3)
#define nAPI_MDELAY               (nAPI_DEVICE_section + 0x4)

#define nAPI_POWER                (nAPI_DEVICE_section + 0x10)
#define nAPI_SET_LCD_BRIGHTNESS   (nAPI_DEVICE_section + 0x12)
#define nAPI_GET_LCD_BRIGHTNESS   (nAPI_DEVICE_section + 0x13)

#define nAPI_GET_EVT_PIPE         (nAPI_DEVICE_section + 0x20)
#define nAPI_RM_EVT_PIPE          (nAPI_DEVICE_section + 0x21)
#define nAPI_GET_EVT              (nAPI_DEVICE_section + 0x22)
#define nAPI_GET_BTN              (nAPI_DEVICE_section + 0x23)

#define nAPI_EXIT                 (nAPI_DEVICE_section + 0x30)
#define nAPI_BKPT                 (nAPI_DEVICE_section + 0x31)
///////////////////// DEVICE API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////
#define nAPI_SOUND_PLAY           (nAPI_SND_section + 0)
#define nAPI_SOUND_PAUSE          (nAPI_SND_section + 1)
#define nAPI_SOUND_STOP           (nAPI_SND_section + 2)
#define nAPI_SOUND_BUFFER_WRITE   (nAPI_SND_section + 3)

#define nAPI_MIXER                (nAPI_SND_section + 10)
#define nAPI_DSP                  (nAPI_SND_section + 11)
////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////
#define nAPI_PRINTF               (nAPI_GFX_section + 50)

#define nAPI_GFX                  (nAPI_GFX_section + 70)
/////////////////////// GFX API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// DSP API ///////////////////////
#define nAPI_DSP_OPEN             (nAPI_DSP_section + 0)
#define nAPI_DSP_CLOSE            (nAPI_DSP_section + 1)
#define nAPI_DSP_LOAD_MEMCODE     (nAPI_DSP_section + 2)
#define nAPI_DSP_LOAD_HDDCODE     (nAPI_DSP_section + 3)
#define nAPI_DSP_ON               (nAPI_DSP_section + 4)
#define nAPI_DSP_OFF              (nAPI_DSP_section + 5)
#define nAPI_DSP_RESET            (nAPI_DSP_section + 6)
#define nAPI_DSP_RUN              (nAPI_DSP_section + 7)
/////////////////////// DSP API ///////////////////////
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
///////////////////// LIB FLOAT ///////////////////////
#define nAPI_MODSI3               (nAPI_FLOAT_section + 0)
#define nAPI_DIVSI3               (nAPI_FLOAT_section + 1)
#define nAPI_UMODSI3              (nAPI_FLOAT_section + 2)
#define nAPI_UDIVSI3              (nAPI_FLOAT_section + 3)
///////////////////// LIB FLOAT ///////////////////////
///////////////////////////////////////////////////////


MED_RET_T swi_call               (long swi_num,long param1,long param2,long param3) ;

int swi_file_handler              (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);
int swi_snd_handler               (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);
int swi_memory_handler            (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);
int swi_kernel_handler            (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);
//int swi_gfx_handler               (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);
int swi_dsp_handler               (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);
int swi_device_handler            (unsigned long nCmd,unsigned long nParam1,unsigned long nParam2,unsigned long nParam3);     

int swi_gfx_handler(unsigned long nCmd,unsigned long cmd,
    unsigned long nParam2,
    unsigned long nParam3);
        
#endif
