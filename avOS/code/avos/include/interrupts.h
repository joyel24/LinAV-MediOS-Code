/* interrupts.h
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/
#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

#define INTERRUPTS_TIMER0   0
#define INTERRUPTS_TIMER1   1
#define INTERRUPTS_TIMER2   2
#define INTERRUPTS_TIMER3   3

#define INTERRUPTS_OSD      7   // Seems to just interrupt really fast
                                // Does this mean end of osd update?
#define INTERRUPTS_UART0    12  // When a character is recieved...
#define INTERRUPTS_UART1    13

#define INTERRUPTS_EXT0     18  // ON button when enabled in gio
#define INTERRUPTS_EXT1     19
#define INTERRUPTS_EXT2     20
#define INTERRUPTS_EXT3     21
#define INTERRUPTS_EXT4     22  // gio4 (EOD)


void interruptsSetMaskA(u32 bitmask);
u32 interruptsGetMaskA();
void interruptsSetCausesA(u32 bitmask);
u32 interruptsGetCausesA();
void interruptsSetMask2A(u32 bitmask);
u32 interruptsGetMask2A();
void interruptsSetCauses2A(u32 bitmask);
u32 interruptsGetCauses2A();
void interruptsInitA(void* intFunction);
u32 interruptsGetIRQEnabledA();
void interruptsSetIRQEnabledA();
void interruptsSetIRQDisabledA();
void interruptsResetIRQA(u32 num);
void interruptsResetMaskA(u32 num);

