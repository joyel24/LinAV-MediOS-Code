#include <buttons.h>
#include <power.h>
#include <ata.h>
#include <mas.h>
#include <masc.h>
#include <uart.h>
#include <gio.h>
#include <interrupts.h>
#include <file.h>
#include <debug.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <timers.h>

void intsub();
void feedMas();

    struct graphicsBuffer screenBitmap;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

static unsigned int buffmem[1];
static char * mp3Buff = (void*)0x03600000;
static int mp3ptr=0;
static int fileSize;
static int masReady=0;

int main(int argc, char * * argv) {
    unsigned int c, v, b, a, i;
    unsigned int vol=0x60;
    unsigned int speed=0x4800;
    
    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03e00000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);

    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03e00000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    graphicsStringA(&screenBitmap, 0, 0, &sprite8_13, std8x13_, 9, 0,
                "Mp3 play v0.01 (c)DoGgEr");
    graphicsStringA(&screenBitmap, 0, 134, &sprite8_13, std8x13_, 9, 0,
                "VOL");
    graphicsStringA(&screenBitmap, 0, 148, &sprite8_13, std8x13_, 9, 0,
                "OUT L");
    graphicsStringA(&screenBitmap, 0, 162, &sprite8_13, std8x13_, 9, 0,
                "OUT R");

	inifatinfo();
	inidir();
	inifile();
	ataSelectHDDA();
	ataPowerUpHDDA();
	ataReadMBR();
	fatInit(getPartition(0));

	int fileHandle;
	fileHandle = fopen(argv[1], "r");

	if (fileHandle < 0) return;

	fileSize = fsize(fileHandle);
	if (fileSize > 0x00600000) return;

    fread(fileHandle, mp3Buff, fileSize);
	fclose(fileHandle);

    ataPowerDownHDDA();

    //debug("Formatting data... [%d]\n", fileSize);
    //ecrSwapBitsBufferA(mp3Buff, mp3Buff, fileSize);
    //ecrSwapBytesBufferA(mp3Buff, mp3Buff, fileSize);


    masResetA();
    
    for(c=0;c<100;c++) {}
        
    debug("GetVersion: %08x\n", masGetVersionA());

    for(c=0;c<100;c++) {}

    debug("masControl %04x\n", masControlReadA());
    
    
    for(c=0;c<100;c++) {}

    c = gioGetAllDirectionsA();
    
    debug("gioDirections %08x\n", c);
    
    gioSetAllDirectionsA(c & 0xffff00ff | 0xf5);

    c = gioGetAllDirectionsA();
    
    debug("gioDirections %08x\n", c);

    gioSetAllInvertsA(0x00000010);      // Just EOD inverted...
    //gioSetAllIRQsA(0xff);
    //interruptsInitA(intsub);
    //interruptsSetIRQEnabledA();    
    //interruptsSetMaskA(interruptsGetMaskA() | 0xffffff7f);

    masConfigAudioCodec(0x0c, 0x0c, 0x0c, MAS_CONFIG_INPUT_MIC
                                        | MAS_CONFIG_ADCLEFT_ENABLE
                                        | MAS_CONFIG_ADCRIGHT_ENABLE
                                        | MAS_CONFIG_DAC_ENABLE);
    
    masConfigInput(MAS_CONFIGINPUT_MONO);
    masConfigOutput(0x0, 0x40, 0);
    masSetBalance(0x00);
    masSetVolume(vol);

    for(c=0;c<20;c++) {}

    buffmem[0] = 0x0;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - none
    while(1) {
        uartOutsA("Waiting for app end...\n");
        masReadD0A(0x7f7, buffmem, 1);      // App running
        if (buffmem[0]==0) break;
    }

    buffmem[0] = 0x04;                  // Check. 0d/04?
    masWriteD0A(0x7f2, buffmem, 1);

    // 48000 = 0x4800
    // 44100 = 0x4e5e
    buffmem[0] = 0x4800;                // Controls speed of playback
    masWriteD0A(0x7f3, buffmem, 1);
    
    buffmem[0] = 0x125;
    masWriteD0A(0x7f1, buffmem, 1);     // Demand mode

    masReady=1;
    debug("Starting app...");

    for(c=0;c<20;c++) {}
    
    buffmem[0] = 0x0c;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!

    while(1) {
        masReadD0A(0x7f7, buffmem, 1);      // App running
        debug("Waiting for app start [%02x]...\n", buffmem[0]);
        if (buffmem[0]==0x0c) break;
    }
    
    debug("App started...\n");

//    timersConfigA(0, TIMERS_TMMD_FREERUN, 0, 1023, 0x400);    
    
    v = (vol * (320-54)) >> 7;
    b = 320-54-v;
    if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 138, b, 6, 0x0000);
    if (v>0) graphicsBoxfA(&screenBitmap, 54, 138, v, 6, 0x0202);
            
    while(1) {
        c = buttonsGetStatusA();
        if (c & BUTTONS_AV300_OFF) return;

        if (c & BUTTONS_AV300_UP) {
            if (vol<0x7f) {
                vol++;
                masSetVolume(vol);
                v = (vol * (320-54)) >> 7;
                b = 320-54-v;
                if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 138, b, 6, 0x0000);
                if (v>0) graphicsBoxfA(&screenBitmap, 54, 138, v, 6, 0x0202);
            }
        } else if (c & BUTTONS_AV300_DOWN) {
            if (vol>0x00) {
                vol--;
                masSetVolume(vol);
                v = (vol * (320-54)) >> 7;
                b = 320-54-v;
                if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 138, b, 6, 0x0000);
                if (v>0) graphicsBoxfA(&screenBitmap, 54, 138, v, 6, 0x0202);
            }
        } else if (c & BUTTONS_AV300_LEFT) {
            if (speed>0x80) {
                speed-=0x80;    
                buffmem[0] = speed;
                masWriteD0A(0x7f3, buffmem, 1);
                
                buffmem[0] = 0x125;
                masWriteD0A(0x7f1, buffmem, 1);     // Demand mode
            
   //             debug("Starting app...");

   //             for(c=0;c<20;c++) {}
                
   //             buffmem[0] = 0x0c;
   //             masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!
            
   //             while(1) {
   //                 masReadD0A(0x7f7, buffmem, 1);      // App running
   //                 debug("Waiting for app start [%02x]...\n", buffmem[0]);
   //                 if (buffmem[0]==0x0c) break;
   //             }
                
                debug("App started...\n");
                
            }
        } else if (c & BUTTONS_AV300_RIGHT) {
            if (speed<0xff80) {
                speed+=0x80;    
                buffmem[0] = speed;
                masWriteD0A(0x7f3, buffmem, 1);
                
                buffmem[0] = 0x125;
                masWriteD0A(0x7f1, buffmem, 1);     // Demand mode
            
   //             debug("Starting app...");
            
   //             for(c=0;c<20;c++) {}
                
   //             buffmem[0] = 0x0c;
   //             masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!
            
   //             while(1) {
   //                 masReadD0A(0x7f7, buffmem, 1);      // App running
   //                 debug("Waiting for app start [%02x]...\n", buffmem[0]);
   //                 if (buffmem[0]==0x0c) break;
   //             }
                
            }
        }

        
        for (c=0;c<20;c++) {
            feedMas();
        }

//            debug("gioBitset %08x\n", gioGetAllBitsetsA());
        
        b = (masReadCodecRegA(0x000c) & 0x7fff);
        v = (b * (320-54)) >> 15;
        b = 320-54-v;
        if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 152, b, 6, 0x0000);
        if (v>0) graphicsBoxfA(&screenBitmap, 54, 152, v, 6, 0x0202);

        b = (masReadCodecRegA(0x000d) & 0x7fff);
        v = (b * (320-54)) >> 15;
        b = 320-54-v;
        if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 166, b, 6, 0x0000);
        if (v>0) graphicsBoxfA(&screenBitmap, 54, 166, v, 6, 0x0202);

//        for (c=0xfd0;c<0xfd8;c++) {
//            c = 0xfd0;
//            masReadD0A(c, buffmem, 1);
//            debug("Mas %08x: %08x\n", c, buffmem[0]);
//        }
        
//        debug("out L, R: %08x, %08x\n", masGetOutPeakLeft(), masGetOutPeakRight());
    }
    
}

int cnt=0;

char irqb[] = "IRQ=xx\n";

void intsub() {
    int i=0,c=0;
    c = interruptsGetCausesA();
    c = c | (~interruptsGetMaskA());
//    debug("INT CAUSES %08x MASK %08x ", c, interruptsGetMaskA());
//    debug("CAUSES2 %08x MASK2 %08x\n", interruptsGetCauses2A(), interruptsGetMask2A());
    
    for (i=0;i<32;i++) {
        if (!(c&1)) break;
        c = c>>1;
    }

    stringPutHexA(irqb+4, i, 2);
    uartOutsA(irqb);

    //debug("IRQ=%d\n", i);
    
    //interruptsResetMaskA(i);      One shot
    interruptsResetIRQA(i);
//    if (i==0) feedMas();                // Timer0
    
//    debug("INT RET ");
//    debug("CAUSES %08x MASK %08x ", interruptsGetCausesA(), interruptsGetMaskA());
//    debug("CAUSES2 %08x MASK2 %08x\n", interruptsGetCauses2A(), interruptsGetMask2A());
}

void feedMas() {
    if (masReady==1) {
        interruptsSetIRQDisabledA();    
        cnt = masWriteDataA(mp3Buff + mp3ptr, 2048);
        interruptsSetIRQEnabledA();    
        mp3ptr+=cnt;
        if (mp3ptr>=fileSize) mp3ptr=0;
    }
    debug("masDataFed %d\n", cnt);
}
