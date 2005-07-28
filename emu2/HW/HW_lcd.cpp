/* 
*   HW_lcd.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include "X11/keysym.h"

#include <stdlib.h>
#include <stdio.h>

#include <HW_lcd.h>
#include <HW_cpld.h>
#include <mem_space.h>

#include <pallette.h>

extern mem_space * mem;

HW_lcd::HW_lcd(HW_mem * mem2)
{
    int x,y;
    
    this->mem2 = mem2;


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
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,     /* Geometry */
            10,                                    /* Width border */
            BlackPixel(display, screen),	
            WhitePixel(display, screen)
            );            
    if(!window) 
    {
            printf("Can't create the window");
            exit(1);
    }
    XStoreName(display, window, "AV Emu LCD");
    
    pal = DefaultColormap(display,screen);
    
    setPalette(ini_pal,256);   
    
    /* initializing the tmp buffer and the screen */
    XSetForeground(display, gc, colorTab[0]);
    for(y=0; y<SCREEN_HEIGHT; y++)
        for(x=0; x<SCREEN_WIDTH; x++)
        {
            XDrawPoint(display, window, gc, x, y);
        }            
    
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
        
    XMapWindow(display, window);
    
    printf("LCD init done\n");    
}

void HW_lcd::setPalette(int palette[256][3],int size)
{
    int r,g,b,i;
    XColor c;
    
    for(i=0; i<size; i++)
    {
        r = palette[i][0];
        g = palette[i][1];
        b = palette[i][2];
        c.red = r*0x100+r;
        c.green = g*0x100+g;
        c.blue = b*0x100+b;
        XAllocColor(display, pal, &c);
        colorTab[i] = c.pixel;
    }
}

int HW_lcd::nxtEvent(uint32_t addr)
{   
 #if 1
    int pending;
    pending = XPending(display);
    KeySym keysym;
    char c = 0;
    
    
  if(pending != 0)
  {
    
    /* next event */
    XNextEvent(display, &event);    
    int val=BTN_INIT_VAL;
    switch (event.type) 
    {
        case Expose :
            if(addr>=SDRAM_START && addr < SDRAM_END)
                updte_lcd(addr);
            break;
        //case KeyRelease :
            //val=0;
        case KeyPress : 
            printf("keypress ");         
            XLookupString (&event.xkey, &c, 1, &keysym, 0);
            switch(keysym)
            {
                case XK_KP_Left:
                case XK_Left:
                case XK_KP_4:
                    printf("left\n"); 
                    mem->hw_cpld->btn_var[BTN_LEFT]=val;
                    break;
            
                case XK_KP_Right:
                case XK_Right:
                case XK_KP_6:
                    printf("right\n");
                    mem->hw_cpld->btn_var[BTN_RIGHT]=val;
                    break;
            
                case XK_KP_Up:
                case XK_Up:
                case XK_KP_8:
                    printf("up\n");
                    mem->hw_cpld->btn_var[BTN_UP]=val;
                    break;
            
                case XK_KP_Down:
                case XK_Down:
                case XK_KP_2:
                    printf("down\n");
                    mem->hw_cpld->btn_var[BTN_DOWN]=val;
                    break;
                
                case XK_KP_Add:
                case XK_Q:
                case XK_q:
                    printf("ON %d\n",mem->hw_cpld->ON_btn->state);
                    mem->hw_cpld->ON_btn->state=mem->hw_cpld->ON_btn->state==1?0:1;
                    break;
                    
                case XK_KP_Enter:
                case XK_A:
                case XK_a:
                    printf("OFF %d\n",mem->hw_cpld->OFF_btn->state);
                    mem->hw_cpld->OFF_btn->state=mem->hw_cpld->OFF_btn->state==1?0:1;
                    break;
                    
                case XK_KP_Divide:
                case XK_1:
                    printf("F1\n");
                    mem->hw_cpld->btn_var[BTN_F1]=val;
                    break;
                    
                case XK_KP_Multiply:
                case XK_2:
                    printf("F2\n");
                    mem->hw_cpld->btn_var[BTN_F2]=val;
                    break;
                    
                case XK_KP_Subtract:
                case XK_3:
                    printf("F3\n");
                    mem->hw_cpld->btn_var[BTN_F3]=val;
                    break;
            }        
            break;
    }
   }  
   #endif
}

void HW_lcd::drawPix(uint32_t addr,uint32_t val)
{
    int i,j;
    
    addr >>= 1;
    i=addr%SCREEN_WIDTH;
    j=addr/SCREEN_WIDTH;
    XSetForeground(display, gc, colorTab[val&0xFF]);
    XDrawPoint(display, window, gc, i, j);
}

void HW_lcd::updte_lcd(uint32_t base_addr)
{
    int a=0;
    int b=0;
    char data[4];
    uint32_t color;
    
    for(int j = 0 ; j < SCREEN_HEIGHT+1 ; j++)
        for(int i = 0 ; i < SCREEN_WIDTH+1 ; i++)
        {
            XSetForeground(display, gc, colorTab[mem2->read(base_addr+(j*(SCREEN_WIDTH*2)+i*2),2)&0xFF]);
            XDrawPoint(display, window, gc, i, j);
         }
}
