/* 
*   include/irqs.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __IRQS_H
#define __IRQS_H

#define NR_IRQS              32

#define IRQ_TMR_0            0
#define IRQ_TMR_1            1
#define IRQ_TMR_2            2
#define IRQ_TMR_3            3

#define IRQ_OSD              7 // interrupt really fast, related to OSD

#define IRQ_DSP              11

#define IRQ_UART0            12 
#define IRQ_UART1            13

#define IRQ_IDE              15  // ide

#define IRQ_BTN_ON           18  // GIO0

#define IRQ_MAS_DATA         22  // GIO4
#define IRQ_IR_DVR           24  // GIO6

#endif
