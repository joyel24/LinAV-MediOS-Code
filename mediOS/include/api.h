/*
*   include/api.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __API_H
#define __API_H

#include <stdarg.h>
#include <kernel/errors.h>

#include <sys_def/time.h>
#include <sys_def/section_types.h>

#include <types.h>

#include <kernel/sound.h>
#include <kernel/ata.h>
#include <kernel/hardware.h>
#include <kernel/bat_power.h>

// API definition


///////////////////////////////////////////////////////
////////////////////// MEMORY API /////////////////////
MED_RET_T API_MALLOC               (void** ppvBuffer, unsigned long nBytes);
MED_RET_T API_FREE                 (void* pvBuffer);
MED_RET_T API_MEMAVAIL             (unsigned long* pnBytes);
MED_RET_T API_REALLOC              (void** ppvBuffer, void* pvBuffer, unsigned long nBytes);

MED_RET_T API_MEMSET               (void* pvBuffer, int fill, int size);
MED_RET_T API_MEMCPY               (void* pvBuffer, const void* pvSrc, int size);

///////////////////////////////////////////////////////
////////////////////// FILE API ///////////////////////
MED_RET_T API_FILE                 (int cmd,void * data1,void * data2);
MED_RET_T API_RUN_GRV              (const char* pGRVPath, HTASK* phTask);
////////////////////// FILE API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// DEVICE API //////////////////////
MED_RET_T API_GET_TICK             (unsigned long * val);
MED_RET_T API_TIME                 (int cmd,struct av_tm * time_data);
MED_RET_T API_POWER                (int cmd,int * val);

MED_RET_T API_SET_LCD_BRIGHTNESS   (int nBrightness);
MED_RET_T API_GET_LCD_BRIGHTNESS   (int* pnBrightness);

MED_RET_T API_EVT_GET_HANDLER      (unsigned int mask, int * evt_handler);
MED_RET_T API_EVT_FREE_HANDLER     (int evt_handler);
MED_RET_T API_EVT_GET              (int evt_handler,int * data);
MED_RET_T API_BTN_GET              (int * data);
#define evt_freeHandler API_EVT_FREE_HANDLER
int evt_get_handler(unsigned int mask);
int evt_get(int evt_handler);
int btn_get(void);

MED_RET_T udelay                   (int val);
MED_RET_T mdelay                   (int val);

MED_RET_T API_BKPT                 (void);

MED_RET_T API_SET_GET_INT_TIMER    (int type,int mode,TRI_DATA * pvData);

///////////////////// DEVICE API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////
MED_RET_T API_SOUND_PLAY           (void* pvBuffer, unsigned long nBytes, unsigned long nFlags);
MED_RET_T API_SOUND_PAUSE          ();
MED_RET_T API_SOUND_STOP           ();
MED_RET_T API_DSP                  (int cmd, void * arg);
MED_RET_T API_MIXER                (int cmd, int dir, void * arg);
int sound_buffer_write(sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),int count,void * param);

MED_RET_T API_SOUND_BUFFER_WRITE   (sound_buffer_s * sound_buffer,sound_api_param  * api_param, void * param);
////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////
MED_RET_T API_PRINTF               (const char* fmt, va_list args);

MED_RET_T API_GFX                  (int cmd, void* ptr, void * pvData);

///////////////////////////////////////////////////////
/////////////////////// DSP API ///////////////////////
MED_RET_T API_DSP_OPEN             (void* pHandler);
MED_RET_T API_DSP_CLOSE            ();
MED_RET_T API_DSP_LOAD_MEMCODE     (void* pCode, int nSize);
MED_RET_T API_DSP_LOAD_HDDCODE     (const char* pszCoffProgram);
MED_RET_T API_DSP_ON               ();
MED_RET_T API_DSP_OFF              ();
MED_RET_T API_DSP_RESET            ();
MED_RET_T API_DSP_RUN              ();
/////////////////////// DSP API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// LIB FLOAT ///////////////////////
MED_RET_T API_MODSI3      (long a, long b,long * res);
MED_RET_T API_DIVSI3      (long a, long b,long * res);
MED_RET_T API_UMODSI3     (long a, long b,long * res);
MED_RET_T API_UDIVSI3     (long a, long b,long * res);
MED_RET_T API_DIVDI3     (unsigned int a, unsigned int b,unsigned int * res);

long __modsi3             (long a, long b);
long __divsi3             (long a, long b);
long __umodsi3            (long a, long b);
long __udivsi3            (long a, long b);
unsigned int __divdi3     (unsigned int a, unsigned int b);
///////////////////////////////////////////////////////
///////////////////// LIB FLOAT ///////////////////////

int        do_api_power         (int cmd);

void       printf               (char *fmt, ...);

void       exit(int code);

void *  malloc(long size);
void *  realloc(void *buff,long size);
void    free(void *buff);

unsigned long get_tick(void);

#define  getTime(time_data)     API_TIME(0x000,time_data)
#define  setTime(time_data)     API_TIME(0x001,time_data)

#define  usbIsConnected()       do_api_power(0x000)
#define  FWIsConnected()        do_api_power(0x001)
#define  powerConnected()       do_api_power(0x002)
#define  getBatLevel()          do_api_power(0x001)

void set_timer_status(int timer_type, int power_mode, int status);
void set_timer_delay(int timer_type, int power_mode, int delay);
int  get_timer_status(int timer_type, int power_mode);
int  get_timer_delay(int timer_type, int power_mode);


#endif
