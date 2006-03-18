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

int lcd_update_cnt[2] = {0,0};

HW_lcd::HW_lcd(HW_mem * mem2,HW_OSD * osd)
{
#ifdef HAS_LCD
    int x,y;

    skip=0;
#ifdef USE_CACHE
    in_cache = 0;
    cache_size = 0;
#endif

    this->mem2 = mem2;
    this->osd = osd;
    
    display = XOpenDisplay(0);  
    if(!display) 
    {
            printf("Error while connecting to X server");
            exit(1);
    }
                
    screen = DefaultScreen(display);
    gc = DefaultGC(display, screen);
    
    window1 = XCreateSimpleWindow(
            display,                               /* Display */
            DefaultRootWindow(display),            /* Main Window */
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,     /* Geometry */
            0,                                    /* Width border */
            BlackPixel(display, screen),	
            WhitePixel(display, screen)
            );
    if(!window1 ) 
    {
            printf("Can't create BMAP0 windows");
            exit(1);
    }
    XStoreName(display, window1, "AV Emu LCD - BMAP1");
    
#ifdef HAS_VID0            
    window2 = XCreateSimpleWindow(
            display,                               /* Display */
            DefaultRootWindow(display),            /* Main Window */
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,     /* Geometry */
            0,                                    /* Width border */
            BlackPixel(display, screen),	
            WhitePixel(display, screen)
            );            
    if(!window2) 
    {
            printf("Can't create VID0 windows");
            exit(1);
    }
   
    
    XStoreName(display, window2, "AV Emu LCD - VID1");
#endif    

    pal = DefaultColormap(display,screen);
    
    setPalette(ini_pal,256);   
    
    /* initializing the tmp buffer and the screen */
    XSetForeground(display, gc, colorTab[0]);
    for(y=0; y<SCREEN_HEIGHT; y++)
        for(x=0; x<SCREEN_WIDTH; x++)
        {
            XDrawPoint(display, window1, gc, x, y);
        }            
    
    XSelectInput(display, window1, ExposureMask | KeyPressMask | KeyReleaseMask);
    XMapWindow(display, window1);
    
#ifdef HAS_VID0     
    XSelectInput(display, window2, ExposureMask);
    XMapWindow(display, window2);
#endif     
#endif
    printf("LCD init done\n");
}

void HW_lcd::setPalette(int palette[256][3],int size)
{
    int r,g,b,i,Y,Cb,Cr;
    XColor c;
    
    for(i=0; i<size; i++)
    {
        Y = palette[i][0];
        Cb = palette[i][1];
        Cr = palette[i][2];
        r=(int)(Y+1.402*(Cr-128));
        g=(int)(Y-0.34414*(Cb-128)-0.71414*(Cr-128));
        b=(int)(Y+1.772*(Cb-128));
        setPalette(r,g,b,i);
    }
}

int HW_lcd::setPalette(int r,int g, int b, int index)
{
#ifdef HAS_LCD
    XColor c;
        
    c.red = r*0x100+r;
    c.green = g*0x100+g;
    c.blue = b*0x100+b;
    XAllocColor(display, pal, &c);
    colorTab[index] = c.pixel;    
    return c.pixel;
#else
    return 0;
#endif
}

int HW_lcd::nxtEvent(int * config,uint32_t * addr)
{   
#ifdef HAS_LCD
    ++skip;
    if(skip<10000) return 0;
    skip=0;

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
            if(config[2]&0x1 && addr[2]>SDRAM_START)
                updte_lcd(addr[2],LCD_BMAP);
#ifdef HAS_VID0
            if(config[1]&0x1 && addr[0]>SDRAM_START)
                updte_lcd(addr[0],LCD_VID);
#endif
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
                case XK_KP_5:
                    printf("ok\n");
                    mem->hw_cpld->btn_var[BTN_OK]=val;
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
    
    addr >>= (osd->OSD_config_regs[2] & 0xc00)==0?1:0;
    i=(addr%SCREEN_WIDTH);
    j=addr/SCREEN_WIDTH;
    XSetForeground(display, gc, colorTab[val&0xFF]);
    XDrawPoint(display, window1, gc, i, j);
}

void HW_lcd::drawVidPix(uint32_t addr,uint32_t val)
{
#ifdef HAS_VID0
    int i,j;
    
    addr >>= 2;
    i=addr%SCREEN_WIDTH;
    j=addr/SCREEN_WIDTH;
    XSetForeground(display, gc, getColor(val));
    XDrawPoint(display, window2, gc, i, j);
#endif
}



void HW_lcd::updte_lcd(uint32_t base_addr,int type)
{
#ifdef HAS_LCD
    int a=0;
    int b=0;
    char data[4];
    uint32_t color;
    lcd_update_cnt[type]++;
    if(type == LCD_BMAP)
    {
        for(int j = 0 ; j < SCREEN_HEIGHT+1 ; j++)
            for(int i = 0 ; i < SCREEN_WIDTH+1 ; i++)        
            {
                color = colorTab[mem2->read(base_addr+(j*(SCREEN_WIDTH)+i)*((osd->OSD_config_regs[2] & 0xc00)==0?2:1),1)&0xFF];
                XSetForeground(display, gc, color);
                XDrawPoint(display, window1, gc, i, j);
            }
    }
#ifdef HAS_VID0    
    else if(type == LCD_VID)
    {
        for(int j = 0 ; j < SCREEN_HEIGHT+1 ; j++)
            for(int i = 0 ; i < SCREEN_WIDTH+1 ; i++)        
            {
                color = getColor(mem2->read(base_addr+(j*(SCREEN_WIDTH*4)+i*4),4));
                XSetForeground(display, gc, color);
                XDrawPoint(display, window2, gc, i, j);
            }
            
    }
#endif
#endif
}

uint32_t HW_lcd::getColor(uint32_t color)
{
    XColor c;
    int r,g,b,Y,Cr,Cb;
    int i,j;
    
    
    
#ifdef USE_CACHE
    //printf("getColor: asked: %x (cache size=%x cache pos=%x) ",color,cache_size,in_cache);
    /*if(cache_size>PX_CACHE_SIZE)
    {
        printf("error in cache_size\n");
        exit(0);
    }*/
    // trying to find already computed value
    for(i=0;i<cache_size;i++)
        if(pixel_cache[i][0]==color)
        {
            //printf("-- find %x (cache size=%x cache pos=%x)\n",pixel_cache[i][1],cache_size,in_cache);
            return pixel_cache[i][1];            
        }
#endif    
    Cb=color&0xFF;
    Y=(color>>8)&0xFF;
    Cr=(color>>16)&0xFF;
    r=(int)(Y+1.402*(Cr-128));
    g=(int)(Y-0.34414*(Cb-128)-0.71414*(Cr-128));
    b=(int)(Y+1.772*(Cb-128));
    c.red = r*0x100+r;
    c.green = g*0x100+g;
    c.blue = b*0x100+b;
    XAllocColor(display, pal, &c);

#ifdef USE_CACHE   
    pixel_cache[in_cache][0]=color;
    pixel_cache[in_cache][1]=c.pixel;
    in_cache++;
    if(cache_size<PX_CACHE_SIZE)
        cache_size++;
    if(in_cache>=PX_CACHE_SIZE)
        in_cache=0;
    //printf("-- find %x (cache size=%x cache pos=%x)\n",c.pixel,cache_size,in_cache);
#endif    
    return c.pixel;
}
