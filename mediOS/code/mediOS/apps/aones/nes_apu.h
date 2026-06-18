#ifndef __NES_APU_H
#define __NES_APU_H

void apu_reset();
void apu_write(uint32 Addr, uint8 Value);
uint8 apu_read(uint32 Addr);

#endif



