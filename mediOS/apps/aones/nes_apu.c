#include "medios.h"
#include "unes.h"
#include "nes_apu.h"

void apu_reset(){
    dspCom->sndWantApuReset=1;
    // wait for the dsp to process the reset
    while(dspCom->sndWantApuReset) /* nothing */;
}

void apu_write(uint32 Addr, uint8 Value){
    volatile sndItem * item = &dspCom->sndQueue[dspCom->sndHead];

    item->address=Addr;
    item->value=Value;

    dsp_write32(&item->tick,CPU_GET_CYCLES());

    dspCom->sndHead=(dspCom->sndHead+1)&SNDQUEUE_MASK;
};

uint8 apu_read(uint32 Addr){
    uint8 Value;

    switch (Addr)
    {
        case 0x4015:
            Value=dspCom->sndStatusReg;
            break;
        default:
            Value = (Addr >> 8); /* heavy capacitance on data bus */
            break;
    }

    return Value;
};

