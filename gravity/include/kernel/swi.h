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

// swi numbers
#define nAPI_TASK_YIELD         0
#define nAPI_TASK_CREATE        1
#define nAPI_TASK_SUSPEND       2
#define nAPI_TASK_CONTINUE      3
#define nAPI_TASK_GETHANDLE     4
#define nAPI_TASK_SLEEP         5
#define nAPI_TASK_SENDMESSAGE   6
#define nAPI_TASK_PEEKMESSAGE   7
#define nAPI_TASK_WAITMESSAGE   8
#define nAPI_TASK_TERMINATE     9
#define nAPI_TASK_SETPRIORITY  10

#define nAPI_MALLOC            20
#define nAPI_FREE              21
#define nAPI_MEMAVAIL          22
#define nAPI_MEMSET            23
#define nAPI_MEMCPY            24
#define nAPI_HEAP_CREATE       25
#define nAPI_HEAP_DESTROY      26
#define nAPI_HEAP_MALLOC       27
#define nAPI_HEAP_FREE         28
#define nAPI_HEAP_AVAIL        29

#define nAPI_PIPE_CREATE       30
#define nAPI_PIPE_DELETE       31
#define nAPI_PIPE_SEND         32
#define nAPI_PIPE_RECV         33
#define nAPI_PIPE_TEST         34

#define nAPI_CRITSEC_CREATE    40
#define nAPI_CRITSEC_DELETE    41
#define nAPI_CRITSEC_ENTER     42
#define nAPI_CRITSEC_LEAVE     43
#define nAPI_CRITSEC_TRYENTER  44

#define nAPI_TIME              50
#define nAPI_POWER             51
#define nAPI_MIXER             52
#define nAPI_DSP               53
#define nAPI_EVT               54

#define nAPI_RUN_GRV           60

#define nAPI_GFX              100
#define nAPI_PRINTF           101
#define nAPI_FILE             102

ERROR_CODE swi_call             (long swi_num,long param1,long param2,long param3) ;

#endif
