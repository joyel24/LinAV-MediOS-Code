#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <ata.h>
#include <fonts.h>
#include <mas.h>

    struct graphicsBuffer screenBitmap;

int main() {
    unsigned int c, v, b, x, y = 120;
    unsigned int pixoff = 0;
    unsigned int scroff = 0;
    unsigned int drawx = 321;
    unsigned int drawx2 = 1;
    unsigned int showin = 0;
    unsigned int mono = 1;
    unsigned int inr=0, inl=0, outr=0, outl=0;
    
    ataPowerDownHDD();

    osdInit();

    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 640*2;          // Make it twice the screen
    screenBitmap.width = 640;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap, 0, 0, 640, 240, 0x0000);

    osdSetMainShift(0x54, 0x12);

    osdSetComponentSize(OSD_BITMAP1, 336*2, 240);
    osdSetComponentPosition(OSD_BITMAP1, 0, 0);
    osdSetComponentOffset(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x28);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);

    masReset();
    c = masGetVersion();
    
    c = masWriteCodecReg(0x08, 0x8000);     // MONO input
    c = masWriteCodecReg(0x0e, 0x0000);
    c = masWriteCodecReg(0x11, 0x0000);     // Balance = center

    c = masWriteCodecReg(0x10, 0x6000);     // Volume

    c = masWriteCodecReg(0x06, 0x4000);     // Input gain

    c = masWriteCodecReg(0, 0xffff);        // Enable stuff

    
    while(1) {
        
        b = (masReadCodecReg(0x000c) & 0x7fff);
        outl = 119 - ((b * 119) >> 15);
        if (mono) {
            outr = 239-outl;
        } else {
            b = (masReadCodecReg(0x000d) & 0x7fff);
            outr = 120 + ((b * 119) >> 15);
        }
        
        graphicsBoxf(&screenBitmap, drawx, 0, 1, 240, 0x0000);        
        graphicsBoxf(&screenBitmap, drawx2, 0, 1, 240, 0x0000);        
        graphicsBoxf(&screenBitmap, drawx, outl, 1, outr-outl, 0x0404);
        graphicsBoxf(&screenBitmap, drawx2, outl, 1, outr-outl, 0x0404);
         
        if (showin) {
            b = (masReadCodecReg(0x000a) & 0x7fff);
            inl = 119 - ((b * 59) >> 15);
            if (mono) {
                inr = 239 - inl;    
            } else {
                b = (masReadCodecReg(0x000b) & 0x7fff);
                inr = 120 + ((b * 59) >> 15);
            }
            graphicsSetPixel(&screenBitmap, drawx, inl, 0x0101);
            graphicsSetPixel(&screenBitmap, drawx2, inl, 0x0101);
            graphicsSetPixel(&screenBitmap, drawx, inr, 0x0101);
            graphicsSetPixel(&screenBitmap, drawx2, inr, 0x0101);        
        }
        
        drawx++;
        if (drawx>639) drawx=0;
        drawx2++;
        if (drawx2>639) drawx2=0;

        pixoff++;
        if (pixoff==16) {
            pixoff = 0;
            scroff++;
            if (scroff==0x14) scroff=0;      // Jump back
            osdSetComponentOffset(OSD_BITMAP1, 0x03800000 + (scroff<<5));   // *16*2            
        }
        osdSetComponentPosition(OSD_BITMAP1, 0x14 + 0x20 - (pixoff<<1), 0);
        
//        do {
            b = buttonsGetStatus();
//        } while (!(b & BUTTONS_AV300_ANY));
        if (b & BUTTONS_AV300_OFF) return;
        if (b & BUTTONS_AV300_MENU1) {
            showin = 1 - showin;
            do {
                b = buttonsGetStatus();
            } while ((b & BUTTONS_AV300_ANY));
        } else if (b & BUTTONS_AV300_MENU2) {
            mono = 1 - mono;
            do {
                b = buttonsGetStatus();
            } while ((b & BUTTONS_AV300_ANY));
        }
    }

}
