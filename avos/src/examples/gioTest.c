#include <uart.h>
#include <dma.h>
#include <buttons.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <gio.h>

char hex2[] = "xx";

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

int main() {
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
