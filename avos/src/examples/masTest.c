#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <power.h>
#include <fonts.h>
#include <ata.h>
#include <mas.h>

    struct graphicsBuffer screenBitmap;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

char masVer[] = "Mas Version: xxxxxxxx";
char masReg[] = "xxxxxxxx\n";
    
int main() {
    int c, v, b;
    
    osdInit();

    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap, 0, 0, 320, 240, 0x0000);

    osdSetComponentSize(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPosition(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffset(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x14);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    graphicsString(&screenBitmap, 0, 0, &sprite8_13, std8x13_, 9, 0,
                "Mas test v0.01 (c)DoGgEr");

    masReset();
    
    c = masGetVersion();
    stringPutHex(masVer + 13, c, 8);
    graphicsString(&screenBitmap, 0, 14, &sprite8_13, std8x13_, 9, 0,
                masVer);

    c = masWriteCodecReg(0x08, 0x8000);     // MONO input
    c = masWriteCodecReg(0x0e, 0x0000);
    c = masWriteCodecReg(0x11, 0x0000);     // Balance = center
    stringPutHex(masReg, c, 8);
    uartOuts(masReg);
    c = masWriteCodecReg(0x10, 0x6000);     // Volume
    stringPutHex(masReg, c, 8);
    uartOuts(masReg);
    c = masWriteCodecReg(0x06, 0x4000);     // Input gain
    stringPutHex(masReg, c, 8);
    uartOuts(masReg);

    c = masWriteCodecReg(0, 0xffff);        // Enable stuff
    stringPutHex(masReg, c, 8);
    uartOuts(masReg);
    
    uartOuts("Done write...\n");
    
    for (v=0;v<0x40;v++) {
        //c = masReadReg(v);
        c = masReadCodecReg(v);
        stringPutHex(masReg, c, 8);
        uartOuts(masReg);
    //graphicsString(&screenBitmap, 0, 14*2, &sprite8_13, std8x13_, 9, 0,
    //            masReg);
    }        
    while(1) {
            b = buttonsGetStatus();
            if (b & BUTTONS_AV300_OFF) return;
    }

}
