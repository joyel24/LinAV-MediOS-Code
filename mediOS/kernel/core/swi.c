/* 
*   kernel/swi.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>

/* used by swi_device */
#include <kernel/usb_fw.h>
#include <kernel/rtc.h>
#include <kernel/bat_power.h>

/* used by swi_dsp */
#include <kernel/irq.h>
#include <kernel/dsp.h>

/* used by swi_memory */
#include <kernel/malloc.h>

/* used by swi_sound */
#include <kernel/sound.h>

__IRAM_CODE int kcswi_handler (
	unsigned long nCmd,
	unsigned long nParam2,
	unsigned long nParam3,
	unsigned long nParam1)
{
    switch (nCmd)
    {
        #include "swi_device.c.h"
        
        #include "swi_dsp.c.h"
        
        //#include "swi_file.c.h"
        
        //#include "swi_kernel.c.h"
        
        #include "swi_memory.c.h"

        default:
            printk("Unknown SWI cmd call %d, module %d\n", nCmd, nCmd>>8);
    }

	return 0;
}
