#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <power.h>
#include <fonts.h>
#include <ata.h>
#include <mas.h>
#include <uart.h>

#include <string.h>
#include <stdio.h>
#include <math.h>

    struct graphicsBuffer screenBitmap;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

char masVer[] = "Mas Version: xxxxxxxx";
char masReg[] = "xxxxxxxx\n";

int main() {
    unsigned int c, v, b;
    
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

    graphicsStringA(&screenBitmap, 0, 0, &sprite8_13, std8x13_, 9, 0,
                "Mas test v0.01 (c)DoGgEr");

    masResetA();
    
    c = masGetVersionA();
    stringPutHexA(masVer + 13, c, 8);
    graphicsStringA(&screenBitmap, 0, 14, &sprite8_13, std8x13_, 9, 0,
                masVer);

    c = masWriteCodecRegA(0x08, 0x8000);     // MONO input
    c = masWriteCodecRegA(0x0e, 0x0000);
    c = masWriteCodecRegA(0x11, 0x0000);     // Balance = center
    stringPutHexA(masReg, c, 8);
    uartOutsA(masReg);
    c = masWriteCodecRegA(0x10, 0x6000);     // Volume
    stringPutHexA(masReg, c, 8);
    uartOutsA(masReg);
    c = masWriteCodecRegA(0x06, 0x4000);     // Input gain
    stringPutHexA(masReg, c, 8);
    uartOutsA(masReg);

    c = masWriteCodecRegA(0, 0xffff);        // Enable stuff
    stringPutHexA(masReg, c, 8);
    uartOutsA(masReg);
    
    uartOutsA("Done write...\n");

    graphicsStringA(&screenBitmap, 0, 120, &sprite8_13, std8x13_, 9, 0,
                "IN  L");
    graphicsStringA(&screenBitmap, 0, 134, &sprite8_13, std8x13_, 9, 0,
                "IN  R");
    graphicsStringA(&screenBitmap, 0, 148, &sprite8_13, std8x13_, 9, 0,
                "OUT L");
    graphicsStringA(&screenBitmap, 0, 162, &sprite8_13, std8x13_, 9, 0,
                "OUT R");
    
    while(1) {
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;

        b = (masReadCodecRegA(0x000a) & 0x7fff);
        v = (b * (320-54)) >> 15;
        b = 320-54-v;
        if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 124, b, 6, 0x0000);
        if (v>0) graphicsBoxfA(&screenBitmap, 54, 124, v, 6, 0x0202);
        
        b = (masReadCodecRegA(0x000b) & 0x7fff);
        v = (b * (320-54)) >> 15;
        b = 320-54-v;
        if (b>0) graphicsBoxfA(&screenBitmap, 54 + v, 138, b, 6, 0x0000);
        if (v>0) graphicsBoxfA(&screenBitmap, 54, 138, v, 6, 0x0202);

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
        
    }

}
