#include <uart.h>
#include <dma.h>
#include <buttons.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <fonts.h>

char srcBuff[] = "Hello world";
char destBuff[12];

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

int main() {
    
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
                "DMA test v0.01 (c)DoGgEr");

    graphicsStringA(&screenBitmap, 0, 14, &sprite8_13, std8x13_, 9, 0,
                "Original: ");
    graphicsStringA(&screenBitmap, 0+(10*9), 14, &sprite8_13, std8x13_, 9, 0,
                srcBuff);
                
    
    uartOutsA("dmaTest starting...\n");
    
    uartOutsA("Waiting for dma to be idle ");
    while(dmaIsRunningA()) {
        uartOutsA(".");
    }
    uartOutsA("Done!\n");
    
    uartOutsA("Setting up for transfer...\n");
    
    dmaSetSourceA(srcBuff - 0x03000000);
    dmaSetDestA(destBuff - 0x03000000);
    dmaSetSizeA(12);
    dmaDevSelectA(DMA_DEV_SDRAM, DMA_DEV_SDRAM);
    
    uartOutsA("dmaStart()...\n");
    dmaStartA(0);        // Go!
    
    uartOutsA("Waiting for dma to be idle ");
    while(dmaIsRunningA()) {
        uartOutsA(".");
    }    
    uartOutsA("Done!\n");

    graphicsStringA(&screenBitmap, 0, 28, &sprite8_13, std8x13_, 9, 0,
                "DMA Copy: ");
    graphicsStringA(&screenBitmap, 0+(10*9), 28, &sprite8_13, std8x13_, 9, 0,
                destBuff);
    
    uartOutsA("Printing out result: [");
    uartOutsA(destBuff);
    uartOutsA("]\n");
    
    while (!(buttonsGetStatusA() & BUTTONS_AV300_ANY)) {}
}
