#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <stdio.h>

    struct graphicsBuffer screenBitmap;
    static int pal16[2] = {0x0000, 0xffff};

    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

void printBuff();

static char* saddr = (char*) 0x30000;
static int vaddr = 0;

static int changeaddr = 1;

int main() {
    int b;

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

    
    while(1) {
        printBuff();
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return 0;
        if (b & BUTTONS_AV300_LEFT) {
            vaddr--;
            vaddr = (vaddr & 7);
        } else if (b & BUTTONS_AV300_RIGHT) {
            vaddr++;
            vaddr = (vaddr & 7);
        } else if (b & BUTTONS_AV300_UP) {
            if (vaddr==7) saddr -= (1<<(4*(7-vaddr))); 
            saddr -= (1<<(4*(7-vaddr)));
            changeaddr = 1;
        } else if (b & BUTTONS_AV300_DOWN) {
            if (vaddr==7) saddr += (1<<(4*(7-vaddr))); 
            saddr += (1<<(4*(7-vaddr)));
            changeaddr = 1;
        }
    }

}

char* pbuff = "xxxx";
char* paddr = "xxxxxxxx";
char* pchars = "xxxxxxxxxxxxxxxx";

static short lastbuff[8*32];

void printBuff() {
    int y, x, a;

    pal16[1] = 0x0101;
    graphicsStringA(&screenBitmap, 4, 0, &sprite4_6, std4x6_, 4, 0, "        ");
    graphicsStringA(&screenBitmap, 4+(4*vaddr), 0, &sprite4_6, std4x6_, 4, 0, "*");

    for (y=0;y<32;y++) {
        if (changeaddr==1) {
            sprintf(paddr, "%08X", (int) (saddr + (y<<4)));
            pal16[1] = 0x0202;
            graphicsStringA(&screenBitmap, 4, 7*(y+1), &sprite4_6, std4x6_, 4, 0, paddr);
        }

        pal16[1] = 0xffff;
        for (x=0;x<8;x++) {
            a = ((unsigned short*)saddr)[(y<<3) + x];
            if (changeaddr || (a!=lastbuff[x + (y<<3)])) {
                sprintf(pbuff, "%04X", a);
                graphicsStringA(&screenBitmap, 4*10+(5*4*x), 7*(y+1), &sprite4_6, std4x6_, 4, 0, pbuff);            
            }
            lastbuff[(y<<3) + x] = a;
        }
            
        for (x=0;x<16;x++) {            
            a = saddr[(y<<4) + x];
            if (a<0x20 || a>0x7f) a='.'; 
            pchars[x] = a;
        }
        pal16[1] = 0x0303;
        graphicsStringA(&screenBitmap, 4*50, 7*(y+1), &sprite4_6, std4x6_, 4, 0, pchars);
    }
    changeaddr = 0;
}

