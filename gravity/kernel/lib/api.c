/* 
*   kernel/api.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdarg.h>
#include <sys_def/stddef.h>
#include <kernel/swi.h>
#include <api.h>

#define swi_call(SWI_NUM)                \
    ({                                   \
        register long _r0 asm("r0");     \
        asm volatile(                    \
            "stmdb sp!, {r3-r5} \n"     \
            "swi %0\n"                   \
            "ldmia sp!, {r3-r5}"        \
        :                                \
        : "i"(SWI_NUM)                   \
        : "memory");                     \
        return (long) _r0;               \
    })    



ERROR_CODE API_TASK_YIELD       ()                                                              { swi_call(nAPI_TASK_YIELD); }
ERROR_CODE API_TASK_CREATE      (void* pvCode, void* pParam, HTASK* phTask)                     { swi_call(nAPI_TASK_CREATE); }
ERROR_CODE API_TASK_SUSPEND     (HTASK hTask)                                                   { swi_call(nAPI_TASK_SUSPEND); }
ERROR_CODE API_TASK_CONTINUE    (HTASK hTask)                                                   { swi_call(nAPI_TASK_CONTINUE); }
ERROR_CODE API_TASK_GETHANDLE   (HTASK* phTask)                                                 { swi_call(nAPI_TASK_GETHANDLE); }
ERROR_CODE API_TASK_SLEEP       (unsigned long nMilliseconds)                                   { swi_call(nAPI_TASK_SLEEP); }
ERROR_CODE API_TASK_SENDMESSAGE (HTASK hTask, MESSAGE* pMsg)                                    { swi_call(nAPI_TASK_SENDMESSAGE); }
ERROR_CODE API_TASK_PEEKMESSAGE (MESSAGE* pMsg)                                                 { swi_call(nAPI_TASK_PEEKMESSAGE); }
ERROR_CODE API_TASK_WAITMESSAGE (MESSAGE* pMsg)                                                 { swi_call(nAPI_TASK_WAITMESSAGE); }
ERROR_CODE API_TASK_TERMINATE   ()                                                              { swi_call(nAPI_TASK_TERMINATE); }
ERROR_CODE API_TASK_SETPRIORITY (HTASK hTask, int nPriority)                                    { swi_call(nAPI_TASK_SETPRIORITY); }

ERROR_CODE API_MALLOC           (void** ppvBuffer, unsigned long nBytes)                        { swi_call(nAPI_MALLOC); }
ERROR_CODE API_FREE             (void* pvBuffer)                                                { swi_call(nAPI_FREE); }
ERROR_CODE API_MEMAVAIL         (unsigned long* pnBytes)                                        { swi_call(nAPI_MEMAVAIL); }
ERROR_CODE API_HEAP_CREATE      (HEAP* phHeap, unsigned long nBytes)                            { swi_call(nAPI_HEAP_CREATE); }
ERROR_CODE API_HEAP_DESTROY     (HEAP hHeap)                                                    { swi_call(nAPI_HEAP_DESTROY); }
ERROR_CODE API_HEAP_MALLOC      (HEAP hHeap, void** ppvBuffer, unsigned long nBytes)            { swi_call(nAPI_HEAP_MALLOC); }
ERROR_CODE API_HEAP_FREE        (HEAP hHeap, void* pvBuffer)                                    { swi_call(nAPI_HEAP_FREE); }
ERROR_CODE API_HEAP_AVAIL       (HEAP hHeap, unsigned long* pnBytes)                            { swi_call(nAPI_HEAP_AVAIL); }

ERROR_CODE API_PIPE_CREATE      (HPIPE* phPipe)                                                 { swi_call(nAPI_PIPE_CREATE); }
ERROR_CODE API_PIPE_DELETE      (HPIPE hPipe)                                                   { swi_call(nAPI_PIPE_DELETE); }
ERROR_CODE API_PIPE_SEND        (HPIPE hPipe, void* pData, unsigned long nBytesToSend)          { swi_call(nAPI_PIPE_SEND); }
ERROR_CODE API_PIPE_RECV        (HPIPE hPipe, void* pData, unsigned long nBytesToReceive)       { swi_call(nAPI_PIPE_RECV); }
ERROR_CODE API_PIPE_TEST        (HPIPE hPipe)                                                   { swi_call(nAPI_PIPE_TEST); }

ERROR_CODE API_CRITSEC_CREATE   (HCRITSEC* phCritSec)                                           { swi_call(nAPI_CRITSEC_CREATE); }
ERROR_CODE API_CRITSEC_DELETE   (HCRITSEC hCritSec)                                             { swi_call(nAPI_CRITSEC_DELETE); }
ERROR_CODE API_CRITSEC_ENTER    (HCRITSEC hCritSec)                                             { swi_call(nAPI_CRITSEC_ENTER); }
ERROR_CODE API_CRITSEC_LEAVE    (HCRITSEC hCritSec)                                             { swi_call(nAPI_CRITSEC_LEAVE); }
ERROR_CODE API_CRITSEC_TRYENTER (HCRITSEC hCritSec)                                             { swi_call(nAPI_CRITSEC_TRYENTER); }

ERROR_CODE API_TIME             (int cmd,struct av_tm * time_data)                              { swi_call(nAPI_TIME); }
ERROR_CODE API_POWER            (int cmd,int * val)                                             { swi_call(nAPI_POWER); }
ERROR_CODE API_MIXER            (int cmd, int dir, void * arg)                                  { swi_call(nAPI_MIXER); }
ERROR_CODE API_DSP              (int cmd, void * arg)                                           { swi_call(nAPI_DSP); }
ERROR_CODE API_EVT              (int cmd, void * arg, void * arg2)                              { swi_call(nAPI_EVT); }

ERROR_CODE API_RUN_GRV          (const char* pGRVPath, HTASK* phTask)                           { swi_call(nAPI_RUN_GRV); }

ERROR_CODE API_SOUND_PLAY       (void* pvBuffer, unsigned long nBytes, unsigned long nFlags)    { swi_call(nAPI_SOUND_PLAY); }
ERROR_CODE API_SOUND_PAUSE      ()                                                              { swi_call(nAPI_SOUND_PAUSE); }
ERROR_CODE API_SOUND_STOP       ()                                                              { swi_call(nAPI_SOUND_STOP); }

ERROR_CODE API_VAR_GET          (const char* pVarName, char* pVarValue, int nBufferSize)        { swi_call(nAPI_VAR_GET); }
ERROR_CODE API_VAR_SET          (const char* pVarName, const char* pVarValue)                   { swi_call(nAPI_VAR_SET); }

ERROR_CODE API_FILE             (int cmd,void * data1,void * data2)                             { swi_call(nAPI_FILE); }

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////
ERROR_CODE API_GFX_OPEN_GRAPHICS    ()                                                                 { swi_call(nAPI_GFX_OPEN_GRAPHICS); }
ERROR_CODE API_GFX_CLOSE_GRAPHICS   ()                                                                 { swi_call(nAPI_GFX_CLOSE_GRAPHICS); }
ERROR_CODE API_GFX_CREATE_CONTEXT   (int nWidth, int nHeight, int nFlags)                              { swi_call(nAPI_GFX_CREATE_CONTEXT); }
ERROR_CODE API_GFX_GET_CONTEXT      (GFX_CONTEXT* pCtx)                                                { swi_call(nAPI_GFX_GET_CONTEXT); }
ERROR_CODE API_GFX_GET_PHYS_CONTEXT (GFX_CONTEXT* pCtx)                                                { swi_call(nAPI_GFX_GET_PHYS_CONTEXT); }
ERROR_CODE API_GFX_SET_DRAWING_RECT (GFX_RECT* pRect)                                                  { swi_call(nAPI_GFX_SET_DRAWING_RECT); }
ERROR_CODE API_GFX_GET_DRAWING_RECT (GFX_RECT* pRect)                                                  { swi_call(nAPI_GFX_GET_DRAWING_RECT); }

ERROR_CODE API_GFX_SET_PLANE        (int nPlane)                                                       { swi_call(nAPI_GFX_SET_PLANE); }
ERROR_CODE API_GFX_GET_PLANE        (int* pnPlane)                                                     { swi_call(nAPI_GFX_GET_PLANE); }
ERROR_CODE API_GFX_SHOW_PLANE       (int nPlane, int bShow)                                            { swi_call(nAPI_GFX_SHOW_PLANE); }
ERROR_CODE API_GFX_SET_PLANE_STATE  ()                                                                 { swi_call(nAPI_GFX_SET_PLANE_STATE); }
ERROR_CODE API_GFX_GET_PLANE_STATE  ()                                                                 { swi_call(nAPI_GFX_GET_PLANE_STATE); }
ERROR_CODE API_GFX_SET_PLANE_POS    (int nPlane, int nX, int nY)                                       { swi_call(nAPI_GFX_SET_PLANE_POS); }
ERROR_CODE API_GFX_GET_PLANE_POS    (int nPlane, int* pnX, int* pnY)                                   { swi_call(nAPI_GFX_GET_PLANE_POS); }

ERROR_CODE API_GFX_UPDATE_RECT      (GFX_RECT* pArea)                                                  { swi_call(nAPI_GFX_UPDATE_RECT); }
ERROR_CODE API_GFX_MOVE             (GFX_POINT* pOrigin)                                               { swi_call(nAPI_GFX_MOVE); }
ERROR_CODE API_GFX_FOREGROUND       ()                                                                 { swi_call(nAPI_GFX_FOREGROUND); }

ERROR_CODE API_GFX_FASTBLIT         (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_POINT* pOrigin)         { swi_call(nAPI_GFX_FASTBLIT); }
ERROR_CODE API_GFX_BLENDBLIT        (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_BLENDPARAMS* pParams)   { swi_call(nAPI_GFX_BLENDBLIT); }
ERROR_CODE API_GFX_STRETCHBLIT      (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, unsigned long* pWorkBuffer) { swi_call(nAPI_GFX_STRETCHBLIT); }
ERROR_CODE API_GFX_PATTERNBLIT      (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc)                             { swi_call(nAPI_GFX_PATTERNBLIT); }
ERROR_CODE API_GFX_CHARBLIT         (GFX_CONTEXT* pDst, GFX_CONTEXT* pSrc, GFX_POINT* pOrigin)         { swi_call(nAPI_GFX_CHARBLIT); }

ERROR_CODE API_GFX_DRAWPIXEL        (int nX, int nY, COLOR Color)                                      { swi_call(nAPI_GFX_DRAWPIXEL); }
COLOR      API_GFX_READPIXEL        (int nX, int nY)                                                   { swi_call(nAPI_GFX_READPIXEL); }
ERROR_CODE API_GFX_DRAWLINE         (GFX_POINT* pt1, GFX_POINT* pt2, COLOR Color)                      { swi_call(nAPI_GFX_DRAWLINE); }
ERROR_CODE API_GFX_DRAWRECT         (GFX_RECT* pRect, COLOR Color)                                     { swi_call(nAPI_GFX_DRAWRECT); }
ERROR_CODE API_GFX_FILLRECT         (GFX_RECT* pRect, COLOR Color)                                     { swi_call(nAPI_GFX_FILLRECT); }
ERROR_CODE API_GFX_SCROLL_HORIZ     ()                                                                 { swi_call(nAPI_GFX_SCROLL_HORIZ); }
ERROR_CODE API_GFX_SCROLL_VERT      ()                                                                 { swi_call(nAPI_GFX_SCROLL_VERT); }

ERROR_CODE API_PRINTF               (const char * fmt, va_list args)                                   { swi_call(nAPI_PRINTF); }
ERROR_CODE API_SET_FONT             (HFONT hFont)                                                      { swi_call(nAPI_SET_FONT); }
ERROR_CODE API_GET_FONT             (HFONT* phFont)                                                    { swi_call(nAPI_GET_FONT); }
ERROR_CODE API_TEXT                 (const char* pszText, int x, int y)                                { swi_call(nAPI_TEXT); }
ERROR_CODE API_GET_TEXT_RECT        (const char* pszText, GFX_RECT* pRect)                             { swi_call(nAPI_GET_TEXT_RECT); }
ERROR_CODE API_SET_FONT_COLOR       (COLOR nColor)                                                     { swi_call(nAPI_SET_FONT_COLOR); }
ERROR_CODE API_GET_FONT_COLOR       (COLOR* pnColor)                                                   { swi_call(nAPI_GET_FONT_COLOR); }

ERROR_CODE API_RENDER_SCENE         ()                                                                 { swi_call(nAPI_RENDER_SCENE); }

ERROR_CODE API_GFX                  (int cmd, void* p, void * pvData)                                  { swi_call(nAPI_GFX); }
/////////////////////// GFX API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////

int sound_buffer_write             (sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),
            int count,void * param)
{
    sound_api_param api_param;
    api_param.count=count;
    api_param.reader_fct=reader_fct;
    API_SOUND_BUFFER_WRITE(sound_buffer,&api_param,param);
    return api_param.count;
}

ERROR_CODE API_SOUND_BUFFER_WRITE(sound_buffer_s * sound_buffer,sound_api_param  * api_param, void * param)
{
    swi_call(nAPI_SOUND_BUFFER_WRITE);
}

////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

void printf(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    API_PRINTF(fmt,ap);
    va_end(ap);
}

/// TMP !!!

void * malloc(long size)
{
    void * ptr;
    API_MALLOC(&ptr,size);
    return ptr;
}

void free(void *buff)
{
    API_FREE(buff);
}

int do_api_power (int cmd)
{
    int res;
    API_POWER(cmd,&res);
    return res;
}

unsigned int register_evt(void)
{
    unsigned int val;
    API_EVT(0x000,&val,NULL);
    return val;
}

void unregister_evt(unsigned int arg)
{
    API_EVT(0x001,arg,NULL);
}

int waitEvt(unsigned int arg)
{
    int val=0;
    API_EVT(0x002,arg,&val);
    return val;
}

void sendEvt(int evt)
{
    API_EVT(0x003,&evt,NULL);
}

void flushEvt(unsigned int arg)
{
    API_EVT(0x004,arg,NULL);
}
