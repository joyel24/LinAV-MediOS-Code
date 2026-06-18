#include "ibdma.h"

ioport unsigned short port0;
ioport unsigned short port1;
ioport unsigned short port2;
ioport unsigned short port3;
ioport unsigned short port4;
ioport unsigned short port5;
ioport unsigned short port6;
ioport unsigned short port7;
ioport unsigned short port8;
ioport unsigned short port9;
ioport unsigned short portA;

void ibDma_start(unsigned long sdAddr,unsigned short sdOffset,
				 unsigned short ibAddr, unsigned short ibOffset,
				 unsigned short x,unsigned short y,
				 short buffer, short direction, short byteToWord){

	int shift=0;

	sdAddr=sdAddr/2;

	port0=sdAddr/0x10000;
	port1=sdAddr%0x10000;
	port2=sdOffset/2;

	port3=ibAddr;
	port4=ibOffset/2;

	port5=x/2;
	port6=y;

	switch(chip_num){
		case 25:
			shift=IB_DSC25_BUF_SHIFT;
			break;
		case 27:
			shift=IB_DM270_BUF_SHIFT;
			break;
		case 32:
			shift=IB_DM320_BUF_SHIFT;
			break;
	}

	port8=0x03<<(buffer*shift);

	if(chip_num==27){
		port9=(byteToWord)?0x0004:0x0000;
	}else if(chip_num>=32){
		portA=(byteToWord)?0x0004:0x0000;
	}else{
		port9=0x0000;
	}

	port7=(direction&0x0001)|0x0002;
}

short ibDma_pending(){
	return (port7&0x0002)!=0;
}

void ibDma_reset(){
	port8=0x0000;
}


