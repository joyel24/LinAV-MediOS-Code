#include "medios.h"
#include "unes.h"

#include "dspshared.h"

#define GMINI_OVERCLOCKING

__IRAM_DATA VirtualNES Vnes;
unsigned char * lj_curRenderingScreenPtr;
char CurrentROMFile[256];

tDspCom * dspCom;

int t=0;
int t2=0;
int i=0;


void ccd_vdInvert(int n){
    while(n--){
      outw(inw(0x30702)^0x0004,0x30702);
    };
}

void ccd_clkInvert(int n){
    while(n--){
      outw(inw(0x30884)^0x2000,0x30884);
    };
}

void ccd_vdSim(){
    ccd_vdInvert(1);
    ccd_clkInvert(4);
    ccd_vdInvert(1);
    ccd_clkInvert(4);
}

void resize_init(){
    long ip,op;

    op=(long)gfx_planeGetBufferOffset(VID1);
    ip=(long)lj_curRenderingScreenPtr+4*NES_WIDTH*4;

    op=(op-SDRAM_START)/32;
    ip=(ip-SDRAM_START)/32;

    outw(0x2800,0x30702); //MODESET
    outw(0x0000,0x30704);
    outw(0x0000,0x30706);
    outw(0x0000,0x30700);

    outw(0,0x30780); //PVEN
    while(inw(0x30780));

    outw(0x000e,0x30782); //PVSET1

    outw(ip>>16,0x30784); //RADDRH
    outw(ip&0xffff,0x30786); //RADDRL
    outw(op>>16,0x30788); //WADDRH
    outw(op&0xffff,0x3078a); //WADDRL

    outw(NES_WIDTH*2,0x3078c); //HSTART
    outw(0,0x30790); //VSTART

    outw(NES_WIDTH*2,0x3078e); //HSIZE
    outw(18,0x307a0); //HRSZ
    outw(231,0x30792); //VSIZE
    outw(21,0x307a2); //VRSZ

    outw(0xff00,0x307ce); //SETUPY
    outw(0xff00,0x307d0); //SETUPC

    ccd_vdSim();

    outw(1,0x30780); //PVEN
}


void dsp_interrupt(int irq,struct pt_regs * regs){

    // frame is completed, launch resizing
    if(dspCom->outBufReady){
      ccd_vdSim();
      dspCom->outBufReady=0;
    }

    // debug message comming from the dsp
    if (dspCom->hasDbgMsg){
        static char str[255];
        int i;

        for(i=0;i<256;++i){
            str[i]=dspCom->dbgMsg[i];
        }
        printk("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
    }

    if(t){
      i++;
      t2+=tmr_getMicroTick()-t;
      if (i==10){
        printk("%d\n",t2);
        t2=0;
        i=0;
      }
      t=0;

    }

    if(dspCom->inBufReady){
      t=tmr_getMicroTick();
    }

};


void dsp_init(){
    extern char _binary_apps_aones_dspcode_aones_dsp_out_start;
    extern char _binary_apps_aones_dspcode_aones_dsp_out_end;
    unsigned char * dspcode=&_binary_apps_aones_dspcode_aones_dsp_out_start;
    int len=&_binary_apps_aones_dspcode_aones_dsp_out_end-&_binary_apps_aones_dspcode_aones_dsp_out_start;

    // dsp irq handler
    irq_changeHandler(IRQ_DSP,dsp_interrupt);
    irq_enable(IRQ_DSP);

    *DSP_COM=0;
    load_dsp_program_mem(dspcode,len);

    // setup dma
    outw((inw(0x309a8)&0x03ff)|0x1000,0x309a8); //REFCTL : DMA 2 is used for DSP<>SDRAM

    dsp_run();

    while(!(*DSP_COM)); // wait for the dsp to finish init

    dspCom = (tDspCom *) DSP_RAM(*DSP_COM);
};


void dsp_write32(void * p, uint32 value)
{
  uint16 * ptr = (uint16 *) p;
  ptr[1] = value;
  ptr[0] = value>>16;
}

void emu_init()
{
  memset((char*)&Vnes,0,sizeof(Vnes));

  Vnes.var.cpucycle=113;
  Vnes.var.deccycle=65536*200/3;
  Vnes.var.curdeccycle=0;
  Vnes.var.currentcpucycle=0;
  Vnes.var.debugmode=0;

  Vnes.var.morethan8spr=0;
  Vnes.var.DrawAllLines=1;
  Vnes.var.scaleMode=0;
  Vnes.var.autofireA=0;
  Vnes.var.autofireB=0;
  Vnes.var.padmode=0;
  Vnes.var.zapperX=128;
  Vnes.var.zapperY=120;

  Vnes.var.InterPolateSnd=0;
  Vnes.var.emupause=0;
  Vnes.var.fps=60; //ntsc
  Vnes.var.frame_time=1000/Vnes.var.fps;

  Vnes.var.enablesound=0;

  Vnes.var.MasterVolume=127;

  Vnes.var.sndfreq=8;
  Vnes.var.sndfilter=2;
}

void emu_initDisplay(){
//    int x,y;

    gfx_planeHide(BMAP1);
    gfx_planeHide(BMAP2);
    gfx_planeHide(VID1);
    gfx_planeHide(VID2);

    Vnes.var.Vbuffer=(uint8 *)malloc(NES_BUFFER_WIDTH*NES_PAL_HEIGHT+64);
    Vnes.var.Vbuffer=(char*)(((unsigned int)Vnes.var.Vbuffer+32)&0xffffffe0);
    Vnes.var.Vbuffer2=(uint8 *)malloc(NES_BUFFER_WIDTH*NES_PAL_HEIGHT+64);
    Vnes.var.Vbuffer2=(char*)(((unsigned int)Vnes.var.Vbuffer2+32)&0xffffffe0);

    Vnes.var.LineOffset=(uint8 *)malloc(NES_PAL_HEIGHT);

    memset(Vnes.var.Vbuffer,0,NES_BUFFER_WIDTH*NES_PAL_HEIGHT);
    memset(Vnes.var.Vbuffer2,0,NES_BUFFER_WIDTH*NES_PAL_HEIGHT);
    memset(Vnes.var.LineOffset,0,NES_PAL_HEIGHT);

    lj_curRenderingScreenPtr=malloc(NES_WIDTH*NES_PAL_HEIGHT*4+64);
    lj_curRenderingScreenPtr=(char*)(((unsigned int)lj_curRenderingScreenPtr+32)&0xffffffe0);

//    gfx_planeGetPos(VID1,&x,&y);
//    gfx_planeSetPos(VID1,x+4,y+4);
};

void emu_run(){
    if ((!Load_ROM(CurrentROMFile))&&(!Open_ROM())&&(!Init_NES(CurrentROMFile))){
        Open_SoundSys(2,0);

        LaunchEmu();
    }else{
        Close_ROM(0);
    }
};

int app_main(){
    gfx_openGraphics();
    gfx_clearScreen(COLOR_WHITE);
    gfx_fontSet(STD6X9);

    dsp_init();

    iniIcon();
    iniBrowser();

#ifdef GMINI_OVERCLOCKING
    //oc
    outw(0x8090,0x30880);
    outw(0x8090,0x30882);
    //uart 115200bps, ref= 27Mhz
    outw(inw(0x30884)|0x0600,0x30884);
    outw(14,0x30302);
#endif

    emu_init();
    emu_initDisplay();

    resize_init();

    dsp_write32(&dspCom->outBufAddr,(uint32)lj_curRenderingScreenPtr);

    for(;;){
        gfx_planeHide(VID1);
        gfx_setPlane(BMAP1);
        gfx_planeShow(BMAP1);

        if(browser_simpleBrowse("/roms/nes",CurrentROMFile)!=MED_OK) break;

        gfx_planeHide(BMAP1);
        gfx_setPlane(VID1);
        gfx_clearScreen(COLOR32_BLACK);
        gfx_planeShow(VID1);

        emu_run();
    }

    gfx_closeGraphics();

#ifdef GMINI_OVERCLOCKING
    //oc
    outw(0x80e1,0x30880);
    //uart 115200bps, ref= ARM
    outw(inw(0x30884)&(~0x0600),0x30884);
    outw(0x36,0x30302);
#endif

    return 0;
}

