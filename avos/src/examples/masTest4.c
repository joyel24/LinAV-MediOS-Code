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

    masResetA();
    
    c = masGetVersionA();

    uartOutsA("GetVersion: ");
    stringPutHexA(buff, c, 8);    
    uartOutsA(buff);
    
    masConfigInput(MAS_CONFIGINPUT_MONO);
    masConfigOutput(0x40, 0x00, 0);
    masSetBalance(0x00);
    masConfigAudioCodec(0x0f, 0x0f, 0x0f, MAS_CONFIG_INPUT_MIC
                                        | MAS_CONFIG_DAC_ENABLE
                                        | MAS_CONFIG_ADCLEFT_ENABLE
                                        | MAS_CONFIG_ADCRIGHT_ENABLE);
    
    masSetVolume(0x60);

    buffmem[0] = 0x00000000;
    masWriteD0A(0x7f6, buffmem, 1);     // App select

    while(1) {
        uartOutsA("Waiting for all stopped...");
        masReadD0A(0x7f7, buffmem, 1);      // App running
        if (buffmem[0]==0) break;
    }

    masWriteRegA(0xa3, 0x98);
    masWriteRegA(0x94, 0xffff);
    buffmem[0] = 0x00000000;
    masWriteD1A(0, buffmem, 1);
    masWriteRegA(0xa3, 0x90);
    
    // Apply settings now...

    buffmem[0] = 0x40000000;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!
    
    while(1) {
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;

        uartOutsA("GIO: ");
        c = gioGetAllBitsetsA();
        stringPutHexA(buff, c, 8);
        uartOutsA(buff);

        // Try some DMA?
        uartOutsA("Trying DMA xfer...\n");
        dmaSetSourceA(0);
        dmaSetDestA(dmamem);
        dmaSetSizeA(8);
        dmaDevSelectA(DMA_DEV_CS4, DMA_DEV_SDRAM);
        dmaStartA(DMA_ENDIAN_3210);
        
        while(dmaIsRunningA()) {
            uartOutsA(".");
        }
        uartOutsA("\nDONE\n");
        
        for (c=0;c<8;c++) {
            stringPutHexA(buff, dmamem[c], 8);
            uartOutsA(buff);
        }
        
        uartOutsA("\nMasMem:\n");
        
        for (c=0x7f0;c<0x800;c++) {
            masReadD0A(c, buffmem, 1);
            stringPutHexA(buff, buffmem[0], 8);
            uartOutsA(buff);
        }
    }
}
