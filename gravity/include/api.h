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

typedef struct _POINT
{
	long x;
	long y;
} POINT;

typedef struct _RECT
{
	long x;
	long y;
	long w;
	long h;
} RECT;

typedef struct _IMAGE
{
	void* pixels;
	long w;
	long h;
	long stride;
	long pixel_size;
	long row_order; // 0=top first, 1=bottom first
} IMAGE;

typedef struct _GFX_DATA
{
    long x;
    long y;
    long w;
    long h;
    int direction;
    long delta; 
    unsigned int color;
    unsigned int bg_color;
} GFX_DATA;

typedef struct _PLANE_DATA
{
    int vplane;
    int state;
    int bpp;
} PLANE_DATA;


typedef unsigned long HTASK;
typedef unsigned long HPIPE;
typedef unsigned long HCRITSEC;
typedef unsigned long MESSAGE;


#define SAVE asm volatile ("stmdb sp!, {r3-r12}")
#define LOAD asm volatile ("ldmia sp!, {r3-r12}")



// API definition

ERROR_CODE API_TASK_YIELD       ();
ERROR_CODE API_TASK_CREATE      (void* pvCode, void* pParam, HTASK* phTask);
ERROR_CODE API_TASK_SUSPEND     (HTASK hTask);
ERROR_CODE API_TASK_CONTINUE    (HTASK hTask);
ERROR_CODE API_TASK_GETHANDLE   (HTASK* phTask);
ERROR_CODE API_TASK_SLEEP       (unsigned long nMilliseconds);
ERROR_CODE API_TASK_SENDMESSAGE (HTASK hTask, MESSAGE msg);
ERROR_CODE API_TASK_PEEKMESSAGE ();

ERROR_CODE API_MALLOC           (void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_FREE             (void* pvBuffer);
ERROR_CODE API_MEMAVAIL         (unsigned long* pnBytes);

ERROR_CODE API_MALLOC           (void** ppvBuffer, unsigned long nBytes);
ERROR_CODE API_FREE             (void* pvBuffer);
ERROR_CODE API_MEMAVAIL         (unsigned long* pnBytes);

ERROR_CODE API_PIPE_CREATE      (HPIPE* phPipe);
ERROR_CODE API_PIPE_DELETE      (HPIPE hPipe);
ERROR_CODE API_PIPE_SEND        (HPIPE hPipe, void* pData, unsigned long nBytesToSend);
ERROR_CODE API_PIPE_RECV        (HPIPE hPipe, void* pData, unsigned long nBytesToReceive);
ERROR_CODE API_CRITSEC_CREATE   (HCRITSEC* phCritSec);
ERROR_CODE API_CRITSEC_DELETE   (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_ENTER    (HCRITSEC hCritSec);
ERROR_CODE API_CRITSEC_LEAVE    (HCRITSEC hCritSec);

ERROR_CODE API_GFX              (int cmd, GFX_DATA * gfxD, void * pvData);
ERROR_CODE API_PRINTF           (const char * fmt, va_list args);
void printf                     (char *fmt, ...);


ERROR_CODE swi_call             (long swi_num,long param1,long param2,long param3) ;

#endif
