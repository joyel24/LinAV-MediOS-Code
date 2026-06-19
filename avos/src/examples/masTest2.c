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

    ataPowerDownHDDA();

    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

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

    graphicsBoxfA(&screenBitmap, 0, 0, 320, 120, 0x0000);
    graphicsBoxfA(&screenTop, 0, 0, 320, 120, 0x0000);

    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 120);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12 + 120);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x15);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    osdSetComponentSizeA(OSD_BITMAP2, 320*2, 120);
    osdSetComponentPositionA(OSD_BITMAP2, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP2, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_BITMAP2, 0x14);
    osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    pal16[1] = 0xffff;
    graphicsStringA(&screenTop, 4, 9, &sprite4_6, std4x6_, 5, 0, "masTest2 (c)DoGgEr");

    masResetA();
    c = masGetVersionA();
    
    c = masWriteCodecRegA(0x08, 0x8000);     // MONO input
    c = masWriteCodecRegA(0x0e, 0x0000);
    c = masWriteCodecRegA(0x11, 0x0000);     // Balance = center

    c = masWriteCodecRegA(0x10, 0x6000);     // Volume

    c = masWriteCodecRegA(0x06, 0x4000);     // Input gain

    c = masWriteCodecRegA(0, 0xffff);        // Enable stuff

    while(1) {
        graphicsBoxfA(&screenBitmap2, 320, 0, 1, 120, 0x0000);
        graphicsSpriteA(&screenBitmap, 0, 0, &screenBitmap2);
        
        b = (masReadCodecRegA(0x000c) & 0x7fff);
        v = (b * 59) >> 15;
        graphicsSetPixelA(&screenBitmap, 319, 60 - v, 0x0404);
        b = (masReadCodecRegA(0x000d) & 0x7fff);
        b = (b * 59) >> 15;
        
        if ((b+v)>0)
            graphicsBoxfA(&screenBitmap, 319, 60 - v, 1, b + v, 0x0404);

        b = (masReadCodecRegA(0x000a) & 0x7fff);
        v = (b * 59) >> 15;
        graphicsSetPixelA(&screenBitmap, 319, 60 - v, 0x0202);
        b = (masReadCodecRegA(0x000b) & 0x7fff);
        v = (b * 59) >> 15;
        graphicsSetPixelA(&screenBitmap, 319, 60 + v, 0x0202);
        
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;
    }

}
