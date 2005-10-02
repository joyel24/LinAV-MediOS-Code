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

#include <kernel/sound.h>
#include <kernel/ata.h>

// API definition

///////////////////////////////////////////////////////
////////////////////// KERNEL API /////////////////////
ERROR_CODE API_TASK_YIELD           ();
ERROR_CODE API_TASK_SLEEP           (unsigned long nMilliseconds);
ERROR_CODE API_TASK_SUSPEND         (HTASK hTask);
ERROR_CODE API_TASK_CONTINUE        (HTASK hTask);
ERROR_CODE API_TASK_SETPRIORITY     (HTASK hTask, int nPriority);
ERROR_CODE API_TASK_GETHANDLE       (HTASK* phTask);

ERROR_CODE API_TASK_CREATE          (void* pvCode, void* pParam, HTASK* phTask);
ERROR_CODE API_TASK_NAME            (HTASK* phTask, char * name);
ERROR_CODE API_TASK_TERMINATE       ();

int        API_OS_VERSION           ();

ERROR_CODE API_TASK_SENDMESSAGE     (HTASK hTask, MESSAGE* pMsg);
ERROR_CODE API_TASK_PEEKMESSAGE     (MESSAGE* pMsg);
ERROR_CODE API_TASK_WAITMESSAGE     (MESSAGE* pMsg);
//ERROR_CODE API_SET_EVENT_MASK   (HTASK hTask, unsigned long nMask);
ERROR_CODE API_PIPE_CREATE          (HPIPE* phPipe);
ERROR_CODE API_PIPE_DELETE          (HPIPE hPipe);
ERROR_CODE API_PIPE_SEND            (HPIPE hPipe, void* pData, unsigned long nBytesToSend);
ERROR_CODE API_PIPE_RECV            (HPIPE hPipe, void* pData, unsigned long nBytesToReceive);
ERROR_CODE API_PIPE_TEST            (HPIPE hPipe);

ERROR_CODE API_CRITSEC_CREATE       (HCRITSEC* phCritSec);
ERROR_CODE API_CRITSEC_DELETE       (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_ENTER        (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_LEAVE        (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_TRYENTER     (HCRITSEC hCritSec);

ERROR_CODE API_VAR_GET              (const char* pVarName, char* pVarValue, int nBufferSize);
ERROR_CODE API_VAR_SET              (const char* pVarName, const char* pVarValue);
////////////////////// KERNEL API /////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// MEMORY API /////////////////////
ERROR_CODE API_MALLOC               (void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_FREE                 (void* pvBuffer);
ERROR_CODE API_MEMAVAIL             (unsigned long* pnBytes);
ERROR_CODE API_MEMSET               (void* pvBuffer, int fill, int size);
ERROR_CODE API_MEMCPY               (void* pvBuffer, const void* pvSrc, int size);

ERROR_CODE API_HEAP_CREATE          (HEAP* phHeap, unsigned long nBytes);
ERROR_CODE API_HEAP_DESTROY         (HEAP hHeap);
ERROR_CODE API_HEAP_MALLOC          (HEAP hHeap, void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_HEAP_FREE            (HEAP hHeap, void* pvBuffer);
ERROR_CODE API_HEAP_AVAIL           (HEAP hHeap, unsigned long* pnBytes);
////////////////////// MEMORY API /////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// FILE API ///////////////////////
ERROR_CODE API_FILE                 (int cmd,void * data1,void * data2);
ERROR_CODE API_RUN_GRV              (const char* pGRVPath, HTASK* phTask);
////////////////////// FILE API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// DEVICE API //////////////////////
ERROR_CODE API_TIME                 (int cmd,struct av_tm * time_data);
ERROR_CODE API_POWER                (int cmd,int * val);
ERROR_CODE API_DSP                  (int cmd, void * arg);
ERROR_CODE API_EVT                  (int cmd, void * arg, void * arg2);
ERROR_CODE API_SET_LCD_BRIGHTNESS   (int nBrightness);
ERROR_CODE API_GET_LCD_BRIGHTNESS   (int* pnBrightness);
///////////////////// DEVICE API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////
ERROR_CODE API_SOUND_PLAY           (void* pvBuffer, unsigned long nBytes, unsigned long nFlags);
ERROR_CODE API_SOUND_PAUSE          ();
ERROR_CODE API_SOUND_STOP           ();

ERROR_CODE API_MIXER                (int cmd, int dir, void * arg);
////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////
ERROR_CODE API_GFX_OPEN_GRAPHICS    ();
ERROR_CODE API_GFX_CLOSE_GRAPHICS   ();
ERROR_CODE API_GFX_CREATE_CONTEXT   (int nWidth, int nHeight, int nFlags);
ERROR_CODE API_GFX_GET_CONTEXT      (GFX_CONTEXT* pCtx);
ERROR_CODE API_GFX_GET_PHYS_CONTEXT (GFX_CONTEXT* pCtx);
ERROR_CODE API_GFX_SET_DRAWING_RECT (GFX_RECT* pRect);
ERROR_CODE API_GFX_GET_DRAWING_RECT (GFX_RECT* pRect);
ERROR_CODE API_GFX_TASK_FROM_POINT  (int nX, int nY, HTASK* phTask);

ERROR_CODE API_GFX_SET_PLANE        (int nPlane);
ERROR_CODE API_GFX_GET_PLANE        (int* pnPlane);
ERROR_CODE API_GFX_SHOW_PLANE       (int nPlane, int bShow);
ERROR_CODE API_GFX_SET_PLANE_STATE  ();
ERROR_CODE API_GFX_GET_PLANE_STATE  ();
ERROR_CODE API_GFX_SET_PLANE_POS    (int nPlane, int nX, int nY);
ERROR_CODE API_GFX_GET_PLANE_POS    (int nPlane, int* pnX, int* pnY);

ERROR_CODE API_GFX_UPDATE_RECT      (GFX_RECT* pArea);
ERROR_CODE API_GFX_MOVE             (GFX_POINT* pOrigin);
ERROR_CODE API_GFX_SET_Z_ORDER      (E_Z_ORDER order);

ERROR_CODE API_GFX_FASTBLIT         (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_POINT* pOrigin);
ERROR_CODE API_GFX_BLENDBLIT        (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_BLENDPARAMS* pParams);
ERROR_CODE API_GFX_STRETCHBLIT      (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, unsigned long* pWorkBuffer);
ERROR_CODE API_GFX_PATTERNBLIT      (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc);
ERROR_CODE API_GFX_CHARBLIT         (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_POINT* pOrigin);

ERROR_CODE API_GFX_DRAWPIXEL        (int nX, int nY, COLOR Color);
COLOR      API_GFX_READPIXEL        (int nX, int nY);
ERROR_CODE API_GFX_DRAWLINE         (GFX_POINT* pt1, GFX_POINT* pt2, COLOR Color);
ERROR_CODE API_GFX_DRAWRECT         (GFX_RECT* pRect, COLOR Color);
ERROR_CODE API_GFX_FILLRECT         (GFX_RECT* pRect, COLOR Color);
ERROR_CODE API_GFX_SCROLL_HORIZ     ();
ERROR_CODE API_GFX_SCROLL_VERT      ();

ERROR_CODE API_PRINTF               (const char* fmt, va_list args);
ERROR_CODE API_SET_FONT             (HFONT hFont);
ERROR_CODE API_GET_FONT             (HFONT* phFont);
ERROR_CODE API_TEXT                 (const char* pszText, int x, int y);
ERROR_CODE API_GET_TEXT_RECT        (const char* pszText, GFX_RECT* pRect);
ERROR_CODE API_SET_FONT_COLOR       (COLOR nColor);
ERROR_CODE API_GET_FONT_COLOR       (COLOR* pnColor);

ERROR_CODE API_RENDER_SCENE         ();

ERROR_CODE API_GFX                  (int cmd, void* ptr, void * pvData);

ERROR_CODE API_GFX_BUILD_SPANS      (int nYmin, int nYmax);
/////////////////////// GFX API ///////////////////////
///////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////

int sound_buffer_write(sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),int count,void * param);

ERROR_CODE API_SOUND_BUFFER_WRITE   (sound_buffer_s * sound_buffer,sound_api_param  * api_param, void * param);

////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

int        do_api_power         (int cmd);

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
