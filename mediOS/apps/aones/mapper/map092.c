#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 92
static void map92_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

// Created by rinao
static void map92_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 c_bank;
	uint8 p_bank;
	data = (uint8)addr & 0xff;
	c_bank = (data & 0x0f) << 1;
	p_bank = data & 0x0f;

	if (addr >= 0x9000){ // Moero!! ProSoccer
		if ((data & 0xf0) == 0xd0){
			mmc_bankromALL4(0,1,c_bank,c_bank+1);
		} else if ((data & 0xf0) == 0xe0){
			mmc_bankvromALL8(p_bank*8,p_bank*8+1,p_bank*8+2,p_bank*8+3,p_bank*8+4,p_bank*8+5,p_bank*8+6,p_bank*8+7);
		}
	} else { // Moero!! Proyakyuu '88 ketteiban
		if ((data & 0xf0) == 0xb0){
			mmc_bankromALL4(0,1,c_bank,c_bank+1);
		} else if ((data & 0xf0) == 0x70){
			mmc_bankvromALL8(p_bank*8,p_bank*8+1,p_bank*8+2,p_bank*8+3,p_bank*8+4,p_bank*8+5,p_bank*8+6,p_bank*8+7);
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map92_getstate(SnssMapperBlock *state)
{

}


static void map92_setstate(SnssMapperBlock *state)
{

}


mapintf_t map92_intf =
{
   92, /* mapper number */
   "Map92", /* mapper name */
   map92_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map92_getstate, /* get state (snss) */
   map92_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map92_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

