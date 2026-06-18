#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"


static uint8 regs[4];

/////////////////////////////////////////////////////////////////////
// Mapper 255
static void map255_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,2,3);
	mmc_bankrom(32,0x8000,0);

	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);

	//set_mirroring(NES_PPU::MIRROR_VERT);
	ppu_mirror(0,1,0,1);

	regs[1] = regs[3] = regs[0] = regs[2] = 0x0F;	 
}

static uint8 map255_MemoryReadLow(uint32 addr)
{
	if(addr >= 0x5800)
	{
		return regs[addr & 0x0003];
	}
	else
	{
		return (uint8)(addr >> 8);
	}
}

static void map255_MemoryWriteLow(uint32 addr, uint8 data)
{
	if(addr >= 0x5800)
	{
		regs[addr & 0x0003] = data & 0x0F;
	}
}

static void map255_MemoryWrite(uint32 addr, uint8 data)
{		
	uint8 rom_bank = (addr >> 14) & 0x01;//(uint8)((addr & 0x4000) >> 14);
	uint8 prg_bank = ((addr >> 7) & 0x1F) | (rom_bank << 5);//(uint8)((addr & 0x0F80) >> 7);
	uint8 chr_bank = ((addr >> 0) & 0x3F) | (rom_bank << 6);//(uint8)(addr & 0x003F);
	
	
	if(addr & 0x2000)
	{
		//set_mirroring(NES_PPU::MIRROR_HORIZ);
		ppu_mirror(0,0,1,1);
	}
	else
	{
		//set_mirroring(NES_PPU::MIRROR_VERT);
		ppu_mirror(0,1,0,1);
	}

	if(addr & 0x1000)
	{//16K PRG_ROM		
		mmc_bankrom(16,0x8000,( (prg_bank << 1) | ((addr & 0x40) >> 6) ));
		mmc_bankrom(16,0xC000,( (prg_bank << 1) | ((addr & 0x40) >> 6) ));
	}
	else
	{//32K PRG_ROM		
		mmc_bankrom(32,0x8000,prg_bank);
	}
	
	mmc_bankvrom(8,0x0000,chr_bank);
}

static void map255_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper255.registers[0] = regs[0];
	state->extraData.mapper255.registers[1] = regs[1];
	state->extraData.mapper255.registers[2] = regs[2];
	state->extraData.mapper255.registers[3] = regs[3];
}

static void map255_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper255.registers[0];
	regs[1] = state->extraData.mapper255.registers[1];
	regs[2] = state->extraData.mapper255.registers[2];
	regs[3] = state->extraData.mapper255.registers[3];
}

mapintf_t map255_intf =
{
   255, /* mapper number */
   "115-in-1", /* mapper name */
   map255_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map255_getstate, /* get state (snss) */
   map255_setstate, /* set state (snss) */
   map255_MemoryReadLow,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map255_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map255_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

