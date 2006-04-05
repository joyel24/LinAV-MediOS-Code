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
#include <sys_def/string.h>

#include "api_fct_list.h"

void badFctName(void)
{
    printk("BAD fct \n");
}

unsigned int swi_handler (
	unsigned long nCmd,
	unsigned long nParam2,
	unsigned long nParam3,
	unsigned long nParam1)
{
        if(nCmd == nAPI_INIAPI)
        {
            int i=0;
            char * name = (char*)nParam1;
            while(fct_array[i].name!=NULL)
            {
                if(!strcmp(fct_array[i].name,name))
                {
                    //printk("found %s @ 0x%08x\n",fct_array[i].name,fct_array[i].ptr);
                    return fct_array[i].ptr;
                }
                i++;
            }            
            printk("fct %s not found\n",name);
            return (unsigned int)badFctName;            
        }
        else
	  // VP removed that for now because I'm flooded by some unknown swi (related to dsp probably)
	  ;//printk("Unknown SWI %x\n",nCmd);
        return 0;
}
