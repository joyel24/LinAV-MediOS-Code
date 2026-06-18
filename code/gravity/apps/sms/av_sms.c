/* 
*   gui/avwm.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <graphics.h>
#include <kernel/osd.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/time.h>
#include <sys_def/font.h>
#include <api.h>
#include <fs_io.h>
#include <kernel/evt.h>

#include "shared.h"

#define OSD_BITMAP1_WIDTH  320
#define OSD_BITMAP1_HEIGHT 240

#define MAX_ROM_SIZE 1024*1024
#define SMS_BANK_SIZE 16384

#define MAX_VS_POS_X 72
#define MAX_VS_POS_Y 16

#define LCD_POS_X 0xfc
#define LCD_POS_Y 0

extern struct graphicsBuffer BITMAP_1;

void initSmsEmul(void);
int loadRom(char *romname);
void setVirtualScreenPos(void);
void doEmulLoop(void);

#define ROMNAME "/test.sms"

/*int vsPosX=0;
int vsPosY=0;*/

//unsigned char * bmap_offset;


void _start(void)
{
    printf("Starting av sms\n");
        
    open_graphics();
    clearScreen(COLOR_WHITE);
    setFont(STD6X9);
    
    //setPos(BMAP1,0, 0);
        
    initSmsEmul();
    
    if (!loadRom(ROMNAME))
    {
        printf("Error loading %s\n", ROMNAME);
    }
    
    system_init(0);
    
    snd.enabled=false;
    
    printf("system_init done\n");

    /*vsPosX=MAX_VS_POS_X/2;
    vsPosY=MAX_VS_POS_Y/2;*/
    //setVirtualScreenPos();

    
    
    doEmulLoop();
}

void system_load_sram(void)
{
}

void initSmsEmul()
{
  sms.use_fm = false;
  sms.country = TYPE_OVERSEAS;
  sms.save = false;

  
  bitmap.width  = OSD_BITMAP1_WIDTH;
  bitmap.height = OSD_BITMAP1_HEIGHT;
  bitmap.depth  = 8;
  bitmap.pitch  = OSD_BITMAP1_WIDTH;
  bitmap.data   = (unsigned char*)getBufferOffset(BMAP1);
  //bitmap.data   = (unsigned char*)0x0301abc0;

  /*setState(BMAP1,OSD_BITMAP_8BIT | OSD_BITMAP_MERGEBACK | OSD_BITMAP_A7 | OSD_BITMAP_ZX1 | OSD_BITMAP_RAMCLUT);
  showPlane(BMAP1);*/
  
  printf("using GFX buffer at: %x\n", bitmap.data);
  
  snd.enabled=false;
  snd.log=false;
  snd.bufsize=0;

  cart.rom=malloc(MAX_ROM_SIZE);
  memset(cart.rom,0,15);
}

int loadRom(char *romname)
{
  int f,cnt;

  f=fopen(romname,O_RDONLY);
  if (f<0) return false;
  cnt=fread(f,cart.rom,MAX_ROM_SIZE);

  cart.pages=ftell(f)/SMS_BANK_SIZE;
  cart.type=TYPE_SMS;
  if (strstr(romname,".GG")) cart.type=TYPE_GG;

  fclose(f);

  printf("rom loaded, %d pages (%d bytes) => @ %x\n",cart.pages,cnt,cart.rom);
  return cnt>0;
}

void setVirtualScreenPos()
{
  /*setPos(BMAP1,LCD_POS_X-vsPosX, LCD_POS_Y);
  
  //osdSetComponentOffsetA(OSD_BITMAP1, bmap_offset+OSD_BITMAP1_WIDTH*vsPosY);

  //change emu rendering window
  if(cart.type==TYPE_SMS){
    vp_vstart=vsPosY;
    vp_vend=OSD_BITMAP1_HEIGHT-MAX_VS_POS_Y+vsPosY;
  
    vp_hstart=vsPosX;
    if ((vp_hstart%16)>0){
      vp_hstart=(vp_hstart>>4)-2;
    }else{
      vp_hstart=(vp_hstart>>4)-1;
    }

    vp_hend=(OSD_BITMAP1_WIDTH<<1)-MAX_VS_POS_X+vsPosX;
    if (vp_hend&0xf){
      vp_hend=(vp_hend>>4)+1;
    }else{
      vp_hend=vp_hend>>4;
    }
  }else{
    vp_vstart = 24;
    vp_vend   = 168;
    vp_hstart = 6;
    vp_hend   = 26;
  };*/
}

void doEmulLoop()
{
 int i;
 unsigned int ycbcr,y,cb,cr;
//    int  frameSkip=10;

  input.pad[1]=0;
vdp.limit=0;
sms.cyclesperframe=512;

  while(1)
  {
   input.pad[0]=0;

    input.system=0;
    
    if (bitmap.pal.update){
      for(i=0;i<32;++i){
        if(bitmap.pal.dirty[i]){
        
          printf("setting palette (%d,%d,%d) for %d\n",bitmap.pal.color[i][0],bitmap.pal.color[i][1],bitmap.pal.color[i][2],i);
        
          setPalletteRGB(bitmap.pal.color[i][0],bitmap.pal.color[i][1],bitmap.pal.color[i][2],i);
          setPalletteRGB(bitmap.pal.color[i][0],bitmap.pal.color[i][1],bitmap.pal.color[i][2],i+0x20);
          setPalletteRGB(bitmap.pal.color[i][0],bitmap.pal.color[i][1],bitmap.pal.color[i][2],i+0x40);
          bitmap.pal.dirty[i]=0;
        }
      }
      bitmap.pal.update=false;
    }
    
    /*for(i=0;i<frameSkip;++i){
      sms_frame(1);
    }*/

    sms_frame(0);


   

  }
}
