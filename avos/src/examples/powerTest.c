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

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

char powerSt[] = "xxxx+";
    
int main() {
    int c, v, b, x, y = 120;
    int slowdown=1;

    ataPowerDownHDDA();
    
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

    screenTop.offset = 0x03c00000;
    screenTop.bytesPerLine = 320*2;
    screenTop.width = 320;
    screenTop.height = 240;
    screenTop.bitsPerPixelShift = 4;
    screenTop.bitsPerPixel = 16;

    screenBitmap2.offset = 0x03800000 + 2;       // So we can shift left a pix
    screenBitmap2.bytesPerLine = 320*2;
    screenBitmap2.width = 320;
    screenBitmap2.height = 240;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;
    
    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);
    graphicsBoxfA(&screenTop, 0, 0, 320, 240, 0x0000);

    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    osdSetComponentSizeA(OSD_BITMAP2, 320*2, 10);
    osdSetComponentPositionA(OSD_BITMAP2, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP2, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_BITMAP2, 0x14);
    osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    graphicsStringA(&screenTop, 0, 0, &sprite4_6, std4x6_, 5, 0,
                "Battery meter v0.01 (c)DoGgEr");

    while(1) {
        graphicsSpriteA(&screenBitmap, 0, 0, &screenBitmap2);
        graphicsBoxfA(&screenBitmap, 319, 0, 1, 240, 0x0000);
        
        v = powerGetStatusA();
        y = v * 240 >> 14;
        
        if (y>239) y=239;
        if (y<0) y=0;
        c = 0xffff;
        if (powerIsDCConnectedA()) c = 0x0404;
        
        graphicsSetPixelA(&screenBitmap, 319, y, c);

        stringPutHexA(powerSt, v, 4);

        if (powerIsDCConnectedA()) {
            powerSt[4] = '+';                
        } else {
            powerSt[4] = ' ';
        }
        pal16[1] = 0xffff;
        graphicsStringA(&screenTop, 4 + 35*5, 2, &sprite4_6, std4x6_, 5, 0,
                        powerSt);
        
        for (v=0;v<slowdown;v++) {
            b = buttonsGetStatusA();
            if (b & BUTTONS_AV300_OFF) return;
            if (b & BUTTONS_AV300_MENU1) {slowdown=1;break;}
            if (b & BUTTONS_AV300_MENU2) {slowdown=0x8000;break;}
            if (b & BUTTONS_AV300_MENU3) {slowdown=0x100000;break;}
        }
    }

}
