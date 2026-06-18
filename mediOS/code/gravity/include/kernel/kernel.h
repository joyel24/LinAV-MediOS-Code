/* 
*   include/uart.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __KERNEL_H
#define __KERNEL_H

void printk(char *fmt, ...);
void print_data(char * data,int length);

unsigned int get_sp(void);

extern unsigned long tick;

extern unsigned int _end_kernel;
extern unsigned int _iram_start;
extern unsigned int _iram_end;

struct pt_regs {
	long uregs[18];
};

#define HZ 100

#define SWAB16(x) (x)
#define SWAB32(x) (x)

/* section definition */

#define __IRAM_CODE    __attribute__ ((section(".core")))
#define __IRAM_DATA    __attribute__ ((section(".cored")))

#define __SDRAM_CODE   __attribute__ ((section(".text")))
#define __SDRAM_DATA   __attribute__ ((section(".data")))

#endif
