/*

 All files in this archive are subject to the GNU General Public License.
 See the file COPYING in the source tree root for full license agreement.
 This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 KIND, either express of implied.

 SMS/GG emulator (port of SMS Plus by Charles Mac Donald)

 Date:     11/10/2005
 Author:   GliGli

*/

#include "shared.h"
#include <graphics.h>
#include <sys_def/colordef.h>

#include <evt.h>

#include <kernel/io.h>
#include <kernel/osd.h>

#define MAX_VS_POS_X 72
#define MAX_VS_POS_Y 16

#define MAX_ROM_SIZE 1024*1024
#define SMS_BANK_SIZE 16384

#define TICKS_PER_FRAME 167

int vsPosX=0;
int vsPosY=0;

bool autoFrameSkip;
int  frameSkip;
bool ggBorder;
bool swapButtons;


char romname[]="/test.sms";

void initSmsEmul();
bool loadRom(char *romname);
void doEmulLoop();

void setVirtualScreenPos();
void moveVirtualScreen();

void showGGBorder(bool show);

void initDisplay();


int _start(int argc, char ** argv){
  
  printf("In SMS loading: %s\n",romname);
  
  initDisplay();
  initSmsEmul();

  
  
  //memset(romname,'\0',255);

    if (!loadRom(romname))
    {
        printf("Error reading : %s\n",romname);
        return 1;
    }

    system_reset();
    
    /*setPalletteRGB(0, 0, 0, 0);
    setPalletteRGB(0, 0, 0, 0x20);
    setPalletteRGB(0, 0, 0, 0x40);*/

    snd.enabled=false;

    doEmulLoop();

    if(sms.save) printf("Should read SRAM\n");
 

  
  return 1;
}


void initDisplay()
{
  open_graphics();
  clearScreen(COLOR_WHITE);
}



void initSmsEmul(){

memset(&sms,0,sizeof(t_sms));

bitmap.width  = 320;
  bitmap.height = 240;
  bitmap.depth  = 8;
  bitmap.pitch  = 320;
  bitmap.data   = (unsigned char*)getBufferOffset(BMAP1);
  printf("Buffer at : %x\n",bitmap.data);
  
cart.rom=malloc(MAX_ROM_SIZE);

  system_init(0);

  sms.use_fm = false;
  sms.country = TYPE_OVERSEAS;
  sms.save = false;
  sms.paused = false;
  
  

  
  
  snd.enabled=false;
  snd.log=false;
  snd.bufsize=0;


}

bool loadRom(char *romname){
  int f,cnt;

  f=open(romname,O_RDONLY);
  if (f<0) return false;
  
  lseek(f,filesize(f)%SMS_BANK_SIZE,SEEK_SET); // skip header if there is one

  cnt=read(f,cart.rom,MAX_ROM_SIZE);

  cart.pages=cnt/SMS_BANK_SIZE;
  cart.type=TYPE_SMS;

  if (strstr(romname,".GG") || strstr(romname,".gg")) cart.type=TYPE_GG;

  close(f);

  printf("rom loaded, %d pages\n",cart.pages);
  return cnt>0;
}



int RGB2Packed(int r, int g, int b)
{
	return  ((RGB2Cr(r,g,b) << 16) | (RGB2Y(r,g,b) << 8) | RGB2Cb(r,g,b));
}

void osdSetPallette_S (int Y, int Cr, int Cb, int index)
{
	Y&=0xFF;
        Cr&=0xFF;
        Cb&=0xFF;
        
        //printk("OSD set palette (%x,%x,%x) at %d\n",Y,Cr,Cb,index);
        
	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;
        outw((Y << 8) | Cb,OSD_PAL_DATA_WRITE);
	//outw((Cr << 8) | index,OSD_PAL_INDEX_WRITE);

	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;

	//outw((Y << 8) | Cb,OSD_PAL_DATA_WRITE);
        outw((Cr << 8) | index,OSD_PAL_INDEX_WRITE);
        
        while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0)
        /* nothing */ ;
}

void doEmulLoop(){
  int i,evt;
 
unsigned int ycbcr,y,cb,cr;

//unsigned int evt_buffer;
int frameSkip = 1;
int prevTick,frameTickDelta,frameTick;

  input.pad[1]=0;
  vdp.limit=0;
    sms.cyclesperline=512;

  /*evt_buffer=get_evt_pipe();
    if(!evt_buffer)
    {
        printf("[ini_status_bar] can't register to evt\n");
    }*/
    
  prevTick=get_tick();  
  
  do{
        
    input.system=0;

    
    
    //evt=get_evt(evt_buffer);
    
    evt=btn_readState() & 0xF7F;
    input.pad[0] = evt;
    
    input.system = evt;
    
    /*
    switch(evt)
    {
        case BTN_UP:
            input.pad[0] |= INPUT_UP;
            break;
        case BTN_DOWN:
            input.pad[0] |= INPUT_DOWN;
            break;
        case BTN_LEFT:
            input.pad[0] |= INPUT_LEFT;
            break;
        case BTN_RIGHT:
            input.pad[0] |= INPUT_RIGHT;
            break;
        case BTN_ON:
            if (cart.type==TYPE_SMS){
                input.system|=INPUT_PAUSE;
            }else{
                input.system|=INPUT_START;
            }
            break;
        case BTN_F1:
            input.system|=INPUT_HARD_RESET;
            break;
        case BTN_F2:
            input.pad[0] |= INPUT_BUTTON2;
            break;
        case BTN_F3:
            input.pad[0] |= INPUT_BUTTON1;
            break;
    }*/
    
    
    for(i=0;i<frameSkip;++i){
      sms_frame(1);
    }
    
    if (bitmap.pal.update){
      for(i=0;i<32;++i){
        if(bitmap.pal.dirty[i]){
        
        ycbcr=RGB2Packed(bitmap.pal.color[i][0],bitmap.pal.color[i][1],bitmap.pal.color[i][2]);
          y=(ycbcr>>8) & 0xff;
          cb=(ycbcr>>16) & 0xff;
          cr=ycbcr & 0xff;

          osdSetPallette_S(y,cb,cr,i);
          osdSetPallette_S(y,cb,cr,i+0x20);
          osdSetPallette_S(y,cb,cr,i+0x40);
         

          bitmap.pal.dirty[i]=false;
        }
      }
      bitmap.pal.update=false;
    }

    sms_frame(0);

    //printf("tick = %x\n",get_tick());
    frameTick=get_tick()-prevTick;
    frameTickDelta=((frameSkip+1)*TICKS_PER_FRAME)-frameTick;
    //if (frameTickDelta>0) delay(frameTickDelta);
    
      if (frameTickDelta>0) --frameSkip;
      if (frameTickDelta<-10) ++frameSkip;

      //printf("%x %x\n",frameSkip,frameTickDelta);
    prevTick=get_tick();
        
  }while(1);
}



