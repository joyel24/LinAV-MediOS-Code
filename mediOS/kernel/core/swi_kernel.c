/* 
*   kernel/swi_kernel.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/evt.h>
#include <api.h>
#include <kernel/irq.h>

int swi_kernel_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
/// Serialize critical API calls to memory manager



        case nAPI_EVT:
            switch((int)nParam1)
            {
                case 0x000:  /* get handling */
                    *(unsigned int *)nParam2=(unsigned int)get_evt_handling();
                    break;
                case 0x001: /* rm handling */
                    rm_evt_handling((struct evt_pipes_s *)nParam2);
                    break;
                case 0x002: /* wait evt */
                    API_PIPE_RECV((HPIPE)(&((struct evt_pipes_s *)nParam2)->evt_pipe), (void*)nParam3 , 1);
                    break;
                case 0x003: /* send evt */
                    send_evt(*(int*)nParam2);
                    break;
                case 0x004: /* flush evt */
                    __cli ();
                    ((struct evt_pipes_s *)nParam2)->evt_pipe.nReceiver = 0;
                    ((struct evt_pipes_s *)nParam2)->evt_pipe.nSender = 0;
                    __sti ();
                    break;                    
            }
            return 0;
      default:
         printk("Unknown SWI call %d\n", nCmd);
		return ERR_NOT_IMPLEMENTED;
	}

	return 0;
}
