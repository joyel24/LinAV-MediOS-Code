#include <graphics.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <buttons.h>

    struct graphicsBuffer screenBitmap;

    static int pal16[2] = {0x0000, 0xffff};

    char hex2[] = "xx";
    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, 0};
    
int main() {    
    int x, y, c = 0;
    osdInitA();

    osdSetVideoOutOnA();
    
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
    
                                     
    for (y=0;y<240;y+=15) {
        for (x=0;x<320;x+=20) {
    graphicsBoxfA(&screenBitmap, x, y, 20, 15, c);
    stringPutHexA(hex2, c++, 2);
    pal16[1] = 0x0000;
    graphicsStringA(&screenBitmap, x+1, y, &sprite8_13, std8x13_, 8, 0, hex2);
    pal16[1] = 0xffff;
    graphicsStringA(&screenBitmap, x+2, y+1, &sprite8_13, std8x13_, 8, 0, hex2);
        }
    }
    
    while (!(buttonsGetStatusA() & BUTTONS_AV300_ANY)) {}    

}
