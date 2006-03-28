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

*  Modified by CjNr11 08/12/2005
*/

#include "defs.h"

/* kernel include */


#include <sys_def/string.h>
#include <evt.h>
#include <buttons.h>


/* MediOS browser include */
#ifdef USE_MEDIOS_BROWSER
#include <gui/file_browser.h>
#include <kernel/osd.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>
#endif

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

#ifdef GMINI4XX
#define LCD_WIDTH 220
#define LCD_HEIGHT 176
#define X_OFFSET 0x09
#define Y_OFFSET 0x09
#endif

#ifdef AV3XX
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define X_OFFSET 0x14
#define Y_OFFSET 0x12
#endif

#define OSD_BITMAP1_WIDTH 160
#define OSD_BITMAP1_HEIGHT 144

#define USER_MENU_QUIT -2

#define OSD_BITMAP1_CONFIG  OSD_COMPONENT_ENABLE | OSD_BITMAP_8BIT | OSD_BITMAP_MERGEBACK | OSD_BITMAP_A7 | OSD_BITMAP_ZX1 | OSD_BITMAP_RAMCLUT

#ifdef AV3XX
int bt_UP = BTMASK_UP;
int bt_DOWN = BTMASK_DOWN;
int bt_LEFT = BTMASK_LEFT;
int bt_RIGHT = BTMASK_RIGHT;
int bt_A = BTMASK_F2;
int bt_B = BTMASK_F1;
int bt_START = BTMASK_ON;
int bt_SELECT = BTMASK_F3;
int bt_MENU = BTMASK_OFF;
#endif

unsigned long OSD_BITMAP1_ADDRESS;

extern int RotScreen;

struct fb fb;
struct pcm pcm;

#ifdef USE_MEDIOS_BROWSER
extern int gui_pal[256][3];
#endif
int app_main(int argc,char** argv)
{
    char * rom;		
/*
		 to use the medios browser you have to define USE_MEDIOS_BROWSER. This completely changes the way the emu works, it mapps button_off
		 to exit the emulator and return to the browser, While exit in the emu ingame menu still completly exits.
		 Cj tell me if you want to do it this way and also let me know if it works for you?
*/

    int i;
    for(i=0;i<argc;i++) printf("%d:%s\n",i,argv[i]);

#ifdef USE_MEDIOS_BROWSER
	while(1)
	{
		osd_setEntirePalette(gui_pal,256);
		gfx_planeSetSize(BMAP1,LCD_WIDTH,LCD_HEIGHT,8);
		gfx_planeSetPos(BMAP1,X_OFFSET,Y_OFFSET);
		iniIcon();
		gfx_openGraphics();
		clearScreen(COLOR_WHITE);
		gfx_fontSet(STD6X9);
		if(argc<2) {
                   ini_file_browser();
		   rom=browse("/",1);
		}
                else {
                  rom = (char *)malloc(MAX_PATH);
                  strcpy(rom,argv[1]);
                }
		if (rom=='0')
		{
			cleanup();
			reload_firmware();
		}
		gfx_openGraphics();
		OSD_BITMAP1_ADDRESS = (int)gfx_planeGetBufferOffset(BMAP1);
		gfx_planeSetSize(BMAP1,160,144,8);
		gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET);
		
		gfx_fillRect(0x00,0,0,160,144);
		
		gfx_fontSet(10);   

    vid_init();
    pcm_init();

    //rom = (char *)malloc(MAX_PATH);
    printf("Rom name : %s\n",rom);
    
    loader_init(rom);

    emu_reset();
    emu_run();
	} 
#endif
#ifndef USE_MEDIOS_BROWSER
		//this uses the avBoy browser and is the default
    gfx_openGraphics();
    OSD_BITMAP1_ADDRESS = (int)gfx_planeGetBufferOffset(BMAP1);
    gfx_planeSetSize(BMAP1,160,144,8);
		gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET);
    
    gfx_fillRect(0x00,0,0,160,144);

    gfx_fontSet(10);   
    
    vid_init();
    pcm_init();
    
    rom = (char *)malloc(MAX_PATH);
    
    printf("argc = %d, argv = %x\n",argc,argv);

    if(argc<2) browser(rom);
    else strcpy(rom,argv[1]);
    
    //browser(rom);
    printf("Rom name : %s (%x,%x)\n",rom,rom,rom+MAX_PATH);

    loader_init(rom);

    emu_reset();
    emu_run();
#endif
    printf("before return\n");
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
int frameskip=1;

void vid_begin(void){
//  debug("vid_begin\n");
  framenum=(framenum+1)%(frameskip+1);
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

  //printf("vid_setpal %d %d %d %d\n",i,r,g,b);
gfx_setPalletteRGB(r,g,b,i);

}

int oldbt=0;

int doevents(void)
{
    int bt,pressed,released;
    
    bt=btn_readState();// & 0xF7F;
    pressed=bt & ~oldbt;
    released=~bt & oldbt;
    oldbt=bt;

    if (pressed)
    {
#ifdef GMINI4XX
        if(pressed & BTMASK_UP)     pad_press(PAD_UP);
        if(pressed & BTMASK_DOWN)   pad_press(PAD_DOWN);
        if(pressed & BTMASK_LEFT)   pad_press(PAD_LEFT);
        if(pressed & BTMASK_RIGHT)  pad_press(PAD_RIGHT);
        if(pressed & BTMASK_BTN1)  pad_press(PAD_A);
        if(pressed & BTMASK_BTN2) pad_press(PAD_B);
        if(pressed & BTMASK_ON)     pad_press(PAD_START);
#ifdef USE_MEDIOS_BROWSER
        if(pressed & BTMASK_OFF) return 0;
#endif
        if(pressed & BTMASK_F3)  pad_press(PAD_SELECT);
        if(pressed & BTMASK_F1) {if (do_user_menu() == USER_MENU_QUIT)
#endif
#ifdef AV3XX
        if(pressed & bt_UP)     pad_press(PAD_UP);
        if(pressed & bt_DOWN)   pad_press(PAD_DOWN);
        if(pressed & bt_LEFT)   pad_press(PAD_LEFT);
        if(pressed & bt_RIGHT)  pad_press(PAD_RIGHT);
        if(pressed & bt_B)  pad_press(PAD_B);
        if(pressed & bt_A) pad_press(PAD_A);
        if(pressed & bt_START)     pad_press(PAD_START);
        if(pressed & bt_SELECT)  pad_press(PAD_SELECT);
#ifdef USE_MEDIOS_BROWSER
        if(pressed & BTMASK_OFF) return 0;
#endif
        if(pressed & bt_MENU) {if (do_user_menu() == USER_MENU_QUIT)
#endif
        {
            cleanup();
#ifdef GMINI4XX
            reload_firmware();
#endif
#ifdef AV3XX
           return 0;
#endif
        }
     }
    }
    
    if (released)
    {

#ifdef GMINI4XX
        if(released & BTMASK_UP)     pad_release(PAD_UP);
        if(released & BTMASK_DOWN)   pad_release(PAD_DOWN);
        if(released & BTMASK_LEFT)   pad_release(PAD_LEFT);
        if(released & BTMASK_RIGHT)  pad_release(PAD_RIGHT);
        if(released & BTMASK_BTN1)  pad_release(PAD_A);
        if(released & BTMASK_BTN2) pad_release(PAD_B);
        if(released & BTMASK_ON)     pad_release(PAD_START);
        if(released & BTMASK_BTN3)  pad_release(PAD_SELECT);
#endif

#ifdef AV3XX
        if(released & bt_UP)     pad_release(PAD_UP);
        if(released & bt_DOWN)   pad_release(PAD_DOWN);
        if(released & bt_LEFT)   pad_release(PAD_LEFT);
        if(released & bt_RIGHT)  pad_release(PAD_RIGHT);
        if(released & bt_B)  pad_release(PAD_B);
        if(released & bt_A) pad_release(PAD_A);
        if(released & bt_START)     pad_release(PAD_START);
        if(released & bt_SELECT)  pad_release(PAD_SELECT);
#endif
    }
    
    return 1;
}

long tickcounter;

void *sys_timer(void){
  tickcounter=tmr_getTick();
  return &tickcounter;
}

int sys_elapsed(long * oldtick){
  int delta;
  long now;
  now=tmr_getTick();
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
