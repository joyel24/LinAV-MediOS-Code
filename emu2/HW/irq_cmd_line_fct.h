/* 
*   irq_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include "cmd_line.h"

int do_irq_fiq_cmd_s(int argc,char ** argv) 
{ 
    if(argc>0)
    {
        int num = my_atoi(argv[0]);
        return irq_obj->do_IRQ_cmd(FIQ,num);
    }
    return 0;
}

int do_irq_irq_cmd_s(int argc,char ** argv) 
{ 
    if(argc>0)
    {
        int num = my_atoi(argv[0]);
        return irq_obj->do_IRQ_cmd(IRQ,num);
    }
    return 0;
}


int HW_IRQ::do_IRQ_cmd(int type,int num)
{
    do_IRQ_FIQ(type,num);
    return 0;
}

void init_irq_static_fct(HW_IRQ * irq)
{
    irq_obj = irq;
    add_cmd_fct("fiq",do_irq_fiq_cmd_s,"Emulates fiq");
    add_cmd_fct("irq",do_irq_irq_cmd_s,"Emulates irq");
}
