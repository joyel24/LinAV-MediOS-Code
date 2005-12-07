/* 
*   apps/avboy/avboy.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
* 
*  Date:     18/10/2005
* Author:   GliGli

*  Modified by CjNr11 06/12/2005
*/


/* kernel include */
#include <sys_def/string.h>
#include <graphics.h>
#include <evt.h>
#include <kernel/malloc.h>
#include <kernel/buttons.h>
#include <kernel/delay.h>
#include <fs_io.h>
#include <api.h>

/* avboy include */
#include "defs.h"
#include "avboy.h"
#include "regs.h"
#include "lcd.h"
#include "fb.h"
#include "input.h"
#include "pcm.h"
#include "mem.h"
#include "hw.h"
#include "rtc.h"
#include "sound.h"
#include "pcm.h"

#define LCD_WIDTH 320
#define LCD_HEIGHT 240

#define OSD_BITMAP1_WIDTH 160
#define OSD_BITMAP1_HEIGHT 144

#define USER_MENU_QUIT -2

#define OSD_BITMAP1_CONFIG  OSD_COMPONENT_ENABLE | OSD_BITMAP_8BIT | OSD_BITMAP_MERGEBACK | OSD_BITMAP_A7 | OSD_BITMAP_ZX1 | OSD_BITMAP_RAMCLUT

unsigned long OSD_BITMAP1_ADDRESS;

struct fb fb;
struct pcm pcm;

int _start(int argc,char* argv)
{
    char * rom;
    open_graphics();
    OSD_BITMAP1_ADDRESS = getBufferOffset(BMAP1);
    setSize(BMAP1,160,144,8);    
    
    fillRect(0x00,0,0,160,144);
    
    setFont(10);   
    
    vid_init();
    pcm_init();
    
    rom = (char *)malloc(MAX_PATH);
    
    browser(rom);
    printf("Rom name : %s\n",rom);
    
    loader_init(rom);

    emu_reset();
    emu_run();
    
    return 0;
}

void vid_preinit(void){
  printf("vid_preinit\n");
}

void vid_init(void){
  printf("vid_init\n");

  fb.w = OSD_BITMAP1_WIDTH;
  fb.h = OSD_BITMAP1_HEIGHT;
  fb.pelsize = 1;
  fb.pitch = OSD_BITMAP1_WIDTH;
  fb.ptr = (unsigned char *)OSD_BITMAP1_ADDRESS;
  fb.enabled = 1;
  fb.dirty = 0; ///1????
  fb.yuv = 0;
  fb.indexed = 1;
}

int framenum=0;

void vid_begin(void){
//  debug("vid_begin\n");
    framenum=(framenum+1)%2;
    fb.enabled=!framenum;
}
void vid_end(void){
//  debug("vid_end\n");
}
void vid_close(void){
  printf("vid_close\n");
}
void vid_settitle(char * title){
  printf("vid_settitle\n");
}
void vid_setpal(int i, int r, int g, int b)
{

  printf("vid_setpal %d %d %d %d\n",i,r,g,b);
setPalletteRGB(r,g,b,i);

}

int oldbt=0;

int doevents(void)
{
    int bt,pressed,released;
    
    bt=read_btn() & 0xF7F;
    pressed=bt & ~oldbt;
    released=~bt & oldbt;
    oldbt=bt;
    
    if (pressed)
    {
        if(pressed & 0x01)     pad_press(PAD_UP);
        if(pressed & 0x08)   pad_press(PAD_DOWN);
        if(pressed & 0x02)   pad_press(PAD_LEFT);
        if(pressed & 0x04)  pad_press(PAD_RIGHT);
        if(pressed & 0x20)  pad_press(PAD_B);
        if(pressed & 0x40) pad_press(PAD_A);
        if(pressed & 0x100)     pad_press(PAD_START);
        if(pressed & 0x10)  pad_press(PAD_SELECT);
        if(pressed & 0x200) {if (do_user_menu() == USER_MENU_QUIT) 
        {
            cleanup();
            DIE("bye");
        }
     }
    }
    
    if (released)
    {
        if(released & 0x01)     pad_release(PAD_UP);
        if(released & 0x08)   pad_release(PAD_DOWN);
        if(released & 0x02)   pad_release(PAD_LEFT);
        if(released & 0x04)  pad_release(PAD_RIGHT);
        if(released & 0x20)  pad_release(PAD_B);
        if(released & 0x40) pad_release(PAD_A);
        if(released & 0x100)     pad_release(PAD_START);
        if(released & 0x10)  pad_release(PAD_SELECT);
    }
    
    return 1;
}

long tickcounter;

void *sys_timer(void){
  tickcounter=get_tick();
  return &tickcounter;
}

int sys_elapsed(long * oldtick){
  int delta;
  long now;
  now=get_tick();
  delta=now-*oldtick;
  *oldtick=now;
  return (delta*10000);
}

void  sys_sleep(int us){
//  debug("sys_sleep\n");
  mdelay(us/1000);
}

void  sys_checkdir(char *path, int wr){
  printf("sys_checkdir\n");
}

void  sys_initpath(char *exe){
  printf("sys_initpath\n");
}

void  sys_sanitize(char *s){
  printf("sys_sanitize\n");
}

void drawProgress(int offset,int length,int mode){};
