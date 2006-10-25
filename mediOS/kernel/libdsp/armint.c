#include "armint.h"

ioport unsigned short portFFFF;
ioport unsigned short port280;

void armInt_trigger(){
    if(chip_num>=32){
		// Interrupt ARM
		port280 |= 0x0008;
	}else{
		// Interrupt ARM
		portFFFF |= 0x0001;

		// Clear interrupt ARM
		portFFFF &= 0xFFFE;
    }
}
