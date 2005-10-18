/* 
*   kernel/swi_memory.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

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
