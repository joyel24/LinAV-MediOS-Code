#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>

extern char (*std8x13_[]) [];

int main() {
    struct graphicsBuffer screenVideo;
    struct graphicsBuffer sprite1;
    
    static int pal32[2] = {
        0x80c0e0, 0xffffff
    };
    static int pal16[2] = {
        0x0101, 0xffff
    };
        
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

    screenVideo.offset = 0x03c00000;
    screenVideo.bytesPerLine = 320*4;
    screenVideo.width = 320;
    screenVideo.height = 240;
    screenVideo.bitsPerPixelShift = 5;
    screenVideo.bitsPerPixel = 32;

    sprite1.bytesPerLine = 1;
    sprite1.width = 8;
    sprite1.height = 13;
    sprite1.bitsPerPixelShift = 0;
    sprite1.bitsPerPixel = 1;
    sprite1.transparent = 0;
    sprite1.pallette16 = &pal16;
    sprite1.pallette32 = &pal32;

    graphicsBoxf(&screenVideo, 0, 0, 320, 240, 0x000000);

    graphicsString(&screenVideo, 2, 20, &sprite1, &std8x13_, 9, 1, "abcdefghijklmnopqrstuvwyxz");
    graphicsString(&screenVideo, 2, 34, &sprite1, &std8x13_, 9, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    graphicsString(&screenVideo, 2, 48, &sprite1, &std8x13_, 9, 1, " .,<>/?;':[]{}|=-+_)(*&^%$#@!");
    pal[1] = 0x503020;
    graphicsString(&screenVideo, 100, 30, &sprite1, &std8x13_, 0, 14, "Font drawing!");
    pal[1] = 0x2090a0;
    graphicsString(&screenVideo, 4, 200, &sprite1, &std8x13_, 11, 0, "BY DoggerMoore");
    
    while(1) {}
}
