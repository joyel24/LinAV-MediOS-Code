#include "string.h"
#include <ata.h>
#include <fat.h>
#include <file.h>
#include <system.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <debug.h>
#include <usb.h>
#include <timers.h>
#include <uart.h>

#include "../utils/filebrowser.h"
#include "../utils/menu.h"
#include "../utils/colors.h"
#include "../utils/utils.h"

#include "shared.h"

#define REG_ARM_CLOCK (REG 0x30880)

#define REG_LCD_VSYNC (REG 0x3082C)

#define LCD_WIDTH 220
#define LCD_HEIGHT 176

#define OSD_BITMAP1_WIDTH 256
#define OSD_BITMAP1_HEIGHT 192

#define OSD_BITMAP2_WIDTH 240
#define OSD_BITMAP2_HEIGHT 176

#define OSD_BITMAP1_CONFIG  OSD_COMPONENT_ENABLE | OSD_BITMAP_8BIT | OSD_BITMAP_MERGEBACK | OSD_BITMAP_A7 | OSD_BITMAP_ZX1 | OSD_BITMAP_RAMCLUT
#define OSD_BITMAP2_CONFIG  OSD_COMPONENT_ENABLE | OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS
#define OSD_CURSOR1_CONFIG  OSD_COMPONENT_ENABLE | 7<<OSD_CURSOR1_BORDERHEIGHT_SHIFT | 7<<OSD_CURSOR1_BORDERWIDTH_SHIFT | OSD_CURSOR1_RAMCLUT | 0<<OSD_CURSOR1_COLOR_SHIFT

#define OSD_BITMAP1_ADDRESS   0x018b0000
#define OSD_BITMAP1_ADDRESS_2 0x018c0000
#define OSD_BITMAP2_ADDRESS   0x018d1500

#define LCD_POS_X 0xfc
#define LCD_POS_Y 0

#define MAX_VS_POS_X 72
#define MAX_VS_POS_Y 16

#define MAX_ROM_SIZE 1024*1024
#define SMS_BANK_SIZE 16384

#define TICKS_PER_FRAME 167

char _real_iram_start;
char _iram_start;
char _iram_end;

static struct graphicsBuffer osdBmp1;
static struct graphicsBuffer osdBmp2;

static int pal[2] = {COL_BLACK, COL_WHITE};

static struct graphicsBuffer f5x7  = {0, 1, 5, 7, 1, 0, -1, 0, 0, 0, 0, (int**) &pal, (int**) NULL};
static struct graphicsBuffer f6x9  = {0, 1, 6, 9, 1, 0, -1, 0, 0, 0, 0, (int**) &pal, (int**) NULL};
static struct graphicsBuffer f8x13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal, (int**) NULL};

int vsPosX=0;
int vsPosY=0;

bool autoFrameSkip;
int  frameSkip;
bool ggBorder;

tMenuItem ingameMenu[]={
  {"",ITEMTYPE_DUMMY,0,0,0},
  {"Frame skip options:",ITEMTYPE_DUMMY,0,0,0},
  {"Auto frame skip",ITEMTYPE_BOOLEAN,true,0,0},
  {"Frame skip",ITEMTYPE_INTEGER,1,0,10},
  {"",ITEMTYPE_DUMMY,0,0,0},
  {"Emulation options:",ITEMTYPE_DUMMY,0,0,0},
  {"Sprite limit",ITEMTYPE_BOOLEAN,false,0,0},
  {"Expand GG screen",ITEMTYPE_BOOLEAN,false,0,0},
  {"CPU cycles / line",ITEMTYPE_INTEGER,256,0,512},
  {"",ITEMTYPE_DUMMY,0,0,0},
  {"Gmini overclock (at your own risk!):",ITEMTYPE_DUMMY,0,0,0},
  {"Enable overclocking",ITEMTYPE_BOOLEAN,false,0,0},
  {"Overclocking rate",ITEMTYPE_INTEGER,0,0,5},
  {"",ITEMTYPE_DUMMY,0,0,0},
  {"Return to game",ITEMTYPE_ACTION,1,0,0}
};

void iramCopyContent();

void initDisplay();
void showOSD(int osd);
void initHDD();

void drawHelp();

void initSmsEmul();
bool loadRom(char *romname);
void doEmulLoop();

void setVirtualScreenPos();
void moveVirtualScreen();

void showGGBorder(bool show);

void doIngameMenu();
void applyConfig();
void overclockARM(int rate);

int main(){
  static char romname[256];

  // disable all interrupts except the wachdog one
  *REG 0x30520=8;
  *REG 0x30522=0;
  *REG 0x30524=0;
  *REG 0x30528=8;
  *REG 0x3052A=0;
  *REG 0x3052C=0;

  initTickCount();

  iramCopyContent();

  bpool(0x1000000,0x600000);

  initDisplay();
  graphicsBoxfA(&osdBmp2,0,0,OSD_BITMAP1_WIDTH,OSD_BITMAP1_HEIGHT,COL_BLACK);
  graphicsStringA(&osdBmp2, 0, 0, &f8x13, std8x13_, 8, 0,"Init...");
  showOSD(OSD_BITMAP2);

  initHDD();

  initSmsEmul();

  for(;;){
    overclockARM(0); // safer not to overclock during HDD access

    drawHelp();
    showOSD(OSD_BITMAP2);

    if (!browseForFile(romname,&osdBmp2,&f6x9,std6x9_,NULL)) break;

    if (!loadRom(romname)) continue;

    system_init(0);
    debug("system_init done\n");

    graphicsBoxfA(&osdBmp1,0,0,OSD_BITMAP2_WIDTH,OSD_BITMAP2_HEIGHT,0x00);
    showOSD(OSD_BITMAP1);

    ataSleepCmdA();

    applyConfig();

    // center virtual screen
    vsPosX=MAX_VS_POS_X/2;
    vsPosY=MAX_VS_POS_Y/2;
    setVirtualScreenPos();

    doEmulLoop();
  }

  overclockARM(0); // restore original ARM clock
  graphicsBoxfA(&osdBmp2,0,0,OSD_BITMAP1_WIDTH,OSD_BITMAP1_HEIGHT,COL_BLACK);
  graphicsStringA(&osdBmp2, 0, 0, &f8x13, std8x13_, 8, 0,"That's all folks ;)");

  return 1;
}
void iramCopyContent(){
  unsigned char *iram,*iram_end,*sdram;
  int len;

  iram=&_iram_start;
  iram_end=&_iram_end;
  len=(iram_end-iram) + 1;
  sdram=&_real_iram_start;

  debug("iramCopyContent() iram=%0.8X sdram=%0.8X len=%d\n",&_iram_start,&_real_iram_start,len);

  for(;len;--len){
    *iram++=*sdram++;
  }
}

void initDisplay(){
  // tweak lcd parameters (screen flip bug workaround)
  *REG_LCD_VSYNC=0x8;

  osdSetComponentConfigA(OSD_VIDEO1, 0);
  osdSetComponentConfigA(OSD_VIDEO2, 0);
  osdSetComponentConfigA(OSD_BITMAP1, 0);
  osdSetComponentConfigA(OSD_BITMAP2, 0);
  osdSetComponentConfigA(OSD_CURSOR1, 0);
  osdSetComponentConfigA(OSD_CURSOR2, 0);

  osdBmp1.offset = OSD_BITMAP1_ADDRESS;
  osdBmp1.bytesPerLine = OSD_BITMAP1_WIDTH;
  osdBmp1.width = OSD_BITMAP1_WIDTH;
  osdBmp1.height = OSD_BITMAP1_HEIGHT;
  osdBmp1.bitsPerPixelShift = 4;
  osdBmp1.bitsPerPixel = 8;

  graphicsBoxfA(&osdBmp1,0,0,OSD_BITMAP1_WIDTH,OSD_BITMAP1_HEIGHT,COL_BLACK);

  osdBmp2.offset = OSD_BITMAP2_ADDRESS;
  osdBmp2.bytesPerLine = OSD_BITMAP2_WIDTH*2;
  osdBmp2.width = OSD_BITMAP2_WIDTH;
  osdBmp2.height = OSD_BITMAP2_HEIGHT;
  osdBmp2.bitsPerPixelShift = 4;
  osdBmp2.bitsPerPixel = 16;

  graphicsBoxfA(&osdBmp2,0,0,OSD_BITMAP2_WIDTH,OSD_BITMAP2_HEIGHT,COL_BLACK);


  osdSetMainConfigA(OSD_COMPONENT_ENABLE);
//  osdSetBacklightA(1);

  osdSetComponentSizeA(OSD_BITMAP1, OSD_BITMAP1_WIDTH*2, OSD_BITMAP1_HEIGHT);
  osdSetComponentPositionA(OSD_BITMAP1, LCD_POS_X, LCD_POS_Y);
  osdSetComponentOffsetA(OSD_BITMAP1, OSD_BITMAP1_ADDRESS);
  osdSetComponentSourceWidthA(OSD_BITMAP1, 8);

  osdSetComponentSizeA(OSD_BITMAP2, OSD_BITMAP2_WIDTH*2, OSD_BITMAP2_HEIGHT);
  osdSetComponentPositionA(OSD_BITMAP2, LCD_POS_X, LCD_POS_Y);
  osdSetComponentOffsetA(OSD_BITMAP2, OSD_BITMAP2_ADDRESS);
  osdSetComponentSourceWidthA(OSD_BITMAP2, 15);

  osdSetComponentSizeA(OSD_CURSOR1, 174*2, 159);
  osdSetComponentPositionA(OSD_CURSOR1, LCD_POS_X+16*2, LCD_POS_Y+2);
}

void showOSD(int osd){
  osdSetComponentConfigA(OSD_BITMAP1, 0);
  osdSetComponentConfigA(OSD_BITMAP2, 0);
  showGGBorder(false);

  if (osd==OSD_BITMAP1){
    osdSetComponentConfigA(OSD_BITMAP1,OSD_BITMAP1_CONFIG);
  }else if (osd==OSD_BITMAP2){
    osdSetComponentConfigA(OSD_BITMAP2,OSD_BITMAP2_CONFIG);
  }
}

void initHDD(){
  inifile();
  inidir();
  inifatinfo();

  ataSelectHDDA();
  delay(1000);

  ataPowerUpHDDA();
  ataResetHDDA();
  delay(5000);

  ataReadMBR();

  fatInit(getPartition(0));
};

void drawHelp(){
  graphicsBoxfA(&osdBmp2,0,0,OSD_BITMAP2_WIDTH,OSD_BITMAP2_HEIGHT,COL_BLACK);
  pal[1]=COL_GREEN;
  graphicsStringA(&osdBmp2, 115, 0, &f8x13, std8x13_, 8, 0,"GminiSMS v0.3");
  pal[1]=COL_CYAN;
  graphicsStringA(&osdBmp2, 115, 15, &f5x7, std5x7_, 5, 0,"by GliGli");
  pal[1]=COL_WHITE;
  graphicsStringA(&osdBmp2, 115, 30, &f5x7, std5x7_, 5, 0,"A Master System/");
  graphicsStringA(&osdBmp2, 115, 37, &f5x7, std5x7_, 5, 0,"GameGear emulator");
  graphicsStringA(&osdBmp2, 115, 44, &f5x7, std5x7_, 5, 0,"based on SMS Plus by");
  graphicsStringA(&osdBmp2, 115, 51, &f5x7, std5x7_, 5, 0,"Charles MacDonald");

  graphicsStringA(&osdBmp2, 115, 64, &f5x7, std5x7_, 5, 0,"Browser keys:");
  graphicsStringA(&osdBmp2, 115, 71, &f5x7, std5x7_, 5, 0,"-Up/Down: move");
  graphicsStringA(&osdBmp2, 115, 78, &f5x7, std5x7_, 5, 0,"-Left: up one level");
  graphicsStringA(&osdBmp2, 115, 85, &f5x7, std5x7_, 5, 0,"-Right/Square:");
  graphicsStringA(&osdBmp2, 115, 92, &f5x7, std5x7_, 5, 0," open dir/rom");
  graphicsStringA(&osdBmp2, 115, 99, &f5x7, std5x7_, 5, 0,"-Off: quit");

  graphicsStringA(&osdBmp2, 115, 112, &f5x7, std5x7_, 5, 0,"Ingame keys:");
  graphicsStringA(&osdBmp2, 115, 119, &f5x7, std5x7_, 5, 0,"-On: MS pause/");
  graphicsStringA(&osdBmp2, 115, 126, &f5x7, std5x7_, 5, 0," GG start");
  graphicsStringA(&osdBmp2, 115, 133, &f5x7, std5x7_, 5, 0,"-F1: options menu");
  graphicsStringA(&osdBmp2, 115, 140, &f5x7, std5x7_, 5, 0,"-F2: reset");
  graphicsStringA(&osdBmp2, 115, 147, &f5x7, std5x7_, 5, 0,"-F3+Up/Dn/Lt/Rt:");
  graphicsStringA(&osdBmp2, 115, 154, &f5x7, std5x7_, 5, 0,"  move MS screen");
  graphicsStringA(&osdBmp2, 115, 161, &f5x7, std5x7_, 5, 0,"-Off: go to browser");
}

void initSmsEmul(){
  sms.use_fm = false;
  sms.country = TYPE_OVERSEAS;
  sms.save = false;

  bitmap.width  = OSD_BITMAP1_WIDTH;
  bitmap.height = OSD_BITMAP1_HEIGHT;
  bitmap.depth  = 8;
  bitmap.pitch  = OSD_BITMAP1_WIDTH;
  bitmap.data   = (unsigned char*)OSD_BITMAP1_ADDRESS;

  snd.enabled=false;
  snd.log=false;
  snd.bufsize=0;

  cart.rom=bget(MAX_ROM_SIZE);
}

bool loadRom(char *romname){
  int f,cnt;

  f=fopen(romname,"r");
  if (f<0) return false;
  cnt=fread(f,cart.rom,MAX_ROM_SIZE);

  cart.pages=ftell(f)/SMS_BANK_SIZE;
  cart.type=TYPE_SMS;
  if (strstr(romname,".GG")) cart.type=TYPE_GG;

  fclose(f);

  debug("rom loaded, %d pages\n",cart.pages);
  return cnt>0;
}

void doEmulLoop(){
  int i,prevTick,frameTickDelta,frameTick;
  int bt;
  int framenum,plop;
  unsigned int ycbcr,y,cb,cr;

  framenum=0;
  plop=0;
  input.pad[1]=0;
  prevTick=getTickCount();
  do{
    bt=buttonsGetStatusA();
    input.pad[0]=((bt>>4) & 0xf) | ((bt>>8) & 0xf0);

    input.system=0;

    if (bt&BUTTONS_GM400_ON){
      if (cart.type==TYPE_SMS){
        input.system|=INPUT_PAUSE;
      }else{
        input.system|=INPUT_START;
      }
    }

    if (bt&BUTTONS_GM400_MENU1){
      doIngameMenu();
    }

    if (bt&BUTTONS_GM400_MENU2){
      input.system|=INPUT_HARD_RESET;
    }

    if (cart.type==TYPE_SMS && bt&BUTTONS_GM400_MENU3){
      moveVirtualScreen();
    }

    for(i=0;i<frameSkip;++i){
      sms_frame(1);
    }

/*    plop=!plop;
    if (plop){
      osdSetComponentOffsetA(OSD_BITMAP1, OSD_BITMAP1_ADDRESS_2);
      bitmap.data = (unsigned char*)OSD_BITMAP1_ADDRESS;
    }else{
      osdSetComponentOffsetA(OSD_BITMAP1, OSD_BITMAP1_ADDRESS);
      bitmap.data = (unsigned char*)OSD_BITMAP1_ADDRESS_2;
    }
*/

    if (bitmap.pal.update){
      for(i=0;i<32;++i){
        if(bitmap.pal.dirty[i]){
          ycbcr=graphicsRGB2PackedA(bitmap.pal.color[i][0],bitmap.pal.color[i][1],bitmap.pal.color[i][2]);

          y=(ycbcr>>8) & 0xff;
          cb=(ycbcr>>16) & 0xff;
          cr=ycbcr & 0xff;

          osdSetPalletteA(y,cb,cr,i);
          osdSetPalletteA(y,cb,cr,i+0x20);
          osdSetPalletteA(y,cb,cr,i+0x40);

          bitmap.pal.dirty[i]=false;
        }
      }
      bitmap.pal.update=false;
    }

    sms_frame(0);
/*    t=timersGetValueA(0);
    sms_frame(0);
    debug("%d\n",timersGetValueA(0)-t);*/

    frameTick=getTickCount()-prevTick;
    frameTickDelta=((frameSkip+1)*TICKS_PER_FRAME)-frameTick;
//    debug("%d %d\n",frameTickDelta,frameSkip);
    if (frameTickDelta>0) delay(frameTickDelta);
    if (autoFrameSkip){
      if (frameTickDelta>0) --frameSkip;
      if (frameTickDelta<-20) ++frameSkip;
    };
    prevTick=getTickCount();

    framenum++;
  }while(!(bt&BUTTONS_GM400_OFF));
}

void setVirtualScreenPos(){
  // change plane position
  osdSetComponentPositionA(OSD_BITMAP1, LCD_POS_X-vsPosX, LCD_POS_Y);
  osdSetComponentOffsetA(OSD_BITMAP1, OSD_BITMAP1_ADDRESS+OSD_BITMAP1_WIDTH*vsPosY);

  //change emu rendering window
  if(cart.type==TYPE_SMS || !ggBorder){
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
  };
}

void moveVirtualScreen(){
  int bt;

  do{
    bt=buttonsWaitAndRepeat(2500,500,-1);

    if(bt&BUTTONS_GM400_UP) vsPosY-=1;
    if(bt&BUTTONS_GM400_DOWN) vsPosY+=1;
    if(bt&BUTTONS_GM400_LEFT) vsPosX-=2;
    if(bt&BUTTONS_GM400_RIGHT) vsPosX+=2;

    if(vsPosX<0) vsPosX=0;
    if(vsPosY<0) vsPosY=0;

    if(vsPosX>MAX_VS_POS_X) vsPosX=MAX_VS_POS_X;
    if(vsPosY>MAX_VS_POS_Y) vsPosY=MAX_VS_POS_Y;

    setVirtualScreenPos();
  }while(bt&BUTTONS_GM400_MENU3);
}

void showGGBorder(bool show){
  if (cart.type==TYPE_GG && show){
    graphicsBoxfA(&osdBmp1,0,0,OSD_BITMAP1_WIDTH,OSD_BITMAP1_HEIGHT,0); // clear border
    osdSetComponentConfigA(OSD_CURSOR1,OSD_CURSOR1_CONFIG);
  }else{
    osdSetComponentConfigA(OSD_CURSOR1,0);
  }
}

void doIngameMenu(){
  showOSD(OSD_BITMAP2);
  showMenu("GminiSMS ingame menu (paused):",ingameMenu,sizeof(ingameMenu)/sizeof(tMenuItem),&osdBmp2,&f6x9,std6x9_,NULL);
  showOSD(OSD_BITMAP1);

  applyConfig();
}

void applyConfig(){
  autoFrameSkip=ingameMenu[2].value;
  frameSkip=ingameMenu[3].value;

  vdp.limit=ingameMenu[6].value;
  ggBorder=ingameMenu[7].value==0;
  sms.cyclesperframe=ingameMenu[8].value;

  if (ingameMenu[11].value){ // overclock enabled?
    overclockARM(ingameMenu[12].value);
  }else{
    overclockARM(0);
  }

  showGGBorder(ggBorder);
  setVirtualScreenPos();
}

void overclockARM(int rate){
  if(rate==0){
    *REG_ARM_CLOCK=0x80E1; // default ARM clock
  }else{
    *REG_ARM_CLOCK=(0x8070+(rate<<4));
  }
}

void drawProgress(int offset,int length,int mode){};

void system_load_sram(void)
{
}

