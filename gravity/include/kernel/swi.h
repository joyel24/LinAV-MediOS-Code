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

// swi numbers
#define nAPI_TASK_YIELD       0
#define nAPI_TASK_CREATE      1
#define nAPI_TASK_REMOVE      2
#define nAPI_TASK_SUSPEND     3
#define nAPI_TASK_CONTINUE    4
#define nAPI_TASK_GETHANDLE   5
#define nAPI_SLEEP            6

#define nAPI_MALLOC          20
#define nAPI_FREE            21
#define nAPI_MEMAVAIL        22
#define nAPI_MEMSET          23
#define nAPI_MEMCPY          24

#define nAPI_PIPE_CREATE     30
#define nAPI_PIPE_DELETE     31
#define nAPI_PIPE_SEND       32
#define nAPI_PIPE_RECV       33

#define nAPI_GFX             100
#define nAPI_PRINTF          101

#endif
