/* 
*   kernel/swi_memory.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
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
