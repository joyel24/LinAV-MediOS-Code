#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <power.h>
#include <fonts.h>
#include <ata.h>

    struct graphicsBuffer screenBitmap;
    struct graphicsBuffer screenTop;
    struct graphicsBuffer screenBitmap2;
    static int pal16[2] = {0x0000, 0xffff};
    static int pal32[2] = {0x004a4956, 0x00000000};

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};
    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, 0, (int**) &pal32};

    void plotVals(int x);
    
int main() {
    int c, v, b, x, y = 120;
    unsigned int pixoff = 0;
    unsigned int scroff = 0;
    unsigned int drawx = 320;
    int slowdown=1;

//    ataPowerDownHDDA();
    
    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 656*2;          // Make it twice the screen + a bit
    screenBitmap.width = 656;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;
    
    screenBitmap2.offset = 0x03c00000;
    screenBitmap2.bytesPerLine = 320*4;
    screenBitmap2.width = 320;
    screenBitmap2.height = 240;
    screenBitmap2.bitsPerPixelShift = 5;
    screenBitmap2.bitsPerPixel = 32;

    graphicsBoxfA(&screenBitmap, 0, 0, 656, 240, 0x0000);
    graphicsBoxfA(&screenBitmap2, 0, 0, 320, 240, 0x004a4956);

    for (y=0;y<=120;y+=30) {
        graphicsBoxfA(&screenBitmap2, 0, y, 320, 1, 0x00000000);
        graphicsBoxfA(&screenBitmap2, 0, 239 - y, 320, 1, 0x00000000);        
    }

    for (x=0;x<=160;x+=32) {
        graphicsBoxfA(&screenBitmap2, x, 0, 1, 240, 0x00000000);
        graphicsBoxfA(&screenBitmap2, 319-x, 0, 1, 240, 0x00000000);        
    }
    
    graphicsStringA(&screenBitmap2, 2, 1, &sprite8_13, std8x13_, 8, 0,
                "PowerTest By DoGgEr");

    osdSetMainShiftA(0x14 + 0x40, 0x13);

    osdSetComponentSizeA(OSD_BITMAP2, 336*2, 240);
    osdSetComponentPositionA(OSD_BITMAP2, 0x40, 0);
    osdSetComponentOffsetA(OSD_BITMAP2, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP2, 0x29);
    osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_0TRANS
                                     | OSD_BITMAP_A1
                                     | OSD_BITMAP_8BIT);

    osdSetComponentSizeA(OSD_VIDEO1, 320*2, 240);
    osdSetComponentPositionA(OSD_VIDEO1, 0x40 - 8, 0);
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);

    
    while(1) {

        // Work out new display position after opdate...        
        pixoff++;        
        if (pixoff==16) {
            pixoff=0;
            scroff++;
            if (scroff==0x15) {
                scroff=0;
                pixoff=0;
                drawx=320;                
            }
        }

        // Draw pixels to the right of display window
        //
        plotVals(drawx);        
        // Done draw...

        drawx++;

        osdSetComponentOffsetA(OSD_BITMAP2, 0x03800000 + (scroff<<5));   // *16*2            
        osdSetComponentPositionA(OSD_BITMAP2, 0x14 + 0x20 - (pixoff<<1), 0);

        if (drawx>=336) {
            plotVals(drawx-336);
        }

        int c=slowdown;
        for (v=0;v<c;v++) {
            b = buttonsGetStatusA();
            if (b & BUTTONS_AV300_OFF) return;
            if (b & BUTTONS_AV300_MENU1) {slowdown=1;break;}
            if (b & BUTTONS_AV300_MENU2) {slowdown=0x8000;break;}
            if (b & BUTTONS_AV300_MENU3) {slowdown=0x100000;break;}
        }
        
    }
}

void plotVals(int x) {
    int y, v;
    graphicsBoxfA(&screenBitmap, x, 0, 1, 240, 0x0000);        

    v = (powerGetStatusA(INTERNAL_ON | ADC_ON | CMD_BAT0) & 0xfff);
    y = v * 240 >> 12;
    if (y>239) y=239;
    if (y<0) y=0;
    graphicsSetPixelA(&screenBitmap, x, 239-y, 0x0101);
    
    v = (powerGetStatusA(INTERNAL_ON | ADC_ON | CMD_BAT1) & 0xfff);
    y = v * 240 >> 12;
    if (y>239) y=239;
    if (y<0) y=0;
    graphicsSetPixelA(&screenBitmap, x, 239-y, 0x0202);

    v = (powerGetStatusA(INTERNAL_ON | ADC_ON | CMD_TEMP0) & 0xfff);
    y = v * 240 >> 12;
    if (y>239) y=239;
    if (y<0) y=0;
    graphicsSetPixelA(&screenBitmap, x, 239-y, 0x0303);

    v = (powerGetStatusA(INTERNAL_ON | ADC_ON | CMD_TEMP1) & 0xfff);
    y = v * 240 >> 12;
    if (y>239) y=239;
    if (y<0) y=0;
    graphicsSetPixelA(&screenBitmap, x, 239-y, 0x0404);
}

