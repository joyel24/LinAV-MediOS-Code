#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[1];

	//  uint32 irq_counter, irq_latch;
	//  uint8 irq_enabled;

/////////////////////////////////////////////////////////////////////
// Mapper 74
static void map74_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0, 1, num_8k_ROM_banks-2, num_8k_ROM_banks-1);
//	set_CPU_banks(0, 1, 2, 3);
	regs[0]=0;
	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}


static void map74_MemoryWrite(uint32 addr, uint8 data)
{
	//	char f=0;
	switch(addr&0xff0f){
	case 0x8000:
		mmc_bankrom4(data&0x3f);
		break;
	case 0x8001:
		mmc_bankrom5(data&0x3f);
		break;
	case 0xE000:
		mmc_bankvrom1(data);
		break;
	}
	//LOG("W " << HEX(addr,4) << "  " << HEX(data,2) << "  " << HEX(regs[0],2) << endl);

}

#if 0
static void map74_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if(scanline<241){
			irq_counter--;
			if(irq_counter<=0){
				//parent_NES->cpu->DoIRQ();
				doIRQ();
				irq_enabled = 0;
				irq_counter = irq_latch;
			}
		}
	}
}
#endif

/////////////////////////////////////////////////////////////////////

static void map74_getstate(SnssMapperBlock *state)
{

}


static void map74_setstate(SnssMapperBlock *state)
{

}


mapintf_t map74_intf =
{
   74, /* mapper number */
   "map74", /* mapper name */
   map74_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map74_getstate, /* get state (snss) */
   map74_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map74_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};





