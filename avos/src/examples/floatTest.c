#include <graphics.h>
#include <osdDSC25.h>
#include <math.h>

//void plot(float x, float y, float c);

    static int pal32[2] = {
        0x8080c0e0, 0xffffffff
    };
    static int pal16[2] = {
        0x0000, 0xffff
    };

    struct graphicsBuffer screenBitmap;


int main() {
    unsigned int dx, dy, dc;
    float x=0, y=0;

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
    
    for (y=0;y<1;y+=0.1) {
        for (x=0;x<1;x+=0.1) {
            //plot(x, y, x*y);        
    dx = x * 320;
    dy = y * 240;
    dc = x*y * 0xffffff;

    graphicsSetPixel(&screenBitmap, dx, dy, dc);
        }
    }

}

//
//
//
//void plot(float x, float y, float c) {
//    unsigned int dx, dy, dc;
//    dx = x * 320;
//    dy = y * 240;
//    dc = c * 0xffffff;

//    graphicsSetPixel(&screenBitmap, dx, dy, dc);
//}
