/* 
*   include/irq.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __TIMER_H
#define __TIMER_H

struct timer_s {
    unsigned long expires;
    void (*action)(void);
    struct timer_s * nxt;
    struct timer_s * prev;
    int trigger;
    char * name;
};

void init_timer(void);
void setup_timer(struct timer_s * timer_data,char * name);
void rm_timer(struct timer_s * timer_data);
void start_timer(struct timer_s * timer_data);
void stop_timer(struct timer_s * timer_data);

void print_timer(void);

#endif
