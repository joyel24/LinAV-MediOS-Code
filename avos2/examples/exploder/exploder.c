#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <stdio.h>
#include <gio.h>
#include <interrupts.h>

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite6_9 = {0, 1, 6, 9, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

void exploderShow();
void exploderButtons();
void exploderInit();
void updateInt(int i);
void intsub();

static char* saddr = (char*) 0x30000;
static int vaddr = 0;
static int changeaddr = 1;

int main() {
    int b, mode=0;

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

    // Assume DVR for now...
    cpldSetModeA(4);
    cpldSetReg2A(0x0b);
    cpldSetReg1A(0x06);

    interruptsInitA((void*)intsub);    
    interruptsSetMaskA(0xffffff7f);
    interruptsSetIRQEnabledA();
    gioSetAllIRQsA(0xff);               // Set all gio (0-7) to be IRQ!
    
    exploderInit();

    for (b=0;b<32;b++) {
        updateInt(b);
    }
    
    while(1) {
        exploderShow();
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return 0;

        exploderButtons(b);        
    }

}


void exploderInit() {
    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);

    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);


    graphicsStringA(&screenBitmap, 6, 10, &sprite6_9, std6x9_, 6, 0, "        ");
    graphicsStringA(&screenBitmap, 6+(6*vaddr), 10, &sprite6_9, std6x9_, 6, 0, "*");
}
    
void exploderButtons(int b) {
    int delay;
    if (b & BUTTONS_AV300_LEFT) {
        vaddr--;
        vaddr = (vaddr & 7);
        pal16[1] = 0xffff;
        graphicsStringA(&screenBitmap, 6, 10, &sprite6_9, std6x9_, 6, 0, "        ");
        graphicsStringA(&screenBitmap, 6+(6*vaddr), 10, &sprite6_9, std6x9_, 6, 0, "*");
        for (delay=0;delay<0x8000;delay++) {}
    } else if (b & BUTTONS_AV300_RIGHT) {
        vaddr++;
        vaddr = (vaddr & 7);
        pal16[1] = 0xffff;
        graphicsStringA(&screenBitmap, 6, 10, &sprite6_9, std6x9_, 6, 0, "        ");
        graphicsStringA(&screenBitmap, 6+(6*vaddr), 10, &sprite6_9, std6x9_, 6, 0, "*");
        for (delay=0;delay<0x8000;delay++) {}
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

char* pbuff = "xxxx";
char* paddr = "xxxxxxxx";
char* pchars = "xxxxxxxxxxxxxxxx";

static short lastbuff[8*32];
static unsigned int lastGio = 0;
static char gioBits[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

void exploderShow() {
    int y, x, a;

    for (y=0;y<16;y++) {
        if (changeaddr==1) {
            sprintf(paddr, "%08X", (int) (saddr + (y<<4)));
            pal16[1] = 0xffff;
            graphicsStringA(&screenBitmap, 6, 10 + 9*(y+1), &sprite6_9, std6x9_, 6, 0, paddr);
        }

        pal16[1] = 0xffff;
        for (x=0;x<8;x++) {
            a = ((unsigned short*)saddr)[(y<<3) + x];
            if (changeaddr || (a!=lastbuff[x + (y<<3)])) {
                sprintf(pbuff, "%04X", a);
                graphicsStringA(&screenBitmap, 6*10+(6*5*x), 9*(y+2), &sprite6_9, std6x9_, 6, 0, pbuff);            
            }
            lastbuff[(y<<3) + x] = a;
        }
    }
    changeaddr = 0;
    
    a = gioGetAllBitsetsA();
    if (a!=lastGio) {
        lastGio = a;
        for (a=0;a<32;a++) {
            if ((lastGio >> a) & 1)
                gioBits[a]='1';
            else
                gioBits[a]='0';
        }
        graphicsStringA(&screenBitmap, 6*10, 9*23, &sprite6_9, std6x9_, 6, 0, gioBits);        
    }
    
}

static int irqCounts[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static char hex4[] = "xxxx";

void updateInt(int i) {
    int x, y, op;  
    
    x = i & 7;
    y = i >> 3;
    
    stringPutHexA(hex4, irqCounts[i], 4);
    graphicsStringA(&screenBitmap, (10 + x*5)*6, (19+y)*9, &sprite6_9, std6x9_, 6, 0, hex4);    
}

void intsub() {
    int i=0,c=0;
    c = interruptsGetCausesA();
    c = c | (~interruptsGetMaskA());
    
    for (i=0;i<32;i++) {
        if (!(c&1)) break;
        c = c>>1;
    }

    irqCounts[i]++;

//    updateInt(i);

    interruptsResetIRQA(i);
}

