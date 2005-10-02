/* 
*   kernel/swi_file.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <api.h>
#include <kernel/threads.h>
#include <kernel/bflat.h>

#include <kernel/kfile.h>
#include <kernel/kdir.h>
#include <kernel/irq.h>

CRITSEC_INFO* g_pCS_FS = 0;

int fs_swi(int cmd,void * data1, void * data2);

__IRAM_CODE int swi_file_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
	case nAPI_RUN_GRV:          //(const char* pGRVPath, HTASK* phTask)
	{
		void* pvTCB = 0;
                int i=0;
		API_MALLOC (&pvTCB, sizeof(TASK_INFO));
		TASK_INFO* pTCB = (TASK_INFO*)pvTCB;
		if (!pTCB)
			return ERR_NOMEMORY;

		kInitialiseTCBVariables (pTCB, 16384 , "U_");

                while(i<7 && ((const char *)nParam1)[i+1] != '\0')
                {
                    pTCB->cName[i+2] = ((const char *)nParam1)[i+1];
                    i++;
                }
                pTCB->cName[9]='\0';
                
                
		ERROR_CODE code = load_bflat ((const char *)nParam1, pTCB);

		void* pStack = 0;
		API_MALLOC (&pStack, 16384);//nStackSize
		pTCB->pStack = (unsigned long*)pStack;

		unsigned char* pTopOfStack = (unsigned char*)pTCB->pStack;
		pTopOfStack += pTCB->nStackSize - 4;
		pTCB->pTopOfStack = kInitialiseStack ((unsigned long*)pTopOfStack, pTCB->pEntry, 0);

		void* pPipe = 0;
		API_MALLOC (&pPipe, sizeof(PIPE));
		pTCB->pMessagePipe = (PIPE*)pPipe;

		pTCB->pMessagePipe->nReceiver = 0;
		pTCB->pMessagePipe->nSender = 0;

		*((TASK_INFO**)nParam2) = pTCB;

		printk ("TASK READY TO START. INCLUDING IN TASK RING...\n");

		// Include new task in task ring...
		__cli ();
		pTCB->pPrevTask = g_pTaskRing;
		pTCB->pNextTask = g_pTaskRing->pNextTask;
		pTCB->pNextTask->pPrevTask = pTCB;
		g_pTaskRing->pNextTask = pTCB;
		__sti ();

		return code;
	}
	break;
        
        case nAPI_FILE:
            return fs_swi((int)nParam1,(void *)nParam2, (void *)nParam3);
	}

	return 0;
}

int fs_swi(int cmd,void * data1, void * data2)
{

    COUPLE_DATA * data=((COUPLE_DATA *)data1);
    off_t * off;
    ssize_t * size;

	if (!g_pCS_FS)
	{
		HCRITSEC hSec = 0;
		API_CRITSEC_CREATE (&hSec);
		g_pCS_FS = (CRITSEC_INFO*)hSec;
	}

	API_CRITSEC_ENTER ((HCRITSEC)g_pCS_FS);

    //printk("FS swi (%d)\n",cmd);

    switch(cmd) {
        case 0x000:            
            *(int*)data2=kfopen((const char*) data1, *(int*)data2);
            break;
        case 0x001:
            *(int*)data2=kfclose((int)data1);
            break;
        case 0x002:
            *(int*)data2=kfsync((int)data1);
            break;
        case 0x003:
            *(ssize_t *)data2=kfread((int)data->a,data->b,*(ssize_t *)data2);
            break;
        case 0x004:
            off=(off_t *)data2;
            *off=klseek((int)data->a,*off,(int)data->b);
            break;
        case 0x005:
            off=(off_t *)data2;
            *off=kftell((int)data1);
            break;
        case 0x006:
            *(int *)data2=kfcreat((const char *)data->a,(mode_t)data->b);
            break;
        case 0x007:
            size=(ssize_t *)data2;
            *size=kfwrite((int)data->a,data->b,*size);
            break;
        case 0x008:
            *(int *)data2=kfremove((const char*)data1);
            break;
        case 0x009:
            *(int *)data2=kfrename((const char*)data->a,(const char*)data->b);
            break;
        case 0x00A:
            *(int *)data2=kftruncate((int)data->a,(off_t)data->b);
            break;
        case 0x00B:
            *(int *)data2=kfilesize((int)data1);
            break;
            
        case 0x100:
            *(DIR**)data2=kopendir((const char*) data1);
            break;
        case 0x101:
            *(int*)data2=kclosedir((DIR*) data1);
            break;
         case 0x102:
            *(int*)data2=kmkdir((const char*) data1, *(int*)data2 );
            break;
         case 0x103:
            *(int*)data2=krmdir((const char*) data1);
            break;
         case 0x104:
            *(struct dirent**)data2=kreaddir((DIR*) data1);
            break;
         default:
             printk("FS undefined swi (%d)\n",cmd);
    }

	API_CRITSEC_LEAVE ((HCRITSEC)g_pCS_FS);

	return 0;
}

