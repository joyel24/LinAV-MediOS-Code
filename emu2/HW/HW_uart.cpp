/*
*   HW_null.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <HW_uart.h>

HW_uart::HW_uart(uint32_t start,uint32_t end,char * name):HW_access(start,end,name)
{
    tx=0;
    ty=0;
    for(int i=0;i<TERM_HEIGHT;++i) term[i][0]=0;

    if_r=0;
    if_w=0;

	  skip=0;

    char fname[]="uartN";
    switch(start)
    {
        case UART0_START:
            uartNum=0;
            break;
        case UART1_START:
            uartNum=1;
            break;
        default:
            printf("UART base address is wrong : 0x%08x\n",start);
            return;
    }
    
    fname[4]=0x30+uartNum;
    fd = fopen(fname, "w");
    if (!fd)
        printf("Error: opening: %s for %s\n",fname,name);
    else
    {
        printf("%s init: output send to %s\n",name,fname);
        fprintf(fd,"%s ouptput\n",name);
        fflush(fd);
    }    

    display = XOpenDisplay(0);
    if(!display)
    {
        printf("Error while connecting to X server");
        exit(1);
    }

    screen = DefaultScreen(display);

    gc = DefaultGC(display, screen);

    window = XCreateSimpleWindow(
            display,                               /* Display */
            DefaultRootWindow(display),            /* Main Window */
            0, 0, TERM_WIDTH*FONT_WIDTH+MARGIN*2, TERM_HEIGHT*FONT_HEIGHT+MARGIN*2,     /* Geometry */
            0,                                    /* Width border */
            BlackPixel(display, screen),
            WhitePixel(display, screen)
            );
    if(!window)
    {
        printf("Can't create the window");
        exit(1);
    }
    XStoreName(display, window, name);

    XSelectInput(display, window, StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask);

    XMapWindow(display, window);
}

HW_uart::~HW_uart()
{
    fclose(fd);
}

int HW_uart::nxtEvent()
{   
    #if 1
    ++skip;
    if(skip<10000) return 0;
    skip=0;

    int pending;
    pending = XPending(display);
    KeySym keysym;
    char c = 0;

    if (pending != 0)
    {
      /* next event */
      XNextEvent(display, &event);
      switch (event.type)
      {
        case Expose :
            update_window();
            break;
        case KeyPress :
            XLookupString (&event.xkey, &c, 1, &keysym, 0);
            
            in_fifo[if_w & 0xff]=c;
            ++if_w;
            break;
      }
    }
   #endif
}

uint32_t HW_uart::read(uint32_t addr,int size)
{
    int ret=0;

    switch(addr&0xF)
    {
        case 0x0:
            if(if_r<if_w){
                ret=in_fifo[if_r & 0xff];
                ++if_r;
            }

            return ret;
        case 0x6:
            if (if_r<if_w)
            	  return 0xffff;
            else
            	  return 0;
        case 0xc:
            return 0xffff;
        default:        
            DEBUG_HW(UART_HW_DEBUG,"%s read @0x%08x, size %x\n",name,addr,size);
            break;
    }
    return 0;
}

void HW_uart::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr&0xF)
    {
        case 0x0:
            char c;

            c=val&0xff;

            if(fd){
                fputc(c,fd);
                fflush(fd);
            }
                //DEBUG_HW(UART_HW_DEBUG,"%c",val&0xFF);

            add_char(c);

            break;
        default:
            DEBUG_HW(UART_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
            break;
    }
}

void HW_uart::add_char(char c){
    int full_update=0;

    if (c==10){
        tx=0;
        ++ty;
    }else if (c==13){

    }else{
        term[ty][tx]=c;
        ++tx;
    }

    if (tx>=TERM_WIDTH){
        term[ty][tx]=0;
    	  tx=0;
        ++ty;
    }

    if (ty>=TERM_HEIGHT){
        for (int i=0;i<TERM_HEIGHT-1;++i){
            strcpy(term[i],term[i+1]);
        }

        ty=TERM_HEIGHT-1;

        full_update=1;
    }

    term[ty][tx]=0;

    if (full_update){
        update_window();
    }else{
	      XClearArea(display,window,MARGIN,ty*FONT_HEIGHT+2+MARGIN,TERM_WIDTH*FONT_WIDTH+MARGIN,FONT_HEIGHT,false);
        XDrawString(display,window,gc,MARGIN,ty*FONT_HEIGHT+FONT_HEIGHT+MARGIN,term[ty],strlen(term[ty]));
        XFlush(display);
    }
}

void HW_uart::update_window(){

	  for (int i=0;i<TERM_HEIGHT;++i){
	      XClearArea(display,window,MARGIN,i*FONT_HEIGHT+2+MARGIN,TERM_WIDTH*FONT_WIDTH+MARGIN,FONT_HEIGHT,false);
        XDrawString(display,window,gc,MARGIN,i*FONT_HEIGHT+FONT_HEIGHT+MARGIN,term[i],strlen(term[i]));
    }

    XFlush(display);
}
