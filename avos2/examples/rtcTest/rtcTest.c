#include <uart.h>
#include <string.h>    
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <rtc.h>

    static int pal32[2] = {
        0x8080c0e0, 0xffffffff
    };
    static int pal16[2] = {
        0x0000, 0xffff
    };

    struct graphicsBuffer screenBitmap;
    char data[512];
    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    struct graphicsBuffer spriteShadow = {0, 2, 12, 18, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    int addr= 0xd0;             // 0x3c, 0x90, 0xd0
    
    struct tm* ourTime;
    char timeSt[] = "xx:xx:xx.xx";
    
int main() {
    int b;

    osdInitA();

    rtcInit();
    
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

    pal16[1] = 0x0202;
    graphicsStringA(&screenBitmap, 0, 0, &sprite4_6, std4x6_, 5, 0,
                "RTCTest By DoGgEr");
    while(1) {
        uartOutsA("rtcGetTime()\n");
        ourTime = rtcGetTime();
        uartOutsA("Ret\n");
        stringPutHexA(timeSt+9, ourTime->tm_ms, 2);
        stringPutHexA(timeSt+6, ourTime->tm_sec, 2);
        stringPutHexA(timeSt+3, ourTime->tm_min, 2);
        stringPutHexA(timeSt, ourTime->tm_hour, 2);
        pal16[1] = 0x0202;
        graphicsStringA(&screenBitmap, 160-(5*14)-7, 120, &spriteShadow, shadow_, 14, 0,
                    timeSt);
        uartOutsA("Loop\n");
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return 0;        
    }
}

