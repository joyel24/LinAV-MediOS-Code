#include <uart.h>
#include <dma.h>
#include <buttons.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <gio.h>
#include <cpld.h>
#include <interrupts.h>
#include <debug.h>
#include <uart.h>
#include <timers.h>

char hex2[] = "xx";

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

void intsub();

int main() {
    unsigned int a,b,c,d;
    int cursor=0;
    
    osdInitA();

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

    graphicsStringA(&screenBitmap, 0, 0, &sprite5_8, std5x8_, 6, 0,
                "GIO test v0.01 (c)DoGgEr");

    graphicsStringA(&screenBitmap, 280, 150, &sprite5_8, std5x8_, 6, 0,
                "IN/OUT");
    graphicsStringA(&screenBitmap, 280, 180, &sprite5_8, std5x8_, 6, 0,
                "INVERT");
    graphicsStringA(&screenBitmap, 280, 210, &sprite5_8, std5x8_, 6, 0,
                "Exit");

    cpldSetModeA(4);
    cpldSetReg2A(0x0b);
    cpldSetReg1A(0x06);

    interruptsInitA((void*)intsub);    
    interruptsSetMaskA(0xffffff7f);     // 0100 = DVR thing?
    interruptsSetIRQEnabledA();
    
    gioSetAllIRQsA(0xff);    // Set all gio (0-7) to be IRQ!


    while(1) {
        c = gioGetAllDirectionsA();
        d = gioGetAllInvertsA();
        
        for (a=0;a<16;a++) {
            if (cursor==a) {
                pal16[0] = 0x0404;
            } else {
                pal16[0] = 0x0000;
            }
            stringPutHexA(hex2, a, 2);
            graphicsStringA(&screenBitmap, 0, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
            b = gioGetBitA(a);
            stringPutHexA(hex2, b, 2);
            graphicsStringA(&screenBitmap, 15, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, ": ");
            if ((c>>a) & 1) {
                graphicsStringA(&screenBitmap, 25, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "IN ");
            } else {
                graphicsStringA(&screenBitmap, 25, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "OUT");
            }
            if ((d>>a) & 1) {
                graphicsStringA(&screenBitmap, 40, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "~");
            } else {
                graphicsStringA(&screenBitmap, 40, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, " ");
            }
            graphicsStringA(&screenBitmap, 50, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
            //
            //
            if (cursor==(a+16)) {
                pal16[0] = 0x0404;
            } else {
                pal16[0] = 0x0000;
            }
            stringPutHexA(hex2, a+16, 2);
            graphicsStringA(&screenBitmap, 160, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
            b = gioGetBitA(a+16);
            stringPutHexA(hex2, b, 2);
            graphicsStringA(&screenBitmap, 175, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, ": ");
            if ((c>>(a+16)) & 1) {
                graphicsStringA(&screenBitmap, 185, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "IN ");
            } else {
                graphicsStringA(&screenBitmap, 185, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "OUT");
            }
            if ((d>>(a+16)) & 1) {
                graphicsStringA(&screenBitmap, 200, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, "~");
            } else {
                graphicsStringA(&screenBitmap, 200, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, " ");
            }
            graphicsStringA(&screenBitmap, 210, 10 + (a*9), &sprite5_8, std5x8_, 5, 0, hex2);
        
        }

        a = buttonsGetStatusA();
        if (a & BUTTONS_AV300_UP) {
            if (cursor>0) cursor--;
        } else if (a & BUTTONS_AV300_DOWN) {
            if (cursor<31) cursor++;    
        } else if (a & BUTTONS_AV300_LEFT) {
            gioClearBitA(cursor);    
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        } else if (a & BUTTONS_AV300_RIGHT) {
            gioSetBitA(cursor);                
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        } else if (a & BUTTONS_AV300_MENU1) {
            b = gioGetAllDirectionsA();
            b ^= (1<<cursor);
            gioSetAllDirectionsA(b);
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        } else if (a & BUTTONS_AV300_MENU2) {
            b = gioGetAllInvertsA();
            b ^= (1<<cursor);
            gioSetAllInvertsA(b);
            while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
        }
        if (a & BUTTONS_AV300_MENU3) return;
    }
}

static unsigned int v=0,f=0,lf=0,state=0,count=0,data=0;
static unsigned int dataCount=0,repCount=0;
char hex4[] = "xxxx";
char hex8[] = "xxxxxxxx";

void intsub() {
    int i=0,c=0, op0, op1;
    c = interruptsGetCausesA();
    c = c | (~interruptsGetMaskA());
    //debug("INT called ");
    //debug("CAUSES %08x MASK %08x ", c, interruptsGetMaskA());
    //debug("CAUSES2 %08x MASK2 %08x\n", interruptsGetCauses2A(), interruptsGetMask2A());
    
    for (i=0;i<32;i++) {
        if (!(c&1)) break;
        c = c>>1;
    }

    if (i==2) {
        timersConfigA(2, TIMERS_TMMD_STOP, TIMERS_TMCLK_EXT, 10, 280);
        // call 4da03(27)
        
        // 43d(2)   - mask this IRQ...
        
        interruptsResetIRQA(2);
        return; 
    }

    if (i==0x18) {    
        interruptsResetIRQA(i);    

        if (state==0) {
            timersConfigA(2, TIMERS_TMMD_FREERUN, TIMERS_TMCLK_EXT, 269, 0xffff);
            interruptsResetIRQA(2);
            state=1;
            lf=0;
        } else if (state==1) {
            v = timersGetValueA(2);
            f = v - lf;
            lf = v;

            if (f<0x7d0) {
                if (f>0x514 && f<0x578) {           // 13548?
                    // On code
                    state=2;
                    count=0;
                    data=0;
                    repCount=0;
                } else if (f>0x44c && f<0x4b0) {    // 11294?
                    // Repeat...
                    debug("44c:4b0\n");
                    repCount++;
                    op0 = pal16[0];
                    op1 = pal16[1];
                    
                    pal16[0] = 0x0000;
                    pal16[1] = 0xffff;
                    stringPutHexA(hex8, repCount, 8);
                    graphicsStringA(&screenBitmap, 4+(18*6), 210, &sprite5_8, std5x8_, 6, 0, hex8);
                    pal16[0] = op0;
                    pal16[1] = op1;
                }
                
            } else {
                timersConfigA(2, TIMERS_TMMD_FREERUN, TIMERS_TMCLK_EXT, 269, 0xffff);
                lf = 0;
            }
        } else if (state==2) {
            v = timersGetValueA(2);
            f = v - lf;
            lf = v;

            if (f>100 && f<130) {           // 1127?
                count++;
                data = (data << 1);
            } else if (f>200 && f<240) {    // 2254?
                count++;
                data = (data << 1) | 1;
            } else {
                count++;
                data = (data << 1);
            }
            
            if (count==32) {
                op0 = pal16[0];
                op1 = pal16[1];
                
                pal16[0] = 0x0000;
                pal16[1] = 0xffff;
                stringPutHexA(hex8, data, 8);
                graphicsStringA(&screenBitmap, 4, 210, &sprite5_8, std5x8_, 6, 0, hex8);
            
                stringPutHexA(hex8, dataCount, 8);
                graphicsStringA(&screenBitmap, 4+(9*6), 210, &sprite5_8, std5x8_, 6, 0, hex8);
                pal16[0] = op0;
                pal16[1] = op1;

                debug("data=%08x\n", data);
                state=0;
                dataCount++;
            }
        }
        interruptsResetIRQA(i);
        return;
    }
//    op0 = pal16[0];
//    op1 = pal16[1];
    
//    pal16[0] = 0x0000;
//    pal16[1] = 0xffff;
//    stringPutHexA(hex4, v, 4);
//    graphicsStringA(&screenBitmap, 4, 210, &sprite5_8, std5x8_, 6, 0, hex4);

//    stringPutHexA(hex4, i, 4);
//    graphicsStringA(&screenBitmap, 4+(5*6), 210, &sprite5_8, std5x8_, 6, 0, hex4);
//    pal16[0] = op0;
//    pal16[1] = op1;

    interruptsResetIRQA(i);    

}

