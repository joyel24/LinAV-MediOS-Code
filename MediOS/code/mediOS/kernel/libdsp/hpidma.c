#include "hpidma.h"

ioport unsigned short port80;
ioport unsigned short port81;
ioport unsigned short port82;
ioport unsigned short port83;
ioport unsigned short port84;
ioport unsigned short port85;
ioport unsigned short port86;
ioport unsigned short port8000;
ioport unsigned short port8001;
ioport unsigned short port8002;
ioport unsigned short port8003;
ioport unsigned short port8004;
ioport unsigned short port8005;
ioport unsigned short port8006;


void hpiDma_start(unsigned long sdAddr,void * dspAddr,unsigned short size, short direction){
    if(chip_num>=32){
        port8000=sdAddr%0x10000;
        port8001=sdAddr/0x10000;
        port8002=(unsigned short)dspAddr;
        port8003=0; // dsp page
        port8004=size;
        port8005=direction; // sdram access, keep byte order

        port8006=1; // trigger
    }else if(chip_num==27){
        port80=sdAddr%0x10000;
        port81=sdAddr/0x10000;
        port82=(unsigned short)dspAddr;
        port83=0; // dsp page
        port84=size;
        port85=direction; // sdram access, keep byte order

        port86=1; // trigger
    }
}

short hpiDma_pending(){
    if(chip_num>=32){
        return (port8006&0x1)!=0;
    }else if(chip_num==27){
        return (port86&0x1)!=0;
    }else{
        return 0;
    }
}

