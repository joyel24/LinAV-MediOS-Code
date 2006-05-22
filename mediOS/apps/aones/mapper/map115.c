#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[1];

static uint32 irq_counter, irq_latch;
static uint8 irq_enabled;

/////////////////////////////////////////////////////////////////////
// Mapper 115
static void map115_Reset()
{
	// set CPU bank pointers
	//mmc_bankromALL4(0, 1, num_8k_ROM_banks-2, num_8k_ROM_banks-1);
	regs[0]=0;
	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		mmc_bankvromALL8(0,1,2,3,4,5,6,7);
	}*/
	irq_latch=irq_counter=irq_enabled=0;
}


static void map115_MemoryWrite(uint32 addr, uint8 data)
{
	char f=0;
	switch(addr&0xf001){
	case 0x8000:
		regs[0]=data;
		f=1;
		break;
	case 0x8001:
		switch(regs[0]&7){
		case 0:
			mmc_bankvrom0(data&0x1f);
			mmc_bankvrom1(data&(0x1f+1));
			break;
		case 1:
			mmc_bankvrom2(data&0x1f);
			mmc_bankvrom3(data&(0x1f+1));
			break;
		case 2:
			mmc_bankvrom4(data&0x1f);
			break;
		case 3:
			mmc_bankvrom5(data&0x1f);
			break;
		case 4:
			mmc_bankvrom6(data&0x1f);
			break;
		case 5:
			mmc_bankvrom7(data&0x1f);
			break;
		case 6:
			mmc_bankrom4(data&0x0f);
			break;
		case 7:
			mmc_bankrom5(data&0x0f);
			break;
		}
		f=1;
		break;
	case 0xA000:
		//			irq_enabled=data;
		break;
	case 0xC000:
		data&=0x1f;
		//			irq_counter=data;
		//			mmc_bankvrom1(data);
		break;
	case 0xE000:
		//			mmc_bankrom4(data&0x0f);
		mmc_bankvromALL8(data, data+1, data+2, data+3, data+4, data+5, data+6, data+7);
		break;
	}
	if(f==0){
		//LOG("W " << HEX(addr,4) << "  " << HEX(data,2) << "  " << HEX(regs[0],2) << endl);
	}

}


static void map115_HSync(uint32 scanline)
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

/////////////////////////////////////////////////////////////////////


static void map115_getstate(SnssMapperBlock *state)
{
	
}


static void map115_setstate(SnssMapperBlock *state)
{
	
}


mapintf_t map115_intf =
{
   115, /* mapper number */
   "Map115", /* mapper name */
   map115_Reset, /* init routine */
   NULL, /* vblank callback */
   map115_HSync, /* hblank callback */
   map115_getstate, /* get state (snss) */
   map115_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map115_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

