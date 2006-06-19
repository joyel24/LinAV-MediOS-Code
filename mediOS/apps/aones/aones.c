#include "medios.h"
#include "unes.h"
#include "unes_io.h"

__IRAM_DATA VirtualNES Vnes;
unsigned char * lj_curRenderingScreenPtr;
char CurrentROMFile[256];

tDspCom * dspCom;

__IRAM_DATA volatile int vblankNum;

// tick of last rendered frame
int prevRenderedFrameTick=0;

// used for fps calculation
int prevFpsTick=0;
int prevFpsFrame=0;

// number of emu frames since reset
int frameCount=0;

// number of frames to skip between 2 rendered frames
int frameSkip=0;




int t=0;
int t2=0;
int i=0;

void clkc_overclockArm(bool over){
#ifdef GMINI_OVERCLOCKING
    if(over){
        //oc
        outw(0x8090,0x30880);
        outw(0x80a0,0x30882);
        //uart 115200bps, ref= 27Mhz
        outw(inw(0x30884)|0x0600,0x30884);
        outw(14,0x30302);
    }else{
        //oc
        outw(0x80e1,0x30880);
        outw(0x8080,0x30882);
        //uart 115200bps, ref= ARM
        outw(inw(0x30884)&(~0x0600),0x30884);
        outw(0x36,0x30302);
    }
#endif
};

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
    ip=(long)lj_curRenderingScreenPtr+/*4*/10*NES_WIDTH*4;

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
    outw(/*231*/220,0x30792); //VSIZE
    outw(/*21*/20,0x307a2); //VRSZ

    outw(0xff00,0x307ce); //SETUPY
    outw(0xff00,0x307d0); //SETUPC

    ccd_vdSim();

    outw(1,0x30780); //PVEN
}

void dsp_interrupt(int irq,struct pt_regs * regs){

    // handle dmc data request from dsp
    if(dspCom->sndDmcWantRead){
        int addr=dspCom->sndDmcAddress;
        int pos=0;
        int len=dspCom->sndDmcLength;

        //printf("dmc req a %x l %d\n",addr,len);

        while(pos<len){
            dspCom->sndDmcData[pos]=Rd6502(addr);

            pos++;
            if(addr!=0xffff){
                addr++;
            }else{
                addr=0x8000;
            }
        }

        dspCom->sndDmcWantRead=0;
    }

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
        printf("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
    }

    if(t){
        i++;
        t2+=tmr_getMicroTick()-t;
        if (i==10){
          printf("%d\n",t2);
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
#if 1
    load_dsp_program_mem(dspcode,len);
#else
    load_dsp_program_hdd("/aones_dsp.out");
#endif

    // setup dma
    outw((inw(0x309a8)&0x03ff)|0x1000,0x309a8); //REFCTL : DMA 2 is used for DSP<>SDRAM

    dsp_run();

    while(!(*DSP_COM)); // wait for the dsp to finish init

    dspCom = (tDspCom *) DSP_RAM(*DSP_COM);
};


void dsp_write32(volatile void * p, uint32 value)
{
    uint16 * ptr = (uint16 *) p;
    ptr[1] = value;
    ptr[0] = value>>16;
}

__IRAM_CODE void osd_interrupt(int irq,struct pt_regs * regs){
    vblankNum++;
}

void snd_init(){
    aic23_setSampleRate(SAMPLE_RATE);
    aic23_setOutputVolume(110,AIC23_CHANNEL_BOTH);
    aic23_enableOutput(true);
}

void snd_close(){
    aic23_enableOutput(false);
}

void emu_init()
{
    memset((char*)&Vnes,0,sizeof(Vnes));

    Vnes.var.LineOffset=malloc(NES_PAL_HEIGHT);

    Vnes.CPUMemory=(uint8 *)malloc(65536);
    Vnes.PPU_patterntables=(uint8 *)malloc(0x8000);
    Vnes.PPU_nametables=(uint8 *)malloc(0x1000);
    Vnes.PPU_palette=(uint8*)malloc(0x100);
  
    Vnes.mapper_extram=(uint8 *)malloc(0x10000);
    Vnes.mapper_extramsize=0;
    Vnes.NESSRAM=(uint8*)malloc(0x10000);

    Vnes.var.Vbuffer=(uint8 *)malloc(NES_BUFFER_WIDTH*NES_PAL_HEIGHT+64);
    Vnes.var.Vbuffer=(char*)(((unsigned int)Vnes.var.Vbuffer+32)&0xffffffe0);
    Vnes.var.Vbuffer2=(uint8 *)malloc(NES_BUFFER_WIDTH*NES_PAL_HEIGHT+64);
    Vnes.var.Vbuffer2=(char*)(((unsigned int)Vnes.var.Vbuffer2+32)&0xffffffe0);

    memset(Vnes.var.Vbuffer,0,NES_BUFFER_WIDTH*NES_PAL_HEIGHT);
    memset(Vnes.var.Vbuffer2,0,NES_BUFFER_WIDTH*NES_PAL_HEIGHT);
    memset(Vnes.var.LineOffset,0,NES_PAL_HEIGHT);
}

void emu_close()
{
    if (Vnes.CPUMemory) free(Vnes.CPUMemory);
    if (Vnes.PPU_patterntables) free(Vnes.PPU_patterntables);
    if (Vnes.PPU_nametables) free(Vnes.PPU_nametables);
    if (Vnes.PPU_palette) free(Vnes.PPU_palette);
    if (Vnes.mapper_extram) free(Vnes.mapper_extram);
    if (Vnes.NESSRAM) free(Vnes.NESSRAM);
    if (Vnes.var.Vbuffer) free(Vnes.var.Vbuffer);
    if (Vnes.var.Vbuffer2) free(Vnes.var.Vbuffer2);
}

void emu_setDefaultParams()
{
    Vnes.var.cpucycle=113;
    Vnes.var.deccycle=65536*200/3;
    Vnes.var.curdeccycle=0;
    Vnes.var.currentcpucycle=0;
    Vnes.var.debugmode=0;

    Vnes.var.morethan8spr=1;
    Vnes.var.DrawAllLines=0;
    Vnes.var.scaleMode=0;
    Vnes.var.autofireA=0;
    Vnes.var.autofireB=0;
    Vnes.var.padmode=0;
    Vnes.var.zapperX=128;
    Vnes.var.zapperY=120;

    Vnes.var.InterPolateSnd=0;
    Vnes.var.emupause=0;
    Vnes.var.fps=60; //ntsc
    Vnes.var.frame_time=100000/Vnes.var.fps;

    Vnes.var.enablesound=0;

    Vnes.var.MasterVolume=127;

    Vnes.var.sndfreq=8;
    Vnes.var.sndfilter=2;
}

void display_init(){
    int x,y;

    vblankNum=0;
    irq_changeHandler(IRQ_OSD,osd_interrupt);
    irq_enable(IRQ_OSD);

    gfx_planeHide(BMAP1);
    gfx_planeHide(BMAP2);
    gfx_planeHide(VID1);
    gfx_planeHide(VID2);

    lj_curRenderingScreenPtr=malloc(NES_WIDTH*NES_PAL_HEIGHT*4+64);
    lj_curRenderingScreenPtr=(char*)(((unsigned int)lj_curRenderingScreenPtr+32)&0xffffffe0);

#ifdef SCREEN_USE_RESIZE
    gfx_planeGetPos(VID1,&x,&y);
    gfx_planeSetPos(VID1,x-8,y);
#else
    gfx_planeSetBufferOffset(VID1,lj_curRenderingScreenPtr);
    gfx_planeSetSize(VID1,NES_WIDTH,NES_PAL_HEIGHT,32);
#endif
};

void emu_run(){
    emu_setDefaultParams();

    if ((!Load_ROM(CurrentROMFile))&&
        (!Open_ROM())&&
        (!Init_NES(CurrentROMFile))){
        LaunchEmu();
    }

    Close_ROM(0);
};

__IRAM_CODE void emu_handleVideoBuffer(){
#ifdef SCREEN_USE_DSP
    byte * tmp;
    int i;

    tmp=Vnes.var.Vbuffer;
    Vnes.var.Vbuffer=Vnes.var.Vbuffer2;
    Vnes.var.Vbuffer2=tmp;

    while(dspCom->inBufReady); // wait for the DSP to finish the last frame

    for(i=0;i<32;++i){
        dspCom->pal[i]=Vnes.PPU_palette[i];
    }

    for(i=0;i<240;++i){
        dspCom->lineOffset[i]=Vnes.var.LineOffset[i];
    }

    dsp_write32(&dspCom->inBufAddr,(uint32)Vnes.var.Vbuffer2);
    dspCom->inBufReady=1;
#else
 #ifdef SCREEN_USE_RESIZE
    ccd_vdSim();
 #endif
#endif
}


int emu_processIngameKeys()
{
  int Key;

  Key=btn_readState();

/*
  if (Key & BTMASK_F2)
  {
     NES_reset();
  }
*/

  if (Key & BTMASK_F2)
  {
     SaveStateSnss(0);
  }

  if (Key & BTMASK_F3)
  {
     LoadStateSnss(0);
  }

  if (Key & BTMASK_OFF)
  {
     return 1;
  }

  return 0;
}

int emu_frameCompleted()
{
    int tick;
    int frameLength;

    frameCount++;

    if (emu_processIngameKeys())
    {
        return 1;
    }

    tick=tmr_getTick();
    if ((tick-prevFpsTick)>=100){
        printf("%d fps\n",frameCount-prevFpsFrame);
        prevFpsTick=tick;
        prevFpsFrame=frameCount;
    }

    if (Vnes.var.DrawCframe){
        // handle frame limit

        frameLength=(frameSkip+1)*Vnes.var.frame_time;
        if (Vnes.var.fps==60){

            //we use the vblank interrupt to get sync
            tick=tmr_getMicroTick();
            if ((tick-prevRenderedFrameTick)<frameLength){
                int vbl=vblankNum;

                while((vblankNum-vbl)<(frameSkip+1)) /* nothing */;

                tick=tmr_getMicroTick();
            }

        }else{

            do{
                tick=tmr_getMicroTick();
            }while((tick-prevRenderedFrameTick)<frameLength);

        }

        prevRenderedFrameTick=tick;
    }

    // handle frame skip
    Vnes.var.DrawCframe=(frameCount%(frameSkip+1))==0;

    return 0;
}


int app_main(){
    snd_init();

    dsp_init();

    iniIcon();
    iniBrowser();

    gfx_openGraphics();
    gfx_clearScreen(COLOR_WHITE);
    gfx_fontSet(STD6X9);

    emu_init();

    display_init();

#ifdef SCREEN_USE_RESIZE
    resize_init();
#endif

    dsp_write32(&dspCom->outBufAddr,(uint32)lj_curRenderingScreenPtr);

    for(;;){
        clkc_overclockArm(false);

        gfx_planeHide(VID1);
        gfx_setPlane(BMAP1);
        gfx_planeShow(BMAP1);

        if(browser_simpleBrowse("/roms/nes",CurrentROMFile)!=MED_OK) break;

        gfx_planeHide(BMAP1);
        gfx_setPlane(VID1);
        gfx_clearScreen(COLOR32_BLACK);
        gfx_planeShow(VID1);

        clkc_overclockArm(true);
        emu_run();

        // temp
        apu_reset();
        while(btn_readState());
    }

    clkc_overclockArm(false);
    gfx_closeGraphics();
    snd_close();

    return 0;
}

