#include <buttons.h>
#include <power.h>
#include <ata.h>
#include <mas.h>
#include <masc.h>
#include <uart.h>
#include <gio.h>
#include <interrupts.h>
#include <file.h>
#include <debug.h>

void intsub();
void feedMas();

static unsigned int buffmem[1];

int main() {
    unsigned int c, v, b, a, i;

    masResetA();
    
    for(c=0;c<100;c++) {}
        
    debug("GetVersion: %08x\n", masGetVersionA());

    for(c=0;c<100;c++) {}
   
    debug("masControl %04x\n", masControlReadA());
    
    masControlWriteA(masControlReadA() | 0xc00);

    debug("masControl %04x\n", masControlReadA());
    
    for(c=0;c<100;c++) {}

    c = gioGetAllDirectionsA();
    gioSetAllDirectionsA(c | 0xff00);
    gioSetAllInvertsA(0x00000010);      // Just EOD inverted...
    gioSetAllIRQsA(0xff);
    interruptsInitA(intsub);
    interruptsSetIRQEnabledA();    
    interruptsSetMaskA(interruptsGetMaskA() | 0xffffff7f);

    masConfigAudioCodec(0x0c, 0x0c, 0x0c, MAS_CONFIG_INPUT_MIC
                                        | MAS_CONFIG_ADCLEFT_ENABLE
                                        | MAS_CONFIG_ADCRIGHT_ENABLE
                                        | MAS_CONFIG_DAC_ENABLE);
    
    masConfigInput(MAS_CONFIGINPUT_MONO);    
    masConfigOutput(0x40, 0x40, 0);
    masSetBalance(0x00);
    masSetVolume(0x73);

    for(c=0;c<20;c++) {}
    
    buffmem[0] = 0x0;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - none
    while(1) {
        uartOutsA("Waiting for app end...\n");
        masReadD0A(0x7f7, buffmem, 1);      // App running
        if (buffmem[0]==0) break;
    }

    buffmem[0] = 0x04;                  // Check. 0d/04?
    masWriteD0A(0x7f2, buffmem, 1);
    
    buffmem[0] = 0x125;
    masWriteD0A(0x7f1, buffmem, 1);     // Demand mode

    debug("Starting app...");
    
    for(c=0;c<20;c++) {}
    
    buffmem[0] = 0x40;
    masWriteD0A(0x7f6, buffmem, 1);     // App select - RUN!

    while(1) {
        masReadD0A(0x7f7, buffmem, 1);      // App running
        debug("Waiting for app start [%02x]...\n", buffmem[0]);
        if (buffmem[0]==0x40) break;
    }
    
    debug("App started...\n");
            
    while(1) {
        if (buttonsGetStatusA() & BUTTONS_AV300_OFF) return;

        feedMas();
        
        debug("\nMasMem:\n");
        
        for (c=0x7f0;c<0x800;c++) {
            masReadD0A(c, buffmem, 1);
            debug("%08x: %08x\n", c, buffmem[0]);
        }
        for (c=0xfd0;c<0xfd8;c++) {
            masReadD0A(c, buffmem, 1);
            debug("%08x: %08x\n", c, buffmem[0]);
        }
    }

    
}

int cnt=0;

void intsub() {
    int i=0,c=0;
    c = interruptsGetCausesA();
    c = c | (~interruptsGetMaskA());
//    debug("INT called ");
//    debug("CAUSES %08x MASK %08x ", c, interruptsGetMaskA());
//    debug("CAUSES2 %08x MASK2 %08x\n", interruptsGetCauses2A(), interruptsGetMask2A());
    
    for (i=0;i<32;i++) {
        if (!(c&1)) break;
        c = c>>1;
    }

    debug("ID=%d\n", i);

    //interruptsResetMaskA(i);      One shot
    interruptsResetIRQA(i);
    
//    debug("INT RET ");
//    debug("CAUSES %08x MASK %08x ", interruptsGetCausesA(), interruptsGetMaskA());
//    debug("CAUSES2 %08x MASK2 %08x\n", interruptsGetCauses2A(), interruptsGetMask2A());
}

char dataBuff[256];

void feedMas() {
    int i;
    i = masReadDataA(dataBuff, 256);
    debug("Data %08x\n", i);
}
