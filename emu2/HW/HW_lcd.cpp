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
#include <stdlib.h>
#include <stdio.h>

#include <HW_lcd.h>

#include <pallette.h>

//#define LCD_UPDATE(x,y,w,h)       {lcd_update(UPDATE_ONLY,x,y,w,h);}

HW_lcd::HW_lcd(HW_mem * mem)
{
    int x,y;
    
    this->mem = mem;

    SCREEN_WIDTH = 320;
    SCREEN_HEIGHT = 240;

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
    
    XMapWindow(display, window);
    
    printf("LCD init done\n");
    nxtEvent(); // call needed to force exposure event
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

int HW_lcd::nxtEvent(void)
{   
 
    int pending;
    pending = XPending(display);
    KeySym keysym;
    unsigned char c = 0;
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
    
  if(pending != 0)
  {
    /* next event */
    XNextEvent(display, &event);    
    
    switch (event.type) 
    {
      case Expose :
          break;
      case KeyPress :
          break;
      case KeyRelease :
      break;
    }
   }  
}

void HW_lcd::updte_lcd(uint32_t base_addr)
{
    int a=0;
    int b=0;
    char data[4];
    
    for(int j = 0 ; j < SCREEN_HEIGHT ; j++)
    {
        for(int i = 0 ; i < SCREEN_WIDTH ; i++)        
        {
            /*for(int k=0;k<4;k++)
                data[3-k]=mem->read(base_addr+(j*SCREEN_WIDTH+i+k)*2,1);
            for(int k=0;k<4;k++)
            {*/
                if(mem->read(base_addr+(j*SCREEN_WIDTH+i),1) == 0)
                {
                    XSetForeground(display, gc, BlackPixel(display,screen));
                    a++;
                }
                else
                {
                    XSetForeground(display, gc, WhitePixel(display,screen));
                    b++;
                }
                /*XSetForeground(display, gc, colorTab[mem->read(base_addr+j*SCREEN_WIDTH+i,1)]);*/
                printf("drawing at (%d,%d)index = %x (addr=%x)\n",i,j,
                   mem->read(base_addr+j*SCREEN_WIDTH+i,1),
                    base_addr+j*SCREEN_WIDTH+i);
                XDrawPoint(display, window, gc, i, j);
            //}
        }
    }
    printf("pix0:%d pix1:%d\n",a,b);
}
