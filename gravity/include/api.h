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

#include <types.h>

// API definition

ERROR_CODE API_TASK_YIELD       ();
ERROR_CODE API_TASK_CREATE      (void* pvCode, void* pParam, HTASK* phTask);
ERROR_CODE API_TASK_SUSPEND     (HTASK hTask);
ERROR_CODE API_TASK_CONTINUE    (HTASK hTask);
ERROR_CODE API_TASK_GETHANDLE   (HTASK* phTask);
ERROR_CODE API_TASK_SLEEP       (unsigned long nMilliseconds);
ERROR_CODE API_TASK_SENDMESSAGE (HTASK hTask, MESSAGE* pMsg);
ERROR_CODE API_TASK_PEEKMESSAGE (MESSAGE* pMsg);
ERROR_CODE API_TASK_WAITMESSAGE (MESSAGE* pMsg);
ERROR_CODE API_TASK_TERMINATE   ();
ERROR_CODE API_TASK_SETPRIORITY (HTASK hTask, int nPriority);
ERROR_CODE API_SET_EVENT_MASK   (HTASK hTask, unsigned long nMask);

ERROR_CODE API_MALLOC           (void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_FREE             (void* pvBuffer);
ERROR_CODE API_MEMAVAIL         (unsigned long* pnBytes);
ERROR_CODE API_HEAP_CREATE      (HEAP* phHeap, unsigned long nBytes);
ERROR_CODE API_HEAP_DESTROY     (HEAP hHeap);
ERROR_CODE API_HEAP_MALLOC      (HEAP hHeap, void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_HEAP_FREE        (HEAP hHeap, void* pvBuffer);
ERROR_CODE API_HEAP_AVAIL       (HEAP hHeap, unsigned long* pnBytes);

ERROR_CODE API_PIPE_CREATE      (HPIPE* phPipe);
ERROR_CODE API_PIPE_DELETE      (HPIPE hPipe);
ERROR_CODE API_PIPE_SEND        (HPIPE hPipe, void* pData, unsigned long nBytesToSend);
ERROR_CODE API_PIPE_RECV        (HPIPE hPipe, void* pData, unsigned long nBytesToReceive);
ERROR_CODE API_PIPE_TEST        (HPIPE hPipe);

ERROR_CODE API_CRITSEC_CREATE   (HCRITSEC* phCritSec);
ERROR_CODE API_CRITSEC_DELETE   (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_ENTER    (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_LEAVE    (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_TRYENTER (HCRITSEC hCritSec);

ERROR_CODE API_GFX_FASTBLIT     (GFX_DATA* pDst, GFX_DATA* pSrc, GFX_POINT* pOrigin);
ERROR_CODE API_GFX_STRETCHBLIT  (GFX_DATA* pDst, GFX_DATA* pSrc);
ERROR_CODE API_GFX_PATTERNBLIT  (GFX_DATA* pDst, GFX_DATA* pSrc);

ERROR_CODE API_SOUND_PLAY       (void* pvBuffer, unsigned long nBytes, unsigned long nFlags);
ERROR_CODE API_SOUND_PAUSE      ();
ERROR_CODE API_SOUND_STOP       ();

ERROR_CODE API_RUN_GRV          (const char* pGRVPath, HTASK* phTask);

ERROR_CODE API_GFX              (int cmd, GFX_DATA * gfxD, void * pvData);
ERROR_CODE API_PRINTF           (const char * fmt, va_list args);

ERROR_CODE API_TIME             (int cmd,struct av_tm * time_data);

ERROR_CODE API_FILE             (int cmd,void * data1,void * data2);

ERROR_CODE API_POWER            (int cmd,int * val);
int        do_api_power         (int cmd);

ERROR_CODE API_MIXER            (int cmd, int dir, void * arg);
ERROR_CODE API_DSP              (int cmd, void * arg);

ERROR_CODE API_EVT              (int cmd, void * arg, void * arg2);

void       printf               (char *fmt, ...);

void *  malloc(long size);
void    free(void *buff);

unsigned int register_evt(void);
void unregister_evt(unsigned int arg);
int waitEvt(unsigned int arg);
void sendEvt(int evt);
void flushEvt(unsigned int arg);

#define  getTime(time_data)     API_TIME(0x000,time_data)
#define  setTime(time_data)     API_TIME(0x001,time_data)

#define  usbIsConnected()       do_api_power(0x000)
#define  FWIsConnected()        do_api_power(0x001)
#define  powerConnected()       do_api_power(0x002)
#define  getBatLevel()          do_api_power(0x001)

//ERROR_CODE getTime              (struct av_tm * time_data);
//ERROR_CODE setTime              (struct av_tm * time_data);

#endif
