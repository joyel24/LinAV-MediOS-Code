/* 
*   kernel/core/swi_memory.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/malloc.h>

__IRAM_CODE int swi_memory_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
            case nAPI_MALLOC:   //void** ppvBuffer, unsigned long nBytes
                *((void**)nParam1) = malloc(nParam2);
                printk("malloc of %x bytes at %x\n",nParam2,*((void**)nParam1));
                break;
        
            case nAPI_FREE:     //void* pvBuffer
                free((void*)nParam1);
                break;
                
            case nAPI_MEMAVAIL: //unsigned long* pnBytes
                {
                    long nAllocatedBytes, nFreeBytes, nMaxFree, nAllocCount, nFreeCount;
                    bstats (&nAllocatedBytes, &nFreeBytes, &nMaxFree, &nAllocCount, &nFreeCount);
                    *((unsigned long*)nParam1)=nAllocatedBytes;
                }
                break;
            default:
                printk("Memory undefined swi (%d)\n",nCmd);
        }
        return 0;
}
