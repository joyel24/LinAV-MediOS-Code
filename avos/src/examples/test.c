#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>

    static int pal32[2] = {
        0x80c0e0, 0xffffff
    };
    static int pal16[2] = {
        0x0101, 0xffff
    };

    struct graphicsBuffer screenVideo;
    struct graphicsBuffer screenBitmap;

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, 0, 0, 0, 0, 0, &pal16, &pal32};
    struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, 0, 0, 0, 0, 0, &pal16, &pal32};
    struct graphicsBuffer sprite7_13 = {0, 1, 7, 13, 1, 0, 0, 0, 0, 0, 0, &pal16, &pal32};
    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, 0, 0, 0, 0, 0, &pal16, &pal32};
    
//
//
//
int main() {
        
    osdInit();
    
    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);
    
    osdSetComponentSize(OSD_VIDEO1, 640, 240);
    osdSetComponentPosition(OSD_VIDEO1, 0x14, 0x13);
    osdSetComponentOffset(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidth(OSD_VIDEO1, 0x28);
    osdSetComponentConfig(OSD_VIDEO1, OSD_COMPONENT_ENABLE);

    osdSetComponentSize(OSD_BITMAP1, 300, 100);
    osdSetComponentPosition(OSD_BITMAP1, 0x14, 0x13);
    osdSetComponentOffset(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x14);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A4);

    screenVideo.offset = 0x03c00000;
    screenVideo.bytesPerLine = 320*4;
    screenVideo.width = 320;
    screenVideo.height = 240;
    screenVideo.bitsPerPixelShift = 5;
    screenVideo.bitsPerPixel = 32;

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 120;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxf(&screenVideo, 0, 0, 320, 240, 0x000000);
    graphicsBoxf(&screenVideo, 0, 0, 160, 120, 0x800000);
    graphicsBoxf(&screenVideo, 20, 120, 200, 100, 0xc0c000);

    graphicsBoxf(&screenBitmap, 0, 0, 160, 120, 0x0000);
    graphicsBoxf(&screenBitmap, 20, 20, 120, 80, 0x0101);
    
    graphicsString(&screenBitmap, 2, 20, &sprite4_6, &std4x6_, 5, 1, "abcdefghijklmnop");
    
    graphicsBoxf(&screenVideo, 80, 60, 100, 100, 0x40e050);
    
    
    graphicsString(&screenVideo, 2, 34, &sprite8_13, &std8x13_, 9, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    graphicsString(&screenVideo, 2, 48, &sprite5_8, &std5x8_, 6, 1, " .,<>/?;':[]{}|=-+_)(*&^%$#@!");
    pal32[1] = 0x503020;
    graphicsString(&screenVideo, 100, 30, &sprite7_13, &std7x13_, 0, 14, "Font drawing!");
    pal32[1] = 0x2090a0;
    graphicsString(&screenVideo, 4, 200, &sprite4_6, &std4x6_, 7, 0, "BY DoggerMoore");
    
    while(1) {}
}
