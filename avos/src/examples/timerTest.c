#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <ata.h>
#include <fonts.h>
#include <timers.h>

    struct graphicsBuffer screenBitmap;
    struct graphicsBuffer screenTop;
    struct graphicsBuffer screenBitmap2;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

char timev[] = "xxxx";
char ct[] = "F1 Timer: x";

int main() {
    int c, v, b, x, y = 120;
    int ctimer=0;
    int div=1023;       // Slowest rate...
    int tmode[4] = {TIMERS_TMMD_STOP, TIMERS_TMMD_STOP, TIMERS_TMMD_STOP, TIMERS_TMMD_STOP};
    int tmax[4] = {65535, 65535, 65535, 65535};

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
    graphicsString(&screenTop, 4, 9, &sprite4_6, std4x6_, 5, 0, "TimerTest - up/down changes value. F3=Trigger");
    stringPutHex(ct + 10, ctimer, 1);
    graphicsString(&screenTop, 4, 2, &sprite4_6, std4x6_, 5, 0, ct);
    stringPutHex(timev, tmax[ctimer], 4);
    graphicsString(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
    if (tmode[ctimer]==0) {
        graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 STOP");
    } else if (tmode[ctimer]==1) {
        graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 ONE ");
    } else if (tmode[ctimer]==2) {
        graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 FREE");
    }
    
    for (c=0;c<4;c++) {
        timersConfig(c, tmode[c], 0, div, tmax[c]);        
    }

    while(1) {
        graphicsBoxf(&screenBitmap2, 320, 0, 1, 120, 0x0000);
        graphicsSprite(&screenBitmap, 0, 0, &screenBitmap2);
        
        for (c=0;c<4;c++) {
            v = timersGetValue(c);
            y = v * 120 >> 16;
            if (y>119) y=119;
            if (y<0) y=0;        
            graphicsSetPixel(&screenBitmap, 319, y, c+1);
        }
        
        b = buttonsGetStatus();
        if (b & BUTTONS_AV300_OFF) return;
        if (b & BUTTONS_AV300_DOWN) {
            tmax[ctimer]-=0x100;
            timersConfig(ctimer, tmode[ctimer], 0, div, tmax[ctimer]);
            stringPutHex(timev, tmax[ctimer], 4);
            graphicsString(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
        } else if (b & BUTTONS_AV300_UP) {
            tmax[ctimer]+=0x100;
            timersConfig(ctimer, tmode[ctimer], 0, div, tmax[ctimer]);
            stringPutHex(timev, tmax[ctimer], 4);
            graphicsString(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
        }
        
        if (b & BUTTONS_AV300_MENU1) {
            while(buttonsGetStatus() & BUTTONS_AV300_MENU1) {}
            ctimer++;
            if (ctimer==4) ctimer=0;
            stringPutHex(ct + 10, ctimer, 1);
            graphicsString(&screenTop, 4, 2, &sprite4_6, std4x6_, 5, 0, ct);
            stringPutHex(timev, tmax[ctimer], 4);
            graphicsString(&screenTop, 4 + 12*5, 2, &sprite4_6, std4x6_, 5, 0, timev);
            if (tmode[ctimer]==0) {
                graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 STOP");
            } else if (tmode[ctimer]==1) {
                graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 ONE ");
            } else if (tmode[ctimer]==2) {
                graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 FREE");
            }            
        }
        if (b & BUTTONS_AV300_MENU2) {
            while(buttonsGetStatus() & BUTTONS_AV300_MENU2) {}
            tmode[ctimer]++;
            if (tmode[ctimer]==3) tmode[ctimer]=0;
            timersConfig(ctimer, tmode[ctimer], 0, div, tmax[ctimer]);
            if (tmode[ctimer]==0) {
                graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 STOP");
            } else if (tmode[ctimer]==1) {
                graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 ONE ");
            } else if (tmode[ctimer]==2) {
                graphicsString(&screenTop, 4 + 17*5, 2, &sprite4_6, std4x6_, 5, 0, "F2 FREE");
            }
        }
        if (b & BUTTONS_AV300_MENU3) {
            while(buttonsGetStatus() & BUTTONS_AV300_MENU3) {}
            timersTrigger(ctimer);
        }
    }

}
