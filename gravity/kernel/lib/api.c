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
            "stmdb sp!, {r3-r12} \n"     \
            "swi %0\n"                   \
            "ldmia sp!, {r3-r12}"        \
        :                      \
        : "i"(SWI_NUM)                   \
        : "memory");                     \
        return (long) _r0;               \
    })    



ERROR_CODE API_TASK_YIELD       ()                                                              { swi_call(nAPI_TASK_YIELD); }

ERROR_CODE API_MALLOC           (void** ppvBuffer, unsigned long nBytes)                        { swi_call(nAPI_MALLOC); }
ERROR_CODE API_FREE             (void* pvBuffer)                                                { swi_call(nAPI_FREE); }
ERROR_CODE API_MEMAVAIL         (unsigned long* pnBytes)                                        { swi_call(nAPI_MEMAVAIL); }

ERROR_CODE API_GFX              (int cmd, GFX_DATA * gfxD, void * pvData)                       { swi_call(nAPI_GFX); }

ERROR_CODE API_PRINTF           (const char * fmt, va_list args)                                { swi_call(nAPI_PRINTF); }

void printf(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    API_PRINTF(fmt,ap);
    va_end(ap);
}


