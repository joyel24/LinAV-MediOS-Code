/* 
*   kernel/main.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/malloc.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/timer.h>

#include <kernel/uart.h>
#include <kernel/cpld.h>

#include <kernel/version.h>

#include <kernel/ata.h>


int lcd_state=1;
int lcd_bright=10;

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    print_irq();
    print_timer();
}

extern char _real_core_start;
extern char _core_code_start;
extern char _core_code_end;


void kernel_start(void)
{
    int i;    
    char *src;
    char *dst;
    
    /* moving core code to lower RAM */    
    clf();    
    src = &_real_core_start;
    dst=&_core_code_start;    
    while (dst < &_core_code_end) {
          *dst++ = *src++;
    }       
    stf();   /* we need to enable the FIQ so WDT is handled */
    
    ata_stop_HD(); /* to allow nice stop of HD in case of crash */
            
    /* print banner on uart */ 
    printk("AMOS %d.%d - kernel loading\n",VER_MAJOR,VER_MINOR);    
    printk("Initial SP: %08x, kernel end: %08x Malloc start: %08x, size: %08x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);
            
    /* init the irq */
    init_irq(); 
    
    /* init the tick timer */
    init_timer();
    
    /* enable the IRQ */
    sti();       
    printk("[init] int. IRQ enable\n");
    
    /* malloc of max space in SDRAM */
    init_malloc((void*)MALLOC_START,MALLOC_SIZE);
    
    /* driver init */
    init_cpld();
           
    printk("[init] ------------ all drivers\n");
    
    print_boot_info();    
       
    
    while(1)
    {
        printk("C ");
        for(i=0;i<0x10000;i++) /*nothing*/;
    }
    /* exit not allowed */
}
