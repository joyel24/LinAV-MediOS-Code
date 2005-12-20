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
ERROR_CODE API_MALLOC               (void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_FREE                 (void* pvBuffer);
ERROR_CODE API_MEMAVAIL             (unsigned long* pnBytes);
ERROR_CODE API_REALLOC              (void** ppvBuffer, void* pvBuffer, unsigned long nBytes);

ERROR_CODE API_MEMSET               (void* pvBuffer, int fill, int size);
ERROR_CODE API_MEMCPY               (void* pvBuffer, const void* pvSrc, int size);

///////////////////////////////////////////////////////
////////////////////// FILE API ///////////////////////
ERROR_CODE API_FILE                 (int cmd,void * data1,void * data2);
ERROR_CODE API_RUN_GRV              (const char* pGRVPath, HTASK* phTask);
////////////////////// FILE API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// DEVICE API //////////////////////
ERROR_CODE API_GET_TICK             (unsigned long * val);
ERROR_CODE API_TIME                 (int cmd,struct av_tm * time_data);
ERROR_CODE API_POWER                (int cmd,int * val);

ERROR_CODE API_SET_LCD_BRIGHTNESS   (int nBrightness);
ERROR_CODE API_GET_LCD_BRIGHTNESS   (int* pnBrightness);

ERROR_CODE API_GET_EVT_PIPE         (EVT_PIPE * pipe);
ERROR_CODE API_RM_EVT_PIPE          (EVT_PIPE pipe);
ERROR_CODE API_GET_EVT              (EVT_PIPE pipe,char * data);
#define rm_evt_pipe                 API_RM_EVT_PIPE
EVT_PIPE get_evt_pipe(void);
char get_evt(EVT_PIPE var);

ERROR_CODE API_BKPT                 (void);

ERROR_CODE API_SET_GET_INT_TIMER    (int type,int mode,TRI_DATA * pvData);

///////////////////// DEVICE API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////
ERROR_CODE API_SOUND_PLAY           (void* pvBuffer, unsigned long nBytes, unsigned long nFlags);
ERROR_CODE API_SOUND_PAUSE          ();
ERROR_CODE API_SOUND_STOP           ();
ERROR_CODE API_DSP                  (int cmd, void * arg);
ERROR_CODE API_MIXER                (int cmd, int dir, void * arg);
int sound_buffer_write(sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),int count,void * param);

ERROR_CODE API_SOUND_BUFFER_WRITE   (sound_buffer_s * sound_buffer,sound_api_param  * api_param, void * param);
////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////
ERROR_CODE API_PRINTF               (const char* fmt, va_list args);

ERROR_CODE API_GFX                  (int cmd, void* ptr, void * pvData);

///////////////////////////////////////////////////////
/////////////////////// DSP API ///////////////////////
ERROR_CODE API_DSP_OPEN             (void* pHandler);
ERROR_CODE API_DSP_CLOSE            ();
ERROR_CODE API_DSP_LOAD_MEMCODE     (void* pCode, int nSize);
ERROR_CODE API_DSP_LOAD_HDDCODE     (const char* pszCoffProgram);
ERROR_CODE API_DSP_ON               ();
ERROR_CODE API_DSP_OFF              ();
ERROR_CODE API_DSP_RESET            ();
ERROR_CODE API_DSP_RUN              ();
/////////////////////// DSP API ///////////////////////
///////////////////////////////////////////////////////

int        do_api_power         (int cmd);

void       printf               (char *fmt, ...);

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
