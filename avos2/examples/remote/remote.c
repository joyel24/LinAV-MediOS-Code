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

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

void intsub();

int main() {
    unsigned int a,b,c,d;
    
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

    graphicsStringA(&screenBitmap, 0, 0, &sprite8_13, std8x13_, 9, 0,
                "Remote v0.01 (c)DoGgEr");

    graphicsStringA(&screenBitmap, 0, 14*2, &sprite8_13, std8x13_, 9, 0,
                "Count:");
    graphicsStringA(&screenBitmap, 0, 14*3, &sprite8_13, std8x13_, 9, 0,
                "Data:");
    graphicsStringA(&screenBitmap, 0, 14*4, &sprite8_13, std8x13_, 9, 0,
                "Repeat:");
                
    cpldSetModeA(4);
    cpldSetReg2A(0x0b);
    cpldSetReg1A(0x06);

    interruptsInitA((void*)intsub);    
    interruptsSetMaskA(0xffffff7f);     // 0100 = DVR thing?
    interruptsSetIRQEnabledA();
    
    gioSetAllIRQsA(0xff);    // Set all gio (0-7) to be IRQ!

    while(1) {

        a = buttonsGetStatusA();
        if (a & BUTTONS_AV300_OFF) { 
            interruptsSetIRQDisabledA();
            return;
        }
    }
}

static unsigned int v=0,f=0,lf=0,state=0,count=0,data=0;
static unsigned int dataCount=0,repCount=0;
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
                    repCount++;
                    stringPutHexA(hex8, repCount, 8);
                    graphicsStringA(&screenBitmap, 8*9, 14*4, &sprite8_13, std8x13_, 9, 0, hex8);
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
                stringPutHexA(hex8, data, 8);
                graphicsStringA(&screenBitmap, 8*9, 14*3, &sprite8_13, std8x13_, 9, 0, hex8);
                stringPutHexA(hex8, dataCount, 8);
                graphicsStringA(&screenBitmap, 8*9, 14*2, &sprite8_13, std8x13_, 9, 0, hex8);

                debug("data=%08x\n", data);
                state=0;
                dataCount++;
            }
        }
        interruptsResetIRQA(i);
        return;
    }

    interruptsResetIRQA(i);    

}

