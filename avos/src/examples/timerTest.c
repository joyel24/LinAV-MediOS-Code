#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <ata.h>
#include <fonts.h>
#include <timers.h>
#include <interrupts.h>

    struct graphicsBuffer screenBitmap;
    struct graphicsBuffer screenTop;
    struct graphicsBuffer screenBitmap2;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

char timev[] = "xxxx";
char ct[] = "F1 Timer: x";

unsigned int* intptr = 0x34;

void intsub();

int main() {
    int c, v, b, x, y = 120;
    int ctimer=0;
    int div=1023;       // Slowest rate...
    int tmode[4] = {TIMERS_TMMD_STOP, TIMERS_TMMD_STOP, TIMERS_TMMD_STOP, TIMERS_TMMD_STOP};
    int tmax[4] = {65535, 65535, 65535, 65535};

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
    graphicsStringA(&screenTop, 4, 9, &sprite4_6, std4x6_, 5, 0, "TimerTest - up/down changes value. F3=Trigger");
    stringPutHexA(ct + 10, ctimer, 1);
    graphicsStringA(&screenTop, 4, 2, &sprite4_6, std4x6_, 5, 0, ct);
    stringPutHexA(timev, tmax[ctimer], 4);
    graphicsStringA(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
    if (tmode[ctimer]==0) {
        graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 STOP");
    } else if (tmode[ctimer]==1) {
        graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 ONE ");
    } else if (tmode[ctimer]==2) {
        graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 FREE");
    }
    
    for (c=0;c<4;c++) {
        timersConfigA(c, tmode[c], 0, div, tmax[c]);        
    }

    *intptr = intsub;
    interruptsSetMaskA(0);

    while(1) {
        graphicsBoxfA(&screenBitmap2, 320, 0, 1, 120, 0x0000);
        graphicsSpriteA(&screenBitmap, 0, 0, &screenBitmap2);
        
        for (c=0;c<4;c++) {
            v = timersGetValueA(c);
            y = v * 120 >> 16;
            if (y>119) y=119;
            if (y<0) y=0;        
            graphicsSetPixelA(&screenBitmap, 319, y, c+1);
        }
        
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;
        if (b & BUTTONS_AV300_DOWN) {
            tmax[ctimer]-=0x100;
            timersConfigA(ctimer, tmode[ctimer], 0, div, tmax[ctimer]);
            stringPutHexA(timev, tmax[ctimer], 4);
            graphicsStringA(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
        } else if (b & BUTTONS_AV300_UP) {
            tmax[ctimer]+=0x100;
            timersConfigA(ctimer, tmode[ctimer], 0, div, tmax[ctimer]);
            stringPutHexA(timev, tmax[ctimer], 4);
            graphicsStringA(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
        }
        
        if (b & BUTTONS_AV300_MENU1) {
            while(buttonsGetStatusA() & BUTTONS_AV300_MENU1) {}
            ctimer++;
            if (ctimer==4) ctimer=0;
            stringPutHexA(ct + 10, ctimer, 1);
            graphicsStringA(&screenTop, 4, 2, &sprite4_6, std4x6_, 5, 0, ct);
            stringPutHexA(timev, tmax[ctimer], 4);
            graphicsStringA(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
            if (tmode[ctimer]==0) {
                graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 STOP");
            } else if (tmode[ctimer]==1) {
                graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 ONE ");
            } else if (tmode[ctimer]==2) {
                graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 FREE");
            }            
        }
        if (b & BUTTONS_AV300_MENU2) {
            while(buttonsGetStatusA() & BUTTONS_AV300_MENU2) {}
            tmode[ctimer]++;
            if (tmode[ctimer]==3) tmode[ctimer]=0;
            timersConfigA(ctimer, tmode[ctimer], 0, div, tmax[ctimer]);
            if (tmode[ctimer]==0) {
                graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 STOP");
            } else if (tmode[ctimer]==1) {
                graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 ONE ");
            } else if (tmode[ctimer]==2) {
                graphicsStringA(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 FREE");
            }
        }
        if (b & BUTTONS_AV300_MENU3) {
            while(buttonsGetStatusA() & BUTTONS_AV300_MENU3) {}
            timersTriggerA(ctimer);
        }
    }

}

void intsub() {
    uartOutsA("INT called\n");
}

