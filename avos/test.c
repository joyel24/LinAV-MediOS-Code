#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>

int main() {
    struct graphicsBuffer screenVideo;
    struct graphicsBuffer sprite1;
    
    static char cursor[16] = {
        0x00, 0x00,
        0x60, 0x00,
        0x90, 0x00,
        0x10, 0x00,
        0x60, 0x00,
        0x80, 0x00,
        0xf0, 0x00,
        0x00, 0x00};

    static int pal[2] = {
        0x80c0e0, 0x809080
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

    sprite1.offset = (int) &cursor;
    sprite1.bytesPerLine = 2;
    sprite1.width = 16;
    sprite1.height = 8;
    sprite1.bitsPerPixelShift = 0;
    sprite1.bitsPerPixel = 1;
    sprite1.pallette = &pal;
    sprite1.transparent = 0;
    
    graphicsBoxf(&screenVideo, 0, 0, 160, 120, 0x0);

    graphicsSprite(&screenVideo, 2, 2, &sprite1);
    
    while(1) {}
}
