/*

 All files in this archive are subject to the GNU General Public License.
 See the file COPYING in the source tree root for full license agreement.
 This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 KIND, either express of implied.

 SMS/GG emulator (port of SMS Plus by Charles Mac Donald)

 Date:     11/10/2005
 Author:   GliGli

 Ported to mediOS 01/11/2006

*/

#include "shared.h"
#include "aosms_gui.h"

#define MAX_ROM_SIZE 1024*1024
#define SMS_BANK_SIZE 16384

#define TICKS_PER_FRAME 1667

#define SMS_WIDTH 256
#define SMS_HEIGHT 192

#define GG_WIDTH 160
#define GG_HEIGHT 144

__IRAM_DATA volatile int vblankNum;
int prevVblankNun=0;

int screen_initialX=0;
int screen_initialY=0;
int screen_initialWidth=0;
int screen_initialHeight=0;

bool autoFrameSkip=true;
int  frameSkip=0;
bool swapButtons=false;
int tvOut=0;
bool useResize=true;
bool overclocking=false;
int armFrequency=0;

int f3Use=0;
bool sticky1Pressed=false;
bool sticky2Pressed=false;
bool prevModPressed=false;

int fpsStart=0;
int fpsCount=0;

char romname[256];

long * framebuffer;
long * vidplane;

tDspCom * dspCom;

void clk_overclock(bool en){
#ifdef ENABLE_OVERCLOCKING
    if(en && overclocking){
        clkc_setClockFrequency(CLK_ARM,armFrequency*1000000);
    }else{
        // default params
        clkc_setClockParameters(CLK_ARM,15,2,2);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
    }
#endif
};

__IRAM_CODE void dsp_interrupt(int irq,struct pt_regs * regs){
    // debug message coming from the dsp
    if (dspCom->hasDbgMsg){
        char * str=malloc(256);
        int i;

        for(i=0;i<256;++i){
            str[i]=dspCom->dbgMsg[i];
        }
        printf("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
        free(str);
    }
};

void dsp_init(){
    extern char _binary_apps_aosms_dspcode_aosms_dsp_out_start;
    extern char _binary_apps_aosms_dspcode_aosms_dsp_out_end;
    unsigned char * dspcode=&_binary_apps_aosms_dspcode_aosms_dsp_out_start;
    int len=&_binary_apps_aosms_dspcode_aosms_dsp_out_end-&_binary_apps_aosms_dspcode_aosms_dsp_out_start;

    printf("initDSP() dspcode=%0.8X len=%d\n",dspcode,len);

    // dsp irq handler
    irq_changeHandler(IRQ_DSP,dsp_interrupt);
    irq_enable(IRQ_DSP);

    *DSP_COM=0;

    dsp_loadProgramFromMemory(dspcode,len);

    dsp_run();

    while(!(*DSP_COM)); // wait for the dsp to finish init
    dspCom = (tDspCom *) DSP_RAM(*DSP_COM);

    dspCom->chipNum=dsp_getChipNum();

    dspCom->psgTail=0;
    dspCom->psgHead=0;
    dspCom->psgStereo=0xff;

    dspCom->armInitFinished=1;
}

void codec_init(){
#ifdef HAVE_AIC23_SOUND
    aic23_setSampleRate(SAMPLE_RATE);
    aic23_enableOutput(true);
    aic23_setOutputVolume(115,AIC23_CHANNEL_BOTH);
#endif
}

void codec_close(){
#ifdef HAVE_AIC23_SOUND
    aic23_enableOutput(false);
#endif
}

void codec_setVolume(int vol){
#ifdef HAVE_AIC23_SOUND
    aic23_setOutputVolume(vol+27,AIC23_CHANNEL_BOTH);
#endif
}

__IRAM_CODE void osd_interrupt(int irq,struct pt_regs * regs){
    vblankNum++;
}

void display_init(){
    char * bmap;

    gfx_openGraphics();

    vblankNum=0;
    irq_changeHandler(IRQ_OSD,osd_interrupt);
    irq_enable(IRQ_OSD);

    bmap=malloc(320*256);
    bmap=(char*)(((unsigned int)bmap+32)&0xffffffe0);
    gfx_planeSetBufferOffset(BMAP1,bmap);

    framebuffer=malloc(SMS_WIDTH*SMS_HEIGHT*4);
    framebuffer=(long*)(((unsigned int)framebuffer+32)&0xffffffe0);

    vidplane=malloc(320*256*4);
    vidplane=(long*)(((unsigned int)vidplane+32)&0xffffffe0);

    gfx_planeGetPos(VID1,&screen_initialX,&screen_initialY);
    gfx_planeGetSize(VID1,&screen_initialWidth,&screen_initialHeight,NULL);

    gfx_setPlane(BMAP1);
    gfx_clearScreen(COLOR_WHITE);
    gfx_planeShow(BMAP1);

    gfx_planeHide(BMAP2);
    gfx_planeHide(VID1);
    gfx_planeHide(VID2);
}

void screen_init(){
    int x,y,w,h;
    int gx,gy,gw,gh;
    int mode;

    gfx_planeSetBufferOffset(VID1,vidplane);

    if(!tvOut){
        if(useResize){
            if(cart.type==TYPE_GG){
                x=LCD_GG_X;
                y=LCD_GG_Y;
                w=screen_initialWidth+LCD_GG_WIDTH_DELTA;
                h=screen_initialHeight+LCD_GG_HEIGHT_DELTA;

                resize_setup((long)framebuffer+(SMS_HEIGHT-GG_HEIGHT+1)/2*SMS_WIDTH*4+(SMS_WIDTH-GG_WIDTH)/2*4,
                             SMS_WIDTH*2,(GG_WIDTH+GG_WIDTH_DELTA)*2,GG_HEIGHT+GG_HEIGHT_DELTA,
                             (long)vidplane,
                             (screen_initialWidth+LCD_GG_WIDTH_DELTA)*2,screen_initialHeight+LCD_GG_HEIGHT_DELTA);
            }else{
                x=LCD_SMS_X;
                y=LCD_SMS_Y;
                w=screen_initialWidth+LCD_SMS_WIDTH_DELTA;
                h=screen_initialHeight+LCD_SMS_HEIGHT_DELTA;

                resize_setup((long)framebuffer,
                             SMS_WIDTH*2,(SMS_WIDTH+SMS_WIDTH_DELTA)*2,SMS_HEIGHT+SMS_HEIGHT_DELTA,
                             (long)vidplane,
                             (screen_initialWidth+LCD_SMS_WIDTH_DELTA)*2,screen_initialHeight+LCD_SMS_HEIGHT_DELTA);
            }
        }else{
            if(cart.type==TYPE_GG){
                x=MAX(0,screen_initialWidth-GG_WIDTH);
                y=MAX(0,(screen_initialHeight-GG_HEIGHT)/2);
                w=GG_WIDTH;
                h=GG_HEIGHT;

                gfx_planeSetBufferOffset(VID1,framebuffer+
                                              SMS_WIDTH*(SMS_HEIGHT-GG_HEIGHT)/2+ //shift Y
                                              (SMS_WIDTH-GG_WIDTH)/2); //shift X

            }else{
                x=MAX(0,screen_initialWidth-SMS_WIDTH),
                y=MAX(0,(screen_initialHeight-SMS_HEIGHT)/2);
                w=SMS_WIDTH;
                h=SMS_HEIGHT;

                gfx_planeSetBufferOffset(VID1,framebuffer+
                                              MAX(0,SMS_WIDTH*(SMS_HEIGHT-screen_initialHeight)/2)+ //shift Y
                                              MAX(0,(SMS_WIDTH-screen_initialWidth)/2)); //shift X
            }
        }

        mode=VIDENC_MODE_LCD;
        gx=gy=0;
        getResolution(&gw,&gh);

    }else{
        switch(tvOut){
            case 1: //PAL
                mode=VIDENC_MODE_PAL;
                x=40;
                y=46;
                w=320;
                h=192;
                gx=60;
                gy=22;
                gw=300;
                gh=240;
                break;
            case 2: //Stretched PAL
                mode=VIDENC_MODE_PAL;
                x=40;
                y=16;
                w=320;
                h=256;
                gx=60;
                gy=22;
                gw=300;
                gh=240;
                break;
            default:
            case 3: //NTSC
                mode=VIDENC_MODE_NTSC;
                x=40;
                y=24;
                w=320;
                h=192;
                gx=60;
                gy=20;
                gw=300;
                gh=200;
                break;
        }

        if(cart.type==TYPE_GG){
            resize_setup((long)framebuffer+(SMS_HEIGHT-GG_HEIGHT+1)/2*SMS_WIDTH*4+(SMS_WIDTH-GG_WIDTH)/2*4,
                         SMS_WIDTH*2,GG_WIDTH*2,GG_HEIGHT,
                         (long)vidplane,
                         w*2,h);
        }else{
            resize_setup((long)framebuffer,
                         SMS_WIDTH*2,SMS_WIDTH*2,SMS_HEIGHT,
                         (long)vidplane,
                         w*2,h);
        }
    }

    gfx_planeSetPos(VID1,screen_initialX+x,screen_initialY+y);
    gfx_planeSetSize(VID1,w,h,32);

    // when no resize, gg screen is 160px wide but buffer width is 256px
    if(!tvOut && !useResize){
        osd_setComponentSourceWidth(OSD_VIDEO1,SMS_WIDTH/8);
    }

    gfx_planeSetPos(BMAP1,screen_initialX+gx,screen_initialY+gy);
    gfx_planeSetSize(BMAP1,gw,gh,8);

    videnc_setup(mode,false);

    // resize browser
    if (gw!=browser->width || gh!=browser->height){
        browser->width=gw;
        browser->height=gh;
        browser->nb_disp_entry=-1; // recompute values
        browser->max_entry_length=-1;
        gui_browserNeedInit=true;
    }
}

void emu_init(){

    memset(romname,0,255);

    sms.use_fm = false;
    sms.country = TYPE_OVERSEAS;
    sms.save = false;
    sms.cyclesperline=256;

    cart.rom=malloc(MAX_ROM_SIZE);

    bitmap.width  = SMS_WIDTH;
    bitmap.height = SMS_HEIGHT;
    bitmap.depth  = 32;
    bitmap.pitch  = SMS_WIDTH*4;
    bitmap.data   = (unsigned char*)framebuffer;

    system_init(0);
}

bool rom_load(char *romname){
    int f,cnt;

    f=open(romname,O_RDONLY);
    if (f<0) return false;

    lseek(f,filesize(f)%SMS_BANK_SIZE,SEEK_SET); // skip header if there is one

    cnt=read(f,cart.rom,MAX_ROM_SIZE);

    cart.pages=cnt/SMS_BANK_SIZE;
    cart.type=TYPE_SMS;

    if (strstr(romname,".GG") || strstr(romname,".gg")) cart.type=TYPE_GG;

    close(f);
    
    printf("%s loaded, %d pages\n",romname,cart.pages);
    return cnt>0;
}

bool emu_processKeys(){
    int bt;

    bt=btn_readState();

    input.pad[0]=bt&0xf; // directions
    
    if(swapButtons){
        if(bt&SMS_BTN_1) input.pad[0]|=INPUT_BUTTON2;
        if(bt&SMS_BTN_2) input.pad[0]|=INPUT_BUTTON1;
    }else{
        if(bt&SMS_BTN_1) input.pad[0]|=INPUT_BUTTON1;
        if(bt&SMS_BTN_2) input.pad[0]|=INPUT_BUTTON2;
    }
    
    input.system=0;
    
    if (bt&SMS_BTN_START){
        if (cart.type==TYPE_SMS){
            input.system|=INPUT_PAUSE;
        }else{
            input.system|=INPUT_START;
        }
    }
    
    if (bt&SMS_BTN_INGAME_MENU){
        dspCom->psgEnabled=0;
    
        gui_execute();
        gui_applySettings();
    
        dspCom->psgEnabled=1;
    }
    
    if (bt&SMS_BTN_QUIT){
        dspCom->psgEnabled=0;
    
        if(gui_confirmQuit()) return true;

        dspCom->psgEnabled=1;
    }
    
    if (bt&SMS_BTN_RESET){
        dspCom->psgEnabled=0;
        system_reset();
    
    }
    
    if(bt & SMS_BTN_MOD_12){
        switch(f3Use){
            case 0: //1+2
                input.pad[0]|=INPUT_BUTTON1|INPUT_BUTTON2;
                break;
            case 1: //Sticky 1
                if (!prevModPressed){ // only on F3 button state change
                    sticky1Pressed=!sticky1Pressed;
                }
                break;
            case 2: //Sticky 2
                if (!prevModPressed){ // only on F3 button state change
                    sticky2Pressed=!sticky2Pressed;
                }
                break;
        }
        prevModPressed=true;
    }else{
        prevModPressed=false;
    }
    
    if (sticky1Pressed){
    	if (input.pad[0]&INPUT_BUTTON1){
    	    input.pad[0]&=~INPUT_BUTTON1;
    	}
    	else{
    	    input.pad[0]|=INPUT_BUTTON1;
    	}
    }
    
    if (sticky2Pressed){
    	if (input.pad[0]&INPUT_BUTTON2){
    	    input.pad[0]&=~INPUT_BUTTON2;
    	}
    	else{
    	    input.pad[0]|=INPUT_BUTTON2;
    	}
    }
    
    return false;
}

void emu_loop(){
    int i,prevTick;
    int frameTickDelta,frameTick,frameLen;
    int page;
    bool quit;

    page=0;
    fpsStart=prevTick=tmr_getMicroTick();
    fpsCount=0;
    quit=false;

    sticky1Pressed=false;
    sticky2Pressed=false;
    prevModPressed=false;

    do{
        // get prev values
        prevVblankNun=vblankNum;
        prevTick=tmr_getMicroTick();

        // frame skip
        for(i=0;i<frameSkip;++i){
            quit|=emu_processKeys();
            sms_frame(1);
        }

        // rendering
        quit|=emu_processKeys();
        sms_frame(0);

        // frame sync
        frameTick=tmr_getMicroTick()-prevTick;
        frameLen=(frameSkip+1)*TICKS_PER_FRAME;
        frameTickDelta=frameLen-frameTick;

        if (tvOut==0 || tvOut==3){
            //we use the vblank interrupt to get sync
            if (frameTick<frameLen){
                while((vblankNum-prevVblankNun)<(frameSkip+1)) /* nothing */;
            }
        }else{

            while(frameTick<frameLen){
                frameTick=tmr_getMicroTick()-prevTick;
            }
        }

        if (autoFrameSkip){
          if (frameTickDelta>2*TICKS_PER_FRAME/3) --frameSkip; // let's assume rendering is done in 2/3 the time of a frame
          if (frameTickDelta<-TICKS_PER_FRAME/20) ++frameSkip; // we tolerate 5% error
          frameSkip=MAX(frameSkip,0);
        };

        // resize
        if(useResize || tvOut){
            resize_execute();
        };

        // fps count
        fpsCount++;
        if (prevTick-fpsStart>=100000){
            printf("%d fps\n",fpsCount);
            fpsCount=0;
            fpsStart=prevTick;
        }
    }while(!quit);
}


void sram_loadSave(bool save){
    char * fn=malloc(256);
    int f;

    if(save){
        gui_showText("Saving SRAM...");
    }else{
        gui_showText("Loading SRAM...");
    }

    //build file name
    sprintf(fn,SAVES_PATH"%s",strrchr(romname,'/')+1);
    *strrchr(fn,'.')='\0';
    strcat(fn,".sav");

    if(save){
        f=open(fn,O_CREAT|O_RDWR);
        if (f>=0){
            write(f,sms_sram,sizeof(sms_sram));
            close(f);
        }
    }else{
        f=open(fn,O_RDONLY);
        if (f>=0){
            read(f,sms_sram,sizeof(sms_sram));
            close(f);
        }
    }

    free(fn);
}

int app_main(){
    display_init();
    codec_init();
    dsp_init();
    emu_init();
    gui_init();

    // disable LCD & halt timer
    set_timer_status(LCD_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(LCD_TIMER,TIMER_MODE_DC,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_DC,MODE_DISABLE);

    // create dirs if they don't exist
    mkdir(AOSMS_PATH,-1);
    mkdir(SAVES_PATH,-1);

    gui_welcomeScreen();

    for(;;){
        gui_showGuiPlane();

        if (!gui_browse()) break;

        gui_showText("Loading ROM...");

        if (!rom_load(romname)) continue;

        system_reset();
        sram_loadSave(false);

        gui_applySettings();
        gui_showEmuPlane();

        emu_loop();

        dspCom->psgEnabled=0;

        clk_overclock(false);

        if(sms.save) sram_loadSave(true);
    }

    gui_showText("Saving settings...");

    clk_overclock(false);
    codec_close();
    gui_close();
    videnc_setup(VIDENC_MODE_LCD,false);
    gfx_closeGraphics();

    return 1;
}
