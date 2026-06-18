#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 prg_reg[4];
static uint8 chr_low_reg[8];
static uint8 chr_high_reg[8];
static uint8 nam_low_reg[4];
static uint8 nam_high_reg[4];

static uint8 prg_bank_size;
static uint8 prg_bank_6000;
static uint8 prg_bank_e000;
static uint8 chr_bank_size;
static uint8 mirror_mode;
static uint8 mirror_type;
static uint32 value1;

static uint8 irq_enabled;
static uint8 irq_counter;
static uint8 irq_latch;

static uint8 mode;

/////////////////////////////////////////////////////////////////////
// Mapper 90

void map90_Sync_Mirror()
{
	uint8 i;
	uint32 nam_bank[4];

	for(i = 0; i < 4; i++)
	{
		nam_bank[i] = ((uint32)nam_high_reg[i] << 8) | (uint32)nam_low_reg[i];
	}

	if(mirror_mode)
	{
		for(i = 0; i < 4; i++)
		{
			if(!nam_high_reg[i] && (nam_low_reg[i] == i))
			{
				mirror_mode = 0;
			}
		}

		if(mirror_mode)
		{
			/*mmc_bankvrom8(nam_bank[0]);
			mmc_bankvrom9(nam_bank[1]);
			mmc_bankvrom10(nam_bank[2]);
			mmc_bankvrom11(nam_bank[3]);*/
			mmc_bankvrom8(nam_bank[0]);
			mmc_bankvrom9(nam_bank[1]);
			mmc_bankvrom10(nam_bank[2]);
			mmc_bankvrom11(nam_bank[3]);
		}
	}
	else
	{
		if(mirror_type == 0)
		{
			//set_mirroring(NES_PPU::MIRROR_VERT);
			ppu_mirror(0,1,0,1);
		}
		else if(mirror_type == 1)
		{
			//set_mirroring(NES_PPU::MIRROR_HORIZ);
			ppu_mirror(0,0,1,1);
		}
		else
		{
			//set_mirroring(0,0,0,0);
			ppu_mirror(0,0,0,0);
		}
	}
}

void map90_Sync_Chr_Banks()
{
	uint8 i;
	uint32 chr_bank[8];

	for(i = 0; i < 8; i++)
	{
		chr_bank[i] = ((uint32)chr_high_reg[i] << 8) | (uint32)chr_low_reg[i];
	}

	if(chr_bank_size == 0)
	{
		mmc_bankvrom0(chr_bank[0]*8+0);
		mmc_bankvrom1(chr_bank[0]*8+1);
		mmc_bankvrom2(chr_bank[0]*8+2);
		mmc_bankvrom3(chr_bank[0]*8+3);
		mmc_bankvrom4(chr_bank[0]*8+4);
		mmc_bankvrom5(chr_bank[0]*8+5);
		mmc_bankvrom6(chr_bank[0]*8+6);
		mmc_bankvrom7(chr_bank[0]*8+7);
	}
	else if(chr_bank_size == 1)
	{
		mmc_bankvrom0(chr_bank[0]*4+0);
		mmc_bankvrom1(chr_bank[0]*4+1);
		mmc_bankvrom2(chr_bank[0]*4+2);
		mmc_bankvrom3(chr_bank[0]*4+3);
		mmc_bankvrom4(chr_bank[4]*4+0);
		mmc_bankvrom5(chr_bank[4]*4+1);
		mmc_bankvrom6(chr_bank[4]*4+2);
		mmc_bankvrom7(chr_bank[4]*4+3);
	}
	else if(chr_bank_size == 2)
	{
		mmc_bankvrom0(chr_bank[0]*2+0);
		mmc_bankvrom1(chr_bank[0]*2+1);
		mmc_bankvrom2(chr_bank[2]*2+0);
		mmc_bankvrom3(chr_bank[2]*2+1);
		mmc_bankvrom4(chr_bank[4]*2+0);
		mmc_bankvrom5(chr_bank[4]*2+1);
		mmc_bankvrom6(chr_bank[6]*2+0);
		mmc_bankvrom7(chr_bank[6]*2+1);
	}
	else
	{
		mmc_bankvrom0(chr_bank[0]);
		mmc_bankvrom1(chr_bank[1]);
		mmc_bankvrom2(chr_bank[2]);
		mmc_bankvrom3(chr_bank[3]);
		mmc_bankvrom4(chr_bank[4]);
		mmc_bankvrom5(chr_bank[5]);
		mmc_bankvrom6(chr_bank[6]);
		mmc_bankvrom7(chr_bank[7]);
	}
}

void map90_Sync_Prg_Banks()
{
#if 1
	if(prg_bank_size == 0)
	{
		/*set_CPU_bank4(num_8k_ROM_banks-4);
		set_CPU_bank5(num_8k_ROM_banks-3);
		set_CPU_bank6(num_8k_ROM_banks-2);
		set_CPU_bank7(num_8k_ROM_banks-1);*/
		mmc_bankromALL4(num_8k_ROM_banks-4,num_8k_ROM_banks-3,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
	else if(prg_bank_size == 1)
	{
		/*set_CPU_bank4(prg_reg[1]*2);
		set_CPU_bank5(prg_reg[1]*2+1);
		set_CPU_bank6(num_8k_ROM_banks-2);
		set_CPU_bank7(num_8k_ROM_banks-1);*/
		mmc_bankromALL4(prg_reg[1]*2,prg_reg[1]*2+1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
	else if(prg_bank_size == 2)
	{
		if(prg_bank_e000)
		{
			/*set_CPU_bank4(prg_reg[0]);
			set_CPU_bank5(prg_reg[1]);
			set_CPU_bank6(prg_reg[2]);
			set_CPU_bank7(prg_reg[3]);*/
			mmc_bankromALL4(prg_reg[0],prg_reg[1],prg_reg[2],prg_reg[3]);
		}
		else
		{
			if(prg_bank_6000)
			{
				//set_CPU_bank3(prg_reg[3]);
				mmc_bankrom3(prg_reg[3]);
			}
			/*set_CPU_bank4(prg_reg[0]);
			set_CPU_bank5(prg_reg[1]);
			set_CPU_bank6(prg_reg[2]);
			set_CPU_bank7(num_8k_ROM_banks-1);*/
			mmc_bankromALL4(prg_reg[0],prg_reg[1],prg_reg[2],num_8k_ROM_banks-1);
		}
	}
	else
	{
		// 8k in reverse mode?
		/*set_CPU_bank4(prg_reg[3]);
		set_CPU_bank5(prg_reg[2]);
		set_CPU_bank6(prg_reg[1]);
		set_CPU_bank7(prg_reg[0]);*/
		mmc_bankromALL4(prg_reg[3],prg_reg[2],prg_reg[1],prg_reg[0]);
	}
#else
	switch(mode&3){
	case 0:
		if(mode&4){
			uint32 bn = prg_reg[3]<<2;
			//set_CPU_banks(bn, bn+1, bn+2, bn+3);
			mmc_bankromALL4(bn,bn+1,bn+2,bn+3);
		}
		else{
			/*set_CPU_bank4(num_8k_ROM_banks-4);
			set_CPU_bank5(num_8k_ROM_banks-3);
			set_CPU_bank6(num_8k_ROM_banks-2);
			set_CPU_bank7(num_8k_ROM_banks-1);*/
			mmc_bankromALL4(num_8k_ROM_banks-4,num_8k_ROM_banks-3,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
		}
		if(mode&0x80){
			//set_CPU_bank3((prg_reg[3]<<2)|3);
			mmc_bankrom3((prg_reg[3]<<2)|3);
		}
		break;
	case 1:
		/*set_CPU_bank4(prg_reg[1]<<1);
		set_CPU_bank5(prg_reg[1]<<1+1);*/
		mmc_bankrom(16,0x8000,prg_reg[1]);
		if(mode&4){
			/*set_CPU_bank6(prg_reg[3]<<1);
			set_CPU_bank7(prg_reg[3]<<1+1);*/
			mmc_bankrom(16,0xC000,prg_reg[3]);
		}
		else{
			/*set_CPU_bank6(num_8k_ROM_banks-2);
			set_CPU_bank7(num_8k_ROM_banks-1);*/
			mmc_bankrom(16,0xC000,(num_8k_ROM_banks-1)>>1);
		}
		if(mode&0x80){
			//set_CPU_bank3((prg_reg[3]<<1)|1);
			mmc_bankrom3((prg_reg[3]<<1)|1);
		}
		break;
	case 2:		
		mmc_bankrom4(prg_reg[0]);
		mmc_bankrom5(prg_reg[1]);
		mmc_bankrom6(prg_reg[2]);

		if(mode & 0x04){
			mmc_bankrom7(prg_reg[3]);
		}
		else{
			mmc_bankrom7(num_8k_ROM_banks-1);
		}
		if(mode & 0x80){
			mmc_bankrom3(prg_reg[3]);
		}
		break;
	case 3:
		mmc_bankrom4(prg_reg[2]);
		mmc_bankrom5(prg_reg[3]);
		mmc_bankrom6(prg_reg[0]);

		if(mode & 0x04){
			mmc_bankrom7(prg_reg[1]);
		}
		else{
			mmc_bankrom7(num_8k_ROM_banks-1);
		}
		if(mode & 0x80){
			mmc_bankrom3(prg_reg[3]);
		}
		break;
		break;
	}
#endif
}


void map90_Reset()
{
	uint8 i;
	// set CPU bank pointers
	/*set_CPU_bank4(num_8k_ROM_banks-4);
	set_CPU_bank5(num_8k_ROM_banks-3);
	set_CPU_bank6(num_8k_ROM_banks-2);
	set_CPU_bank7(num_8k_ROM_banks-1);*/
	mmc_bankromALL4(num_8k_ROM_banks-4,num_8k_ROM_banks-3,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	//mmc_bankvroms(0,1,2,3,4,5,6,7);

	irq_counter = 0;
	irq_latch = 0;
	irq_enabled = 0;

	chr_bank_size=0;
	prg_bank_size=0;

	for(i = 0; i < 4; i++)
	{
		prg_reg[i] = num_8k_ROM_banks-4+i;
		nam_low_reg[i] = 0;
		nam_high_reg[i] = 0;
		chr_low_reg[i] = i;
		chr_high_reg[i] = 0;
		chr_low_reg[i+4] = i+4;
		chr_high_reg[i+4] = 0;
	}
	value1^=1;
	mode = 0;
}

uint8 map90_MemoryReadLow(uint32 addr)
{
#if 0
	if(addr == 0x5000)
	{
		return (uint8)(value1*value2 & 0x00FF);
	}
	else
	{
		return (uint8)(addr >> 8);
	}
#else
	if(value1&1)
		return 0xFF;
	else
		return 0;
#endif
}

void map90_MemoryWriteLow(uint32 addr, uint8 data)
{
#if 0
	if(addr == 0x5000)
	{
		value1 = data;
	}
	else if(addr == 0x5001)
	{
		value2 = data;
	}
#endif
}

void map90_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr&0xF007)
	{
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		{
			prg_reg[addr & 0x03] = data;
			map90_Sync_Prg_Banks();
			//	  LOG("W " << HEX(addr,4) << "  " << HEX(data,2) << "  " << HEX(prg_bank_size,2) << endl);
		}
		break;

	case 0x9000:
	case 0x9001:
	case 0x9002:
	case 0x9003:
	case 0x9004:
	case 0x9005:
	case 0x9006:
	case 0x9007:
		{
			chr_low_reg[addr & 0x07] = data;
			map90_Sync_Chr_Banks();
		}
		break;

	case 0xA000:
	case 0xA001:
	case 0xA002:
	case 0xA003:
	case 0xA004:
	case 0xA005:
	case 0xA006:
	case 0xA007:
		{
			chr_high_reg[addr & 0x07] = data;
			map90_Sync_Chr_Banks();
		}
		break;

	case 0xB000:
	case 0xB001:
	case 0xB002:
	case 0xB003:
		{
			nam_low_reg[addr & 0x03] = data;
			map90_Sync_Mirror();
		}
		break;

	case 0xB004:
	case 0xB005:
	case 0xB006:
	case 0xB007:
		{
			nam_high_reg[addr & 0x03] = data;
			map90_Sync_Mirror();
		}
		break;

	case 0xC002:
		{
			irq_enabled = 0;
		}
		break;

	case 0xC003:
	case 0xC004:
		{
			if(irq_enabled == 0)
			{
				irq_enabled = 1;
				irq_counter = irq_latch;
			}
		}
		break;

	case 0xC005:
		{
			irq_counter = data;
			irq_latch = data;
		}
		break;

	case 0xD000:
		{
			prg_bank_6000 = data & 0x80;
			prg_bank_e000 = data & 0x04;
			prg_bank_size = data & 0x03;
			chr_bank_size = (data & 0x18) >> 3;
			mirror_mode = data & 0x20;
			map90_Sync_Prg_Banks();
			map90_Sync_Chr_Banks();
			map90_Sync_Mirror();
			mode = data;
		}
		break;

	case 0xD001:
		{
			mirror_type = data & 0x03;
			map90_Sync_Mirror();
		}
		break;

	case 0xD003:
		{
			// bank page
		}
		break;
	}
	//  LOG("W " << HEX(addr,4) << "  " << HEX(data,2) << "  " << HEX(prg_bank_size,2) << endl);
}

void map90_HSync(uint32 scanline)
{
	if((scanline >= 0) && (scanline <= 239)) //239
	{
		//if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
		if (ppu_enabled())
		{
			if(--irq_counter == 0)
			{
				if(irq_enabled)
				{
					//parent_NES->cpu->DoIRQ();
					doIRQ();
				}
				//		 irq_counter = irq_latch;
				//       irq_latch = 0;
				//         irq_enabled = 0;
			}
			else
			{
				//		  irq_counter--;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////

static void map90_getstate(SnssMapperBlock *state)
{

}


static void map90_setstate(SnssMapperBlock *state)
{

}


mapintf_t map90_intf =
{
   90, /* mapper number */
   "Map90", /* mapper name */
   map90_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map90_getstate, /* get state (snss) */
   map90_setstate, /* set state (snss) */   
   map90_MemoryReadLow,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map90_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map90_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

