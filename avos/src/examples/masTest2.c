#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <ata.h>
#include <fonts.h>
#include <mas.h>

    struct graphicsBuffer screenBitmap;
    struct graphicsBuffer screenTop;
    struct graphicsBuffer screenBitmap2;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

int main() {
    unsigned int c, v, b, x, y = 120;

    ataPowerDownHDD();

    osdInit();

    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 336*2;
    screenBitmap.width = 320;
    screenBitmap.height = 120;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    screenTop.offset = 0x03c00000;
    screenTop.bytesPerLine = 320*2;
    screenTop.width = 320;
    screenTop.height = 120;
    screenTop.bitsPerPixelShift = 4;
    screenTop.bitsPerPixel = 16;

    screenBitmap2.offset = 0x03800000 + 2;       // So we can shift left a pix
    screenBitmap2.bytesPerLine = 336*2;
    screenBitmap2.width = 321;
    screenBitmap2.height = 120;
    screenBitmap2.transparent = -1;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap, 0, 0, 320, 120, 0x0000);
    graphicsBoxf(&screenTop, 0, 0, 320, 120, 0x0000);

    osdSetComponentSize(OSD_BITMAP1, 320*2, 120);
    osdSetComponentPosition(OSD_BITMAP1, 0x14, 0x12 + 120);
    osdSetComponentOffset(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x15);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    osdSetComponentSize(OSD_BITMAP2, 320*2, 120);
    osdSetComponentPosition(OSD_BITMAP2, 0x14, 0x12);
    osdSetComponentOffset(OSD_BITMAP2, 0x03c00000);
    osdSetComponentSourceWidth(OSD_BITMAP2, 0x14);
    osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    pal16[1] = 0xffff;
    graphicsString(&screenTop, 4, 9, &sprite4_6, std4x6_, 5, 0, "masTest2 (c)DoGgEr");

    masReset();
    c = masGetVersion();
    
    c = masWriteCodecReg(0x08, 0x8000);     // MONO input
    c = masWriteCodecReg(0x0e, 0x0000);
    c = masWriteCodecReg(0x11, 0x0000);     // Balance = center

    c = masWriteCodecReg(0x10, 0x6000);     // Volume

    c = masWriteCodecReg(0x06, 0x4000);     // Input gain

    c = masWriteCodecReg(0, 0xffff);        // Enable stuff

    while(1) {
        graphicsBoxf(&screenBitmap2, 320, 0, 1, 120, 0x0000);
        graphicsSprite(&screenBitmap, 0, 0, &screenBitmap2);
        
        b = (masReadCodecReg(0x000c) & 0x7fff);
        v = (b * 59) >> 15;
        graphicsSetPixel(&screenBitmap, 319, 60 - v, 0x0404);
        b = (masReadCodecReg(0x000d) & 0x7fff);
        b = (b * 59) >> 15;
        
        if ((b+v)>0)
            graphicsBoxf(&screenBitmap, 319, 60 - v, 1, b + v, 0x0404);

        b = (masReadCodecReg(0x000a) & 0x7fff);
        v = (b * 59) >> 15;
        graphicsSetPixel(&screenBitmap, 319, 60 - v, 0x0202);
        b = (masReadCodecReg(0x000b) & 0x7fff);
        v = (b * 59) >> 15;
        graphicsSetPixel(&screenBitmap, 319, 60 + v, 0x0202);
        
        b = buttonsGetStatus();
        if (b & BUTTONS_AV300_OFF) return;
    }

}
