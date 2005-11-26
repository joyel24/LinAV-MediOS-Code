/* 
*   kernel/core/swi.c
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


__IRAM_CODE int kcswi_handler (
	unsigned long nCmd,
	unsigned long nParam2,
	unsigned long nParam3,
	unsigned long nParam1)
{
    switch (nCmd >> 8)
    {
        case nAPI_GFX_section_code:
                return swi_gfx_handler (nCmd, nParam1, nParam2, nParam3);
    
        case nAPI_SND_section_code:
                return swi_snd_handler (nCmd, nParam1, nParam2, nParam3);
    
        case nAPI_FILE_section_code:
                return swi_file_handler (nCmd, nParam1, nParam2, nParam3);
    
        case nAPI_DEVICE_section_code:
                return swi_device_handler (nCmd, nParam1, nParam2, nParam3);
    
        case nAPI_MEMORY_section_code:
                return swi_memory_handler (nCmd, nParam1, nParam2, nParam3);
    
        case nAPI_DSP_section_code:
                return swi_dsp_handler (nCmd, nParam1, nParam2, nParam3);
    
        default:
                printk("Unknown SWI module call %x\n", nCmd);
    }
	return 0;
}
