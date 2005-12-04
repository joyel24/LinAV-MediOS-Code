/* 
*   include/irq.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __TIMER_H
#define __TIMER_H

#include <kernel/io.h>

struct timer_s {
    unsigned long expires;
    void (*action)(void);
    struct timer_s * nxt;
    struct timer_s * prev;
    int trigger;
    char * name;
};

extern void main_timer_action(int irq);

extern void setup_timer(struct timer_s * timer_data,char * name);
extern void rm_timer(struct timer_s * timer_data);
extern void start_timer(struct timer_s * timer_data);
extern void stop_timer(struct timer_s * timer_data);

void init_timer(void);
void print_timer(void);

void arch_init_timer(void);

#define TMR_BASE(TMR_NUM)                 (TIMER0_BASE+0x80*TMR_NUM)

#define SET_TIMER_REGS(VAL,REG,TMR_NUM)   {outw(VAL,TMR_BASE(TMR_NUM)+REG);}
#define GET_TIMER_REGS(REG,TMR_NUM)       (inw(TMR_BASE(TMR_NUM)+REG))

#define TMR0 0
#define TMR1 1
#define TMR2 2
#define TMR3 3

#define SET_TIMER_MODE(MODE,TMR_NUM)     {SET_TIMER_REGS(MODE&0x3,TIMER_MODE,TMR_NUM)}
#define GET_TIMER_MODE(TMR_NUM)          (GET_TIMER_REGS(TIMER_MODE,TMR_NUM)&0x3)

#define SET_TIMER_SEL(MODE,TMR_NUM)      {SET_TIMER_REGS(MODE&0x1,TIMER_SEL,TMR_NUM)}
#define GET_TIMER_SEL(TMR_NUM)           (GET_TIMER_REGS(TIMER_SEL,TMR_NUM)&0x1)

#define SET_TIMER_SCAL(SCAL_VAL,TMR_NUM) {SET_TIMER_REGS(SCAL_VAL&0x3FF,TIMER_SCAL,TMR_NUM)}
#define GET_TIMER_SCAL(TMR_NUM)          (GET_TIMER_REGS(TIMER_SCAL,TMR_NUM)&0x3FF)

#define SET_TIMER_DIV(DIV_VAL,TMR_NUM)   {SET_TIMER_REGS(DIV_VAL&0xFFFF,TIMER_DIV,TMR_NUM)}
#define GET_TIMER_DIV(TMR_NUM)           (GET_TIMER_REGS(TIMER_DIV,TMR_NUM)&0xFFFF)

#define TRIGGER_TIMER(TMR_NUM)           {SET_TIMER_REGS(0x1,TIMER_TRG,TMR_NUM)}

#define GET_TIMER_CNT(TMR_NUM)           (GET_TIMER_REGS(TIMER_CNT,TMR_NUM)&0xFFFF)

#endif
