#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <power.h>
#include <fonts.h>
#include <ata.h>
#include <mas.h>
#include <masc.h>
#include <uart.h>
#include <gio.h>
#include <dma.h>

#include <string.h>
#include <stdio.h>
#include <math.h>

char buff[] = "xxxxxxxx\n";

unsigned int buffmem[1];

unsigned int dmamem[8];

int main() {
    unsigned int c, v, b, a;

    c = gioGetAllDirectionsA();
    gioSetAllDirectionsA(0x5f7e00f5);
    gioSetAllInvertsA(0x00000010);      // Just EOD inverted...

    debug("GIO=%08x\n", gioGetAllBitsetsA());

    masResetA();
    
    c = masGetVersionA();

    uartOutsA("GetVersion: ");
    stringPutHexA(buff, c, 8);    
    uartOutsA(buff);

    buffmem[0] = 0x0;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - none
    while(1) {
        uartOutsA("Waiting for app end...\n");
        masReadD0A(0x7f7, buffmem, 1);      // App running
        if (buffmem[0]==0) break;
    }

    buffmem[0] = 0x04;
    masWriteD0A(0x7f2, buffmem, 1);
    
    buffmem[0] = 0x125;
    masWriteD0A(0x7f1, buffmem, 1);     // Demand mode
        
//    buffmem[0] = 0x0c;
//    masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!

//    while(1) {
//        uartOutsA("Waiting for app start...\n");
//        masReadD0A(0x7f7, buffmem, 1);      // App running
//        if (buffmem[0]==0x0c) break;
//    }

    masWriteCodecRegA(0, 1);
    masWriteCodecRegA(6, 0);
    masWriteCodecRegA(7, 0x4000);
    
    
//    masConfigAudioCodec(0x0c, 0x0c, 0x0c, MAS_CONFIG_INPUT_MIC
//                                        | MAS_CONFIG_DAC_ENABLE);
    
//    masConfigInput(MAS_CONFIGINPUT_MONO);    
//    masConfigOutput(0x00, 0x40, 0);
//    masSetBalance(0x00);
//    masSetVolume(0x60);
    
    for (b=0;b<40;b++) {
        debug("GIO=%08x\n", gioGetAllBitsetsA());
        a = masWriteDataA(5);
        debug("DATA: %08x\n", a);
    }


    uartOutsA("\nMasMem 7f0:\n");
    
    for (c=0x7f0;c<0x800;c++) {
        masReadD0A(c, buffmem, 1);
        stringPutHexA(buff, buffmem[0], 8);
        uartOutsA(buff);
    }

    uartOutsA("\nMasMem fd0:\n");
    
    for (c=0xfd0;c<0xfd8;c++) {
        masReadD0A(c, buffmem, 1);
        stringPutHexA(buff, buffmem[0], 8);
        uartOutsA(buff);
    }
    
    maingio();
       
}

char hex2[] = "xx";

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

int maingio() {
    unsigned int a,b,c,d;
    int cursor=0;
    
    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);

    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    graphicsStringA(&screenBitmap, 0, 0, &sprite5_8, std5x8_, 6, 0,
                "GIO test v0.01 (c)DoGgEr");

    graphicsStringA(&screenBitmap, 280, 150, &sprite5_8, std5x8_, 6, 0,
                "IN/OUT");
    graphicsStringA(&screenBitmap, 280, 180, &sprite5_8, std5x8_, 6, 0,
                "INVERT");
    graphicsStringA(&screenBitmap, 280, 210, &sprite5_8, std5x8_, 6, 0,
                "Exit");

    while(1) {
        c = gioGetAllDirectionsA();
        d = gioGetAllInvertsA();
        
        for (a=0;a<16;a++) {
            if (cursor==a) {
                pal16[0] = 0x0404;
            } else {
                pal16[0] = 0x0000;
            }
            stringPutHexA(hex2, a, 2);
            graphicsStringA(&screenBitmap, 0, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
            b = gioGetBitA(a);
            stringPutHexA(hex2, b, 2);
            graphicsStringA(&screenBitmap, 15, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, ": ");
            if ((c>>a) & 1) {
                graphicsStringA(&screenBitmap, 25, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "IN ");
            } else {
                graphicsStringA(&screenBitmap, 25, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "OUT");
            }
            if ((d>>a) & 1) {
                graphicsStringA(&screenBitmap, 40, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "~");
            } else {
                graphicsStringA(&screenBitmap, 40, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, " ");
            }
            graphicsStringA(&screenBitmap, 50, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
            //
            //
            if (cursor==(a+16)) {
                pal16[0] = 0x0404;
            } else {
                pal16[0] = 0x0000;
            }
            stringPutHexA(hex2, a+16, 2);
            graphicsStringA(&screenBitmap, 160, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
            b = gioGetBitA(a+16);
            stringPutHexA(hex2, b, 2);
            graphicsStringA(&screenBitmap, 175, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, ": ");
            if ((c>>(a+16)) & 1) {
                graphicsStringA(&screenBitmap, 185, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "IN ");
            } else {
                graphicsStringA(&screenBitmap, 185, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "OUT");
            }
            if ((d>>(a+16)) & 1) {
                graphicsStringA(&screenBitmap, 200, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "~");
            } else {
                graphicsStringA(&screenBitmap, 200, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, " ");
            }
            graphicsStringA(&screenBitmap, 210, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
        
        }

        a = buttonsGetStatusA();
        if (a & BUTTONS_AV300_UP) {
            if (cursor>0) cursor--;
        } else if (a & BUTTONS_AV300_DOWN) {
            if (cursor<31) cursor++;    
        } else if (a & BUTTONS_AV300_LEFT) {
            gioClearBitA(cursor);    
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        } else if (a & BUTTONS_AV300_RIGHT) {
            gioSetBitA(cursor);                
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        } else if (a & BUTTONS_AV300_MENU1) {
            b = gioGetAllDirectionsA();
            b ^= (1<<cursor);
            gioSetAllDirectionsA(b);
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        } else if (a & BUTTONS_AV300_MENU2) {
            b = gioGetAllInvertsA();
            b ^= (1<<cursor);
            gioSetAllInvertsA(b);
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        }
        if (a & BUTTONS_AV300_MENU3) return;
    }
}
