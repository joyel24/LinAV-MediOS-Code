/*
*   HW_uart.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_UART_H
#define __HW_UART_H

#include "emu.h"
#include "HW_access.h"

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define FONT_WIDTH 6
#define FONT_HEIGHT 12
#define TERM_WIDTH 80
#define TERM_HEIGHT 50
#define MARGIN 3

#define UART_START(NUM) (NUM==0?UART0_START:NUM==1?UART1_START:0x0)
#define UART_END(NUM) (NUM==0?UART0_END:NUM==1?UART1_END:0x0)

class HW_uart : public HW_access {
    public:
        HW_uart(int num,char * name);
        ~HW_uart();
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);

        int nxtEvent();

        void add_char(char c);
        void update_window();
    private:
        int uartNum;
        FILE * fd;

        Display* display;
        Window window;
        GC gc;
        int screen;
        XEvent event;

        int skip;

        char term[TERM_HEIGHT][TERM_WIDTH+1];
        int tx;
        int ty;

        char in_fifo[256];
        int if_r;
        int if_w;
};

#endif // __HW_UART_H
