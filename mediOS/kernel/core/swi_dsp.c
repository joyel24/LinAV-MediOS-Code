/* 
*   kernel/core/swi_dsp.c
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
#include <kernel/dsp.h>
#include <kernel/irq.h>

int swi_dsp_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
        
            case nAPI_DSP_OPEN:             //(void* pHandler);
            {
                    g_pDSPHandler = (DSP_HANDLER)nParam1;
                    enable_irq (IRQ_DSP);
            }
            break;
        
            case nAPI_DSP_CLOSE:            //();
            {
                    dsp_off ();
                    disable_irq (IRQ_DSP);
            }
            break;
        
            case nAPI_DSP_LOAD_MEMCODE:     //(void* pCode, int nSize);
            {
                    load_dsp_program_mem ((void*)nParam1, nParam2);
            }
            break;
        
            case nAPI_DSP_LOAD_HDDCODE:     //(const char* pszCoffProgram);
            {
                    load_dsp_program_hdd ((const char*)nParam1);
            }
            break;
        
            case nAPI_DSP_ON:               //();
            {
                    dsp_on ();
            }
            break;
        
            case nAPI_DSP_OFF:              //();
            {
                    dsp_off ();
            }
            break;
        
            case nAPI_DSP_RESET:            //();
            {
                    dsp_reset ();
            }
            break;
        
            case nAPI_DSP_RUN:              //();
            {
                    dsp_run ();
            }
            break;
            
            default:
                printk("dsp undefined swi (%d)\n",nCmd);
        }
        return 0;
}

