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

#include <sys_def/stddef.h>

#include <kernel/api.h>

ERROR_CODE API_TASK_YIELD       ()                                                              { SAVE; asm("swi 0"); LOAD; }

ERROR_CODE API_MALLOC           (void** ppvBuffer, unsigned long nBytes)                        { SAVE; asm("swi 20"); LOAD; }
ERROR_CODE API_FREE             (void* pvBuffer)                                                { SAVE; asm("swi 21"); LOAD; }
ERROR_CODE API_MEMAVAIL         (unsigned long* pnBytes)                                        { SAVE; asm("swi 22"); LOAD; }

ERROR_CODE API_GFX              (int cmd, GFX_DATA * gfxD, void * pvData)                       { SAVE; asm("swi 100"); LOAD; }
