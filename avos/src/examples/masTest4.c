#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <power.h>
#include <fonts.h>
#include <ata.h>
#include <mas.h>
#include <masc.h>
#include <uart.h>
#include <gio.h>
#include <dma.h>

#include <string.h>
#include <stdio.h>
#include <math.h>

char buff[] = "xxxxxxxx\n";

unsigned int buffmem[1];

unsigned int dmamem[8];

int main() {
    unsigned int c, v, b;

    c = gioGetAllDirectionsA();
    gioSetAllDirectionsA(c & 0xffff00ff);
    
    uartOutsA("GIO: ");
    c = gioGetAllBitsetsA();
    stringPutHexA(buff, c, 8);
    uartOutsA(buff);
    
    masResetA();
    
    c = masGetVersionA();

    uartOutsA("GetVersion: ");
    stringPutHexA(buff, c, 8);    
    uartOutsA(buff);
    
    masConfigAudioCodec(0x0c, 0x0c, 0x0c, MAS_CONFIG_INPUT_MIC
                                        | MAS_CONFIG_DAC_ENABLE
                                        | MAS_CONFIG_ADCLEFT_ENABLE);
    
    masConfigInput(MAS_CONFIGINPUT_MONO);

    // 01111110
    // 01111010
    // 01110010
    //
    //     MM       ?
    
    masConfigOutput(0x00, 0x40, 0);
    masSetBalance(0x00);
    masSetVolume(0x60);
    
    buffmem[0] = 4;
    masWriteD0A(0x7f2, buffmem, 1);     // No mute
    
    buffmem[0] = 0x125;
    masWriteD0A(0x7f1, buffmem, 1);     // Demand mode, no monitoring, validate
    
    buffmem[0] = 0x0c;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!

    while(1) {
        uartOutsA("Waiting for app start...");
        masReadD0A(0x7f7, buffmem, 1);      // App running
        if (buffmem[0]==0x0c) break;
    }

    
    while(1) {
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;

        uartOutsA("GIO: ");
        c = gioGetAllBitsetsA();
        stringPutHexA(buff, c, 8);
        uartOutsA(buff);

        // Try some DMA?
        uartOutsA("Trying DMA xfer to S4...\n");
        dmaSetSourceA(dmamem);
        dmaSetDestA(0);
        dmaSetSizeA(8);
        dmaDevSelectA(DMA_DEV_SDRAM, DMA_DEV_CS4);
        dmaStartA(DMA_ENDIAN_3210);
        
        while(dmaIsRunningA()) {
            uartOutsA(".");
        }
        uartOutsA("\nDONE\n");

        uartOutsA("\nMasMem 7f0:\n");
        
        for (c=0x7f0;c<0x800;c++) {
            masReadD0A(c, buffmem, 1);
            stringPutHexA(buff, buffmem[0], 8);
            uartOutsA(buff);
        }

        uartOutsA("\nMasMem fd0:\n");
        
        for (c=0xfd0;c<0xfd8;c++) {
            masReadD0A(c, buffmem, 1);
            stringPutHexA(buff, buffmem[0], 8);
            uartOutsA(buff);
        }
    }
}
