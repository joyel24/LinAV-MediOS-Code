#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <ata.h>
#include <fonts.h>
#include <mas.h>

    static int pal32[2] = {0x004a4956, 0x00000000};
    struct graphicsBuffer screenBitmap;
    struct graphicsBuffer screenBitmap2;
    struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, 0, (int**) &pal32};
    
int main() {
    unsigned int c, v, b, x, y = 120, div;
    unsigned int pixoff = 0;
    unsigned int scroff = 0;
    unsigned int drawx = 320;
    unsigned int showin = 0;
    unsigned int mono = 1;
    unsigned int inr=0, inl=0, outr=0, outl=0;
    
    ataPowerDownHDDA();
    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 656*2;          // Make it twice the screen + a bit
    screenBitmap.width = 656;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;
    
    screenBitmap2.offset = 0x03c00000;
    screenBitmap2.bytesPerLine = 320*4;
    screenBitmap2.width = 320;
    screenBitmap2.height = 240;
    screenBitmap2.bitsPerPixelShift = 5;
    screenBitmap2.bitsPerPixel = 32;

    graphicsBoxfA(&screenBitmap, 0, 0, 656, 240, 0x0000);
    graphicsBoxfA(&screenBitmap2, 0, 0, 320, 240, 0x004a4956);

    for (y=0;y<=120;y+=30) {
        graphicsBoxfA(&screenBitmap2, 0, y, 320, 1, 0x00000000);
        graphicsBoxfA(&screenBitmap2, 0, 239 - y, 320, 1, 0x00000000);        
    }

    for (x=0;x<=160;x+=32) {
        graphicsBoxfA(&screenBitmap2, x, 0, 1, 240, 0x00000000);
        graphicsBoxfA(&screenBitmap2, 319-x, 0, 1, 240, 0x00000000);        
    }
    
    graphicsStringA(&screenBitmap2, 2, 1, &sprite8_13, std8x13_, 8, 0,
                "Mas test3 By DoGgEr");

    osdSetMainShiftA(0x14 + 0x40, 0x13);

    osdSetComponentSizeA(OSD_BITMAP2, 336*2, 240);
    osdSetComponentPositionA(OSD_BITMAP2, 0x40, 0);
    osdSetComponentOffsetA(OSD_BITMAP2, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP2, 0x29);
    osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_0TRANS
                                     | OSD_BITMAP_A1
                                     | OSD_BITMAP_8BIT);

    osdSetComponentSizeA(OSD_VIDEO1, 320*2, 240);
    osdSetComponentPositionA(OSD_VIDEO1, 0x40 - 8, 0);
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);

    masResetA();
    c = masGetVersionA();
    
    c = masWriteCodecRegA(0x08, 0x8000);     // MONO input
    c = masWriteCodecRegA(0x0e, 0x0000);
    c = masWriteCodecRegA(0x11, 0x0000);     // Balance = center

    c = masWriteCodecRegA(0x10, 0x6000);     // Volume

    c = masWriteCodecRegA(0x06, 0x4000);     // Input gain

    c = masWriteCodecRegA(0, 0xffff);        // Enable stuff

    
    while(1) {
        
        b = (masReadCodecRegA(0x000c) & 0x7fff);
        outl = 119 - ((b * 119) >> 15);
        if (mono) {
            outr = 239-outl;
        } else {
            b = (masReadCodecRegA(0x000d) & 0x7fff);
            outr = 120 + ((b * 119) >> 15);
        }

        // Work out new display position after opdate...        
        pixoff++;        
        if (pixoff==16) {
            pixoff=0;
            scroff++;
            if (scroff==0x15) {
                scroff=0;
                pixoff=0;
                drawx=320;                
            }
        }

        // Draw pixels to the right of display window
        //
        graphicsBoxfA(&screenBitmap, drawx, 0, 1, 240, 0x0000);        
        graphicsBoxfA(&screenBitmap, drawx, outl, 1, outr-outl, 0x0202);
         
        if (showin) {
            b = (masReadCodecRegA(0x000a) & 0x7fff);
            inl = 119 - ((b * 59) >> 15);
            if (mono) {
                inr = 239 - inl;    
            } else {
                b = (masReadCodecRegA(0x000b) & 0x7fff);
                inr = 120 + ((b * 59) >> 15);
            }
            graphicsSetPixelA(&screenBitmap, drawx, inl, 0x0101);
            graphicsSetPixelA(&screenBitmap, drawx, inr, 0x0101);
        }
        // Done draw...

        drawx++;

        osdSetComponentOffsetA(OSD_BITMAP2, 0x03800000 + (scroff<<5));   // *16*2            
        osdSetComponentPositionA(OSD_BITMAP2, 0x14 + 0x20 - (pixoff<<1), 0);

        if (drawx>=336) {
            graphicsBoxfA(&screenBitmap, drawx-336, 0, 1, 240, 0x0000);        
            graphicsBoxfA(&screenBitmap, drawx-336, outl, 1, outr-outl, 0x0202);
            if (showin) {
                graphicsSetPixelA(&screenBitmap, drawx-336, inr, 0x0101);        
                graphicsSetPixelA(&screenBitmap, drawx-336, inl, 0x0101);
            }
        }

//        do {
        b = buttonsGetStatusA();
//        } while(!(b & BUTTONS_AV300_ANY));
        if (b & BUTTONS_AV300_OFF) return;
        if (b & BUTTONS_AV300_MENU1) {
            showin = 1 - showin;
            do {
                b = buttonsGetStatusA();
            } while ((b & BUTTONS_AV300_ANY));
        } else if (b & BUTTONS_AV300_MENU2) {
            mono = 1 - mono;
            do {
                b = buttonsGetStatusA();
            } while ((b & BUTTONS_AV300_ANY));
        } else if (b & BUTTONS_AV300_MENU3) {
            do {
                b = buttonsGetStatusA();
            } while ((b & BUTTONS_AV300_ANY));            
        }
    }

}
