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
#define nAPI_KERNEL_section_code   0
#define nAPI_GFX_section_code      1
#define nAPI_SND_section_code      2
#define nAPI_FILE_section_code     3
#define nAPI_DEVICE_section_code   4
#define nAPI_MEMORY_section_code   5

#define nAPI_KERNEL_section   (nAPI_KERNEL_section_code << 8)
#define nAPI_GFX_section      (nAPI_GFX_section_code    << 8)
#define nAPI_SND_section      (nAPI_SND_section_code    << 8)
#define nAPI_FILE_section     (nAPI_FILE_section_code   << 8)
#define nAPI_DEVICE_section   (nAPI_DEVICE_section_code << 8)
#define nAPI_MEMORY_section   (nAPI_MEMORY_section_code << 8)
///////////////////// API SECTIONS ////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// KERNEL API /////////////////////
#define nAPI_TASK_YIELD         (nAPI_KERNEL_section+0)
#define nAPI_TASK_SLEEP         (nAPI_KERNEL_section+1)
#define nAPI_TASK_SUSPEND       (nAPI_KERNEL_section+2)
#define nAPI_TASK_CONTINUE      (nAPI_KERNEL_section+3)
#define nAPI_TASK_SETPRIORITY   (nAPI_KERNEL_section+4)
#define nAPI_TASK_GETHANDLE     (nAPI_KERNEL_section+5)

#define nAPI_TASK_CREATE        (nAPI_KERNEL_section+10)
#define nAPI_TASK_TERMINATE     (nAPI_KERNEL_section+11)

#define nAPI_OS_VERSION         (nAPI_KERNEL_section+20)

#define nAPI_TASK_SENDMESSAGE   (nAPI_KERNEL_section+30)
#define nAPI_TASK_PEEKMESSAGE   (nAPI_KERNEL_section+31)
#define nAPI_TASK_WAITMESSAGE   (nAPI_KERNEL_section+32)

#define nAPI_PIPE_CREATE        (nAPI_KERNEL_section+40)
#define nAPI_PIPE_DELETE        (nAPI_KERNEL_section+41)
#define nAPI_PIPE_SEND          (nAPI_KERNEL_section+42)
#define nAPI_PIPE_RECV          (nAPI_KERNEL_section+43)
#define nAPI_PIPE_TEST          (nAPI_KERNEL_section+44)

#define nAPI_CRITSEC_CREATE     (nAPI_KERNEL_section+50)
#define nAPI_CRITSEC_DELETE     (nAPI_KERNEL_section+51)
#define nAPI_CRITSEC_ENTER      (nAPI_KERNEL_section+52)
#define nAPI_CRITSEC_LEAVE      (nAPI_KERNEL_section+53)
#define nAPI_CRITSEC_TRYENTER   (nAPI_KERNEL_section+54)

#define nAPI_VAR_GET            (nAPI_KERNEL_section+60)
#define nAPI_VAR_SET            (nAPI_KERNEL_section+61)
////////////////////// KERNEL API /////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// MEMORY API /////////////////////
#define nAPI_MALLOC             (nAPI_MEMORY_section+0)
#define nAPI_FREE               (nAPI_MEMORY_section+1)
#define nAPI_MEMAVAIL           (nAPI_MEMORY_section+2)
#define nAPI_MEMSET             (nAPI_MEMORY_section+3)
#define nAPI_MEMCPY             (nAPI_MEMORY_section+4)

#define nAPI_HEAP_CREATE        (nAPI_MEMORY_section+10)
#define nAPI_HEAP_DESTROY       (nAPI_MEMORY_section+11)
#define nAPI_HEAP_MALLOC        (nAPI_MEMORY_section+12)
#define nAPI_HEAP_FREE          (nAPI_MEMORY_section+13)
#define nAPI_HEAP_AVAIL         (nAPI_MEMORY_section+14)
////////////////////// MEMORY API /////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// FILE API ///////////////////////
#define nAPI_FILE              (nAPI_FILE_section + 0)
#define nAPI_RUN_GRV           (nAPI_FILE_section + 10)
////////////////////// FILE API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// DEVICE API //////////////////////
#define nAPI_TIME              (nAPI_DEVICE_section + 0)
#define nAPI_POWER             (nAPI_DEVICE_section + 1)
#define nAPI_DSP               (nAPI_DEVICE_section + 2)
#define nAPI_EVT               (nAPI_DEVICE_section + 3)
///////////////////// DEVICE API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////
#define nAPI_SOUND_PLAY        (nAPI_SND_section + 0)
#define nAPI_SOUND_PAUSE       (nAPI_SND_section + 1)
#define nAPI_SOUND_STOP        (nAPI_SND_section + 2)

#define nAPI_MIXER             (nAPI_SND_section + 10)
////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////
#define nAPI_GFX_CREATE_CONTEXT (nAPI_GFX_section + 0)

#define nAPI_GFX_UPDATE_RECT    (nAPI_GFX_section + 10)
#define nAPI_GFX_MOVE           (nAPI_GFX_section + 11)
#define nAPI_GFX_FOREGROUND     (nAPI_GFX_section + 12)

#define nAPI_GFX_FASTBLIT       (nAPI_GFX_section + 20)
#define nAPI_GFX_BLENDBLIT      (nAPI_GFX_section + 21)
#define nAPI_GFX_STRETCHBLIT    (nAPI_GFX_section + 22)
#define nAPI_GFX_PATTERNBLIT    (nAPI_GFX_section + 23)

#define nAPI_PRINTF             (nAPI_GFX_section + 30)

#define nAPI_GFX                (nAPI_GFX_section + 40)
/////////////////////// GFX API ///////////////////////
///////////////////////////////////////////////////////

ERROR_CODE swi_call             (long swi_num,long param1,long param2,long param3) ;

#endif
