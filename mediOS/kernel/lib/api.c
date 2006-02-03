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


///////////////////////////////////////////////////////
////////////////////// MEMORY API /////////////////////
MED_RET_T API_MALLOC   (void** ppvBuffer, unsigned long nBytes)                 { swi_call(nAPI_MALLOC); }
MED_RET_T API_FREE     (void* pvBuffer)                                         { swi_call(nAPI_FREE); }
MED_RET_T API_MEMAVAIL (unsigned long* pnBytes)                                 { swi_call(nAPI_MEMAVAIL); }
MED_RET_T API_REALLOC  (void** ppvBuffer, void* pvBuffer, unsigned long nBytes) { swi_call(nAPI_REALLOC); }

///////////////////////////////////////////////////////
////////////////////// FILE API ///////////////////////
MED_RET_T API_FILE     (int cmd,void * data1,void * data2)     { swi_call(nAPI_FILE); }
MED_RET_T API_RUN_GRV  (const char* pGRVPath, HTASK* phTask)   { swi_call(nAPI_RUN_GRV); }

////////////////////// FILE API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// DEVICE API //////////////////////
MED_RET_T API_GET_TICK             (unsigned long * val)                { swi_call(nAPI_GET_TICK); }
MED_RET_T API_TIME                 (int cmd,struct av_tm * time_data)   { swi_call(nAPI_TIME); }
MED_RET_T API_POWER                (int cmd,int * val)                  { swi_call(nAPI_POWER); }

MED_RET_T udelay                   (int val)                            { swi_call(nAPI_UDELAY); }
MED_RET_T mdelay                   (int val)                            { swi_call(nAPI_MDELAY); }

MED_RET_T API_SET_LCD_BRIGHTNESS   (int nBrightness)                    { swi_call(nAPI_SET_LCD_BRIGHTNESS); }
MED_RET_T API_GET_LCD_BRIGHTNESS   (int* pnBrightness)                  { swi_call(nAPI_GET_LCD_BRIGHTNESS); }

MED_RET_T API_EVT_GET_HANDLER      (unsigned int mask, int * evt_handler)  { swi_call(nAPI_GET_EVT_PIPE); }
MED_RET_T API_EVT_FREE_HANDLER     (int evt_handler)                    { swi_call(nAPI_RM_EVT_PIPE); }
MED_RET_T API_EVT_GET              (int evt_handler,int * data)         { swi_call(nAPI_GET_EVT); }
MED_RET_T API_BTN_GET              (int * data)                         { swi_call(nAPI_GET_BTN); }
MED_RET_T API_BKPT                 (void)                               { swi_call(nAPI_BKPT); }

MED_RET_T API_SET_GET_INT_TIMER    (int type,int mode,TRI_DATA * pvData) { swi_call(nAPI_IO_INT_TIMER); }

MED_RET_T API_EXIT                 (int code)                           { swi_call(nAPI_EXIT); }

///////////////////// DEVICE API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
////////////////////// SOUND API //////////////////////
MED_RET_T API_SOUND_PLAY           (void* pvBuffer, unsigned long nBytes, unsigned long nFlags)       { swi_call(nAPI_SOUND_PLAY); }
MED_RET_T API_SOUND_PAUSE          ()                                                                 { swi_call(nAPI_SOUND_PAUSE); }
MED_RET_T API_SOUND_STOP           ()                                                                 { swi_call(nAPI_SOUND_STOP); }
MED_RET_T API_DSP                  (int cmd, void * arg)                                              { swi_call(nAPI_DSP); }
MED_RET_T API_MIXER                (int cmd, int dir, void * arg)                                     { swi_call(nAPI_MIXER); }
int sound_buffer_write             (sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),
            int count,void * param)
{
    sound_api_param api_param;
    api_param.count=count;
    api_param.reader_fct=reader_fct;
    API_SOUND_BUFFER_WRITE(sound_buffer,&api_param,param);
    return api_param.count;
}

MED_RET_T API_SOUND_BUFFER_WRITE(sound_buffer_s * sound_buffer,sound_api_param  * api_param, void * param)
{
    swi_call(nAPI_SOUND_BUFFER_WRITE);
}
////////////////////// SOUND API //////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// GFX API ///////////////////////

MED_RET_T API_PRINTF               (const char * fmt, va_list args)                                   { swi_call(nAPI_PRINTF); }
MED_RET_T API_GFX                  (int cmd, void* p, void * pvData)                                  { swi_call(nAPI_GFX); }

/////////////////////// GFX API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/////////////////////// DSP API ///////////////////////
MED_RET_T API_DSP_OPEN             (void* pHandler)             { swi_call(nAPI_DSP_OPEN); }
MED_RET_T API_DSP_CLOSE            ()                           { swi_call(nAPI_DSP_CLOSE); }
MED_RET_T API_DSP_LOAD_MEMCODE     (void* pCode, int nSize)     { swi_call(nAPI_DSP_LOAD_MEMCODE); }
MED_RET_T API_DSP_LOAD_HDDCODE     (const char* pszCoffProgram) { swi_call(nAPI_DSP_LOAD_HDDCODE); }
MED_RET_T API_DSP_ON               ()    { swi_call(nAPI_DSP_ON); }
MED_RET_T API_DSP_OFF              ()    { swi_call(nAPI_DSP_OFF); }
MED_RET_T API_DSP_RESET            ()    { swi_call(nAPI_DSP_RESET); }
MED_RET_T API_DSP_RUN              ()    { swi_call(nAPI_DSP_RUN); }
/////////////////////// DSP API ///////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////// LIB FLOAT ///////////////////////
MED_RET_T API_MODSI3      (long a, long b,long * res)    { swi_call(nAPI_MODSI3); }
MED_RET_T API_DIVSI3      (long a, long b,long * res)    { swi_call(nAPI_DIVSI3); }
MED_RET_T API_UMODSI3     (long a, long b,long * res)    { swi_call(nAPI_UMODSI3); }
MED_RET_T API_UDIVSI3     (long a, long b,long * res)    { swi_call(nAPI_UDIVSI3); }
MED_RET_T API_DIVDI3      (unsigned int a, unsigned int b,unsigned int * res)    { swi_call(nAPI_DIVDI3); }
#ifdef BUILD_LIB
long __modsi3(long a, long b)
{
    long res=0;
    API_MODSI3(a,b,&res);
    return res;
}

long __divsi3(long a, long b)
{
    long res=0;
    API_DIVSI3(a,b,&res);
    return res;
}

long __umodsi3(long a, long b)
{
    long res=0;
    API_UMODSI3(a,b,&res);
    return res;
}

long __udivsi3(long a, long b)
{
    long res=0;
    API_UDIVSI3(a,b,&res);
    return res;
}

unsigned int __divdi3(unsigned int a, unsigned int b)
{
    unsigned int res=0;
    API_DIVDI3(a,b,&res);
    return res;
}
#endif
///////////////////////////////////////////////////////
///////////////////// LIB FLOAT ///////////////////////


void printf(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    API_PRINTF(fmt,ap);
    va_end(ap);
}

/// TMP !!!



void exit(int code)
{
    API_EXIT(code);
}

void * malloc(long size)
{
    void * ptr;
    API_MALLOC(&ptr,size);
    return ptr;
}

void * realloc(void *buff,long size){
    void * ptr;
    API_REALLOC(&ptr,buff,size);
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

int evt_get_handler(unsigned int mask)
{
    int val;
    API_EVT_GET_HANDLER(mask, &val);
    return val;
}

int evt_get(int evt_pipe)
{
    int c=0;
    API_EVT_GET(evt_pipe,&c);
    return c;
}

int btn_get(void)
{
    int c=0;
    API_BTN_GET(&c);
    return c;
}

unsigned long get_tick(void)
{
    unsigned long ret = 0;
    API_GET_TICK(&ret);
    return ret;
}

void set_timer_status(int timer_type, int power_mode, int status)
{
    TRI_DATA pvData;
    pvData.a = timer_type;
    pvData.b = power_mode;
    pvData.c = status;
    API_SET_GET_INT_TIMER(1,1,&pvData);
}

void set_timer_delay(int timer_type, int power_mode, int delay)
{
    TRI_DATA pvData;
    pvData.a = timer_type;
    pvData.b = power_mode;
    pvData.c = delay;
    API_SET_GET_INT_TIMER(1,0,&pvData);
}

int get_timer_status(int timer_type, int power_mode)
{
    TRI_DATA pvData;
    pvData.a = timer_type;
    pvData.b = power_mode;
    pvData.c = 0;
    API_SET_GET_INT_TIMER(0,1,&pvData);
    return pvData.c;
}

int get_timer_delay(int timer_type, int power_mode)
{
    TRI_DATA pvData;
    pvData.a = timer_type;
    pvData.b = power_mode;
    pvData.c = 0;
    API_SET_GET_INT_TIMER(0,0,&pvData);
    return pvData.c;
}
