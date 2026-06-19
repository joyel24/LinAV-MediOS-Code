#include <graphics.h>
#include <osdDSC25.h>
#include <video.h>
#include <gio.h>
#include <fonts.h>
#include <buttons.h>

    struct graphicsBuffer screenBitmap;

    static int pal16[2] = {0x0000, 0xffff};

    char hex2[] = "xx";
    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, 0};
    
int main() {    
    int x, y, c = 0, width, height;
    int vmode=0;            // 0 = LCD  320x240
                            // 1 = NTSC 304x192
                            // 2 = PAL  304x192
    osdInitA();

    while(1) {
    
    if (vmode==0) {
        gioSetBitA(GIO_LINE_EXT_VIDEO);
        gioClearBitA(GIO_LINE_BACKLIGHT_POWER);
        videoSetModeA(0x4071);
        videoLCDEnableA();
        osdSetMainShiftA(0x79, 0x00);
        width=320;
        height=240;
        osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x13);
    } else if (vmode==1) {
        gioClearBitA(GIO_LINE_EXT_VIDEO);
        gioClearBitA(GIO_LINE_BACKLIGHT_POWER);
        videoSetModeA(0x407d | VIDEO_MODE_NTSC);
        videoLCDDisableA();
        osdSetMainShiftA(0x78, 0x12);
        width=304;
        height=192;
        osdSetComponentPositionA(OSD_BITMAP1, 0x3a, 0x18);
    } else if (vmode==2) {
        gioClearBitA(GIO_LINE_EXT_VIDEO);
        gioSetBitA(GIO_LINE_BACKLIGHT_POWER);
        videoSetModeA(0x407d | VIDEO_MODE_PAL);
        videoLCDDisableA();
        osdSetMainShiftA(0xae, 0x2d);
        width=304;
        height=240;        
        osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x00);
    }
    
    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = width*2;
    screenBitmap.width = width;
    screenBitmap.height = height;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, width, height, 0x0000);    
    
    osdSetComponentSizeA(OSD_BITMAP1, width*2, height);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, width>>4);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);
    
                                     
    for (y=0;y<height;y+=(height>>4)) {
        for (x=0;x<width;x+=(width>>4)) {
    graphicsBoxfA(&screenBitmap, x, y, width>>4, height>>4, c | (c<<8));
    stringPutHexA(hex2, c++, 2);
    pal16[1] = 0x0000;
    graphicsStringA(&screenBitmap, x, y, &sprite8_13, std8x13_, 8, 0, hex2);
    pal16[1] = 0xffff;
    graphicsStringA(&screenBitmap, x+1, y, &sprite8_13, std8x13_, 8, 0, hex2);
        }
    }

    while (!(c=buttonsGetStatusA() & BUTTONS_AV300_ANY)) {}    

    if (c&BUTTONS_AV300_OFF) break;
    if (c&BUTTONS_AV300_MENU1) {
        vmode++;
        if (vmode==3) vmode=0;
    }
    }
}
