#include "medios.h"
#include "aones.h"
#include "aones_gui.h"
#include "nes_apu.h"
#include "unes.h"
#include "unes_io.h"

__IRAM_DATA VirtualNES Vnes;
unsigned char * lj_curRenderingScreenPtr;
char CurrentROMFile[256]="";

tDspCom * dspCom;

__IRAM_DATA volatile int vblankNum;

// tick of last rendered frame
int prevRenderedFrameTick=0;

// vblank # of last rendered frame
int prevVblankNun=0;

// used for fps calculation
int prevFpsTick=0;
int prevFpsFrame=0;

// number of emu frames since reset
int frameCount=0;

// number of frames to skip between 2 rendered frames
int frameSkip=0;

// buttons stuff
int autoFire=0;
int f3Use=0;
bool buttonsSwap=false;
bool stickyAPressed=false;
bool stickyBPressed=false;
bool prevF3Pressed=false;
bool autoFirePressed=false;

// OC stuff
bool overclocking=false;
int armFrequency=0;
int dspFrequency=0;


void clk_overclock(bool en){
#ifdef GMINI_OVERCLOCKING
    int dspf;

    if(en && overclocking){
        // dsp freq must always be higher than arm freq or equal
        dspf=MAX(armFrequency,dspFrequency);
        clkc_setClockFrequency(CLK_DSP,dspf*1000000);

        clkc_setClockFrequency(CLK_ARM,armFrequency*1000000);
    }else{
        // default params
        clkc_setClockParameters(CLK_DSP,9,1,2);
        clkc_setClockParameters(CLK_ARM,15,2,2);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
    }
#endif
};

void resize_init(){
    long ip,op;

    op=(long)gfx_planeGetBufferOffset(VID1);
    ip=(long)lj_curRenderingScreenPtr+/*4*/10*NES_WIDTH*4;

    resize_setup(ip,NES_WIDTH*2,NES_WIDTH*2,220,op,224*2,176,RESIZE_CONTINUOUS);
}

int t=0;
int t2=0;
int i=0;

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
        resize_execute();
        dspCom->outBufReady=0;
    }

    // debug message coming from the dsp
    if (dspCom->hasDbgMsg){
        static char str[255];
        int i;

        for(i=0;i<256;++i){
            str[i]=dspCom->dbgMsg[i];
        }
        printf("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
    }

#ifdef DSP_VID_PROFILE
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
#endif
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
    dsp_loadProgramFromMemory(dspcode,len);
#else
    dsp_loadProgramFromHDD("/aones_dsp.out");
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
    aic23_enableOutput(true);
}

void snd_close(){
    aic23_enableOutput(false);
}

void emu_init()
{
    memset((char*)&Vnes,0,sizeof(Vnes));

    Vnes.var.LineOffset=malloc(NES_PAL_HEIGHT*sizeof(uint32));

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
    memset(Vnes.var.LineOffset,0,NES_PAL_HEIGHT*sizeof(uint32));

}

void emu_close()
{
    if (Vnes.var.LineOffset) free(Vnes.var.LineOffset);
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

    stickyAPressed=false;
    stickyBPressed=false;
}

void display_init(){
    int x,y,w,h,bpp;

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
    gfx_planeGetSize(VID1,&w,&h,&bpp);
    gfx_planeSetPos(VID1,x-8,y);
    gfx_planeSetSize(VID1,w+4,h,bpp);
#else
    gfx_planeSetBufferOffset(VID1,lj_curRenderingScreenPtr);
    gfx_planeSetSize(VID1,NES_WIDTH,NES_PAL_HEIGHT,32);
#endif
};

void emu_loadRom(){
    Load_ROM(CurrentROMFile);
};

void emu_run(){
    emu_setDefaultParams();

    if ((!Open_ROM())&&
        (!Init_NES(CurrentROMFile))){

        // load a possible sram file
        LoadSaveSRAM(false);

        LaunchEmu();

        if(Vnes.var.SaveRAM){
            aic23_setOutputVolume(0,AIC23_CHANNEL_BOTH); // mute sound
            gui_showText("Saving SRAM...");

            LoadSaveSRAM(true);
        }
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
    resize_execute();
 #endif
#endif
}

long emu_joypad1State(){
    long state;
    int bt;
    state=0;

    if ((Vnes.var.padmode==0)||(Vnes.var.padmode==2)){

        bt=btn_readState();

        if (Vnes.var.padmode==0){
            if(bt & BTMASK_UP) state|=0x10;
            if(bt & BTMASK_DOWN) state|=0x20;
            if(bt & BTMASK_LEFT) state|=0x40;
            if(bt & BTMASK_RIGHT) state|=0x80;

            if (buttonsSwap){
                if(bt & BTMASK_BTN1) state|=2;
                if(bt & BTMASK_BTN2) state|=1;
            }else{
                if(bt & BTMASK_BTN1) state|=1;
                if(bt & BTMASK_BTN2) state|=2;
            }

            if(bt & BTMASK_F3){
                switch(f3Use){
                    case 0: //A+B
                        state|=3;
                        break;
                    case 1: //Sticky A
                        if (!prevF3Pressed){ // only on F3 button state change
                            stickyAPressed=!stickyAPressed;
                        }
                        break;
                    case 2: //Sticky B
                        if (!prevF3Pressed){ // only on F3 button state change
                            stickyBPressed=!stickyBPressed;
                        }
                        break;
                }
                prevF3Pressed=true;
            }else{
                prevF3Pressed=false;
            }

            if (stickyAPressed) state|=1;
            if (stickyBPressed) state|=2;

            if (autoFire){
                if (state&1 && autoFire&1 && !autoFirePressed) state&=0xfffe;
                if (state&2 && autoFire&2 && !autoFirePressed) state&=0xfffd;
            }
        }

        if(bt & BTMASK_ON) state|=8;
        if(bt & BTMASK_F2) state|=4;
    }

    state|=0x10000;           //1player signature
    return(state);
}

long emu_joypad2State(){
    long state;
    if (Vnes.var.padmode!=1) return 0x20000;
    state=0;
    state|=0x20000;           //1player signature
    return(state);
}

int emu_processIngameKeys()
{
  int bt;

  bt=btn_readState();

  if (bt & BTMASK_F1)
  {
     aic23_setOutputVolume(0,AIC23_CHANNEL_BOTH); // mute sound
     gui_execute();
     gui_applySettings();
  }

  if (bt & BTMASK_OFF)
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

                while((vblankNum-prevVblankNun)<(frameSkip+1)) /* nothing */;

                prevVblankNun=vblankNum;

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
    
    // handle autofire
    if(autoFire && frameCount%AUTOFIRE_INTERVAL==0) autoFirePressed=!autoFirePressed;

    return 0;
}


int app_main(){
    // disable LCD & halt timer
    set_timer_status(LCD_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(LCD_TIMER,TIMER_MODE_DC,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_DC,MODE_DISABLE);

    // create dirs if they don't exist
    mkdir(AONES_PATH,-1);
    mkdir(SAVES_PATH,-1);

    // init sound
    snd_init();

    // init dsp
    dsp_init();

    // init graphics
    gfx_openGraphics();
    gfx_clearScreen(COLOR_WHITE);
    gfx_fontSet(STD6X9);

    // init gui
    gui_init();

    // init emu stuff
    emu_init();

    // init planes addresses, size & all
    display_init();

#ifdef SCREEN_USE_RESIZE
    // init hw resize
    resize_init();
#endif

    // init hw resize
    dsp_write32(&dspCom->outBufAddr,(uint32)lj_curRenderingScreenPtr);

    // apply settings
    gui_applySettings();

    // welcome screen
    gui_welcomeScreen();

    for(;;){
        // don't overclock during hdd access
        clk_overclock(false);

        gui_showGuiPlane();

        if(!gui_browse()) break;

        gui_showText("Loading ROM...");

        emu_loadRom();

        gui_applySettings();

        gui_showEmuPlane();

        emu_run();

        aic23_setOutputVolume(0,AIC23_CHANNEL_BOTH); // mute sound
        apu_reset(); // reset apu //HACK: it prevents the dsp from crashing on frequency change
    }

    gui_showText("Saving settings...");

    clk_overclock(false);
    snd_close();
    gui_close();
    gfx_closeGraphics();
    emu_close();

    return 0;
}

