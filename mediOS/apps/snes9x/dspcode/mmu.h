#include "port.h"

START_EXTERN_C

typedef uint16 u16;

void mmu_reset();
u16 mmu_getbyte(u16 addr);
u16 * mmu_getbyteptr(u16 addr);
void mmu_getbytes(u16 addr, int size, short * buffer);
void mmu_setbyte(u16 addr, u16 value);
void mmu_setbytes(u16 addr, int size, const short * buffer);

void queue_dsp(u16 value);

#define GET(addr) mmu_getbyte(addr)
#define SET(addr, value) mmu_setbyte(addr, value)

END_EXTERN_C
