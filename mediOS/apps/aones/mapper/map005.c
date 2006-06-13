#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint32 wb[8];
//	uint8 wram[8*0x2000];
static uint8 *wram;
static uint8 wram_size;

static uint8 chr_reg[8][2];

static uint8 irq_enabled;
static uint8 irq_status;
static uint32 irq_line;

static uint32 value0;
static uint32 value1;

static uint8 wram_protect0;
static uint8 wram_protect1;
static uint8 prg_size;
static uint8 chr_size;
static uint8 gfx_mode;
static uint8 split_control;
static uint8 split_bank;


/////////////////////////////////////////////////////////////////////
// Mapper 5

static void sync_Chr_banks(uint8 mode)
{
	if(chr_size == 0)
	{
		/*set_PPU_bank0(chr_reg[7][mode]*8+0);
		set_PPU_bank1(chr_reg[7][mode]*8+1);
		set_PPU_bank2(chr_reg[7][mode]*8+2);
		set_PPU_bank3(chr_reg[7][mode]*8+3);
		set_PPU_bank4(chr_reg[7][mode]*8+4);
		set_PPU_bank5(chr_reg[7][mode]*8+5);
		set_PPU_bank6(chr_reg[7][mode]*8+6);
		set_PPU_bank7(chr_reg[7][mode]*8+7);*/
		mmc_bankvrom(8,0x0000,chr_reg[7][mode]);
	}
	else if(chr_size == 1)
	{
		/*set_PPU_bank0(chr_reg[3][mode]*4+0);
		set_PPU_bank1(chr_reg[3][mode]*4+1);
		set_PPU_bank2(chr_reg[3][mode]*4+2);
		set_PPU_bank3(chr_reg[3][mode]*4+3);
		set_PPU_bank4(chr_reg[7][mode]*4+0);
		set_PPU_bank5(chr_reg[7][mode]*4+1);
		set_PPU_bank6(chr_reg[7][mode]*4+2);
		set_PPU_bank7(chr_reg[7][mode]*4+3);*/
		mmc_bankvrom(4,0x0000,chr_reg[3][mode]);
		mmc_bankvrom(4,0x1000,chr_reg[7][mode]);		
	}
	else if(chr_size == 2)
	{
		/*set_PPU_bank0(chr_reg[1][mode]*2+0);
		set_PPU_bank1(chr_reg[1][mode]*2+1);
		set_PPU_bank2(chr_reg[3][mode]*2+0);
		set_PPU_bank3(chr_reg[3][mode]*2+1);
		set_PPU_bank4(chr_reg[5][mode]*2+0);
		set_PPU_bank5(chr_reg[5][mode]*2+1);
		set_PPU_bank6(chr_reg[7][mode]*2+0);
		set_PPU_bank7(chr_reg[7][mode]*2+1);*/
		mmc_bankvromALL8(chr_reg[1][mode]*2+0,chr_reg[1][mode]*2+1,chr_reg[3][mode]*2+0,chr_reg[3][mode]*2+1,
		chr_reg[5][mode]*2+0,chr_reg[5][mode]*2+1,chr_reg[7][mode]*2+0,chr_reg[7][mode]*2+1);
	}
	else
	{
		/*set_PPU_bank0(chr_reg[0][mode]);
		set_PPU_bank1(chr_reg[1][mode]);
		set_PPU_bank2(chr_reg[2][mode]);
		set_PPU_bank3(chr_reg[3][mode]);
		set_PPU_bank4(chr_reg[4][mode]);
		set_PPU_bank5(chr_reg[5][mode]);
		set_PPU_bank6(chr_reg[6][mode]);
		set_PPU_bank7(chr_reg[7][mode]);*/
		mmc_bankvromALL8(chr_reg[0][mode],chr_reg[1][mode],chr_reg[2][mode],chr_reg[3][mode],
		chr_reg[4][mode],chr_reg[5][mode],chr_reg[6][mode],chr_reg[7][mode]);
	}
}

static uint8 map5_PPU_Latch_RenderScreen(uint8 mode, uint32 addr)
{
	uint8 ex_pal = 0;

	if(gfx_mode == 1 && mode == 1)
	{
		// ex gfx mode
		//uint8* nametable2 = parent_NES->ppu->get_namt() + 0x800;
		uint8* nametable2 = (uint8*)(mmc_getinfo()->PPU_nametables);
		uint32 bank = (nametable2[0x800|addr] & 0x3F) << 2;
		//set_PPU_banks(bank,bank+1,bank+2,bank+3,bank,bank+1,bank+2,bank+3);
		mmc_bankvromALL8(bank,bank+1,bank+2,bank+3,bank,bank+1,bank+2,bank+3);
		ex_pal = ((nametable2[0x800|addr] & 0xC0) >> 4) | 0x01;
	}
	else
	{
		// normal
		sync_Chr_banks(mode);
	}
	return ex_pal;
}


static void MMC5_set_WRAM_bank(uint8 page, uint8 bank)
{
	if(bank != 8)
	{
		if(wram_size == 1) bank = (bank > 3) ? 8 : 0;
		if(wram_size == 2) bank = (bank > 3) ? 1 : 0;
		if(wram_size == 3) bank = (bank > 3) ? 8 : bank;
		if(wram_size == 4) bank = (bank > 3) ? 4 : bank;
	}
	wb[page] = bank;

	if(bank != 8)
	{
		/*NES_6502::Context context;
		parent_NES->cpu->GetContext(&context);
		context.mem_page[page] = wram + bank*0x2000;
		parent_NES->cpu->SetContext(&context);*/
		mmc_getinfo()->CPUPageIndex[page]=(byte*)(wram + bank*0x2000);
		#ifdef __asmcpu__   		
		switch (page)
		{
			case 3:init_sram(wram + bank*0x2000);reload_fast_pc(); break;
			case 4:set_cpu_bank0(wram + bank*0x2000); break;
			case 5:set_cpu_bank1(wram + bank*0x2000); break;
			case 6:set_cpu_bank2(wram + bank*0x2000); break;
			case 7:set_cpu_bank3(wram + bank*0x2000); break;			
		}
   		#else
   		//nes6502 uses directly CPUPageIndex...
   		// so jsut calling to have pfast_pc_update if needed	
   		nes6502_update_fast_pc();
   		#endif
   		
	}
}


static void MMC5_set_CPU_bank(uint8 page, uint8 bank)
{
	if(bank & 0x80)
	{
		if(prg_size == 0)
		{
			if(page == 7)
			{
				/*set_CPU_bank4((bank & 0x7C)+0);
				set_CPU_bank5((bank & 0x7C)+1);
				set_CPU_bank6((bank & 0x7C)+2);
				set_CPU_bank7((bank & 0x7C)+3);*/
				mmc_bankrom(32,0x8000,(bank & 0x7C)>>2);
				wb[4] = wb[5] = wb[6] = 8;
			}
		}
		if(prg_size == 1)
		{
			if(page == 5)
			{
				/*set_CPU_bank4((bank & 0x7E)+0);
				set_CPU_bank5((bank & 0x7E)+1);*/
				mmc_bankrom(16,0x8000,(bank & 0x7E)>>1);
				wb[4] = wb[5] = 8;
			}
			if(page == 7)
			{
				/*set_CPU_bank6((bank & 0x7E)+0);
				set_CPU_bank7((bank & 0x7E)+1);*/
				mmc_bankrom(16,0xC000,(bank & 0x7E)>>1);
				wb[6] = 8;
			}
		}
		if(prg_size == 2)
		{
			if(page == 5)
			{
				/*set_CPU_bank4((bank & 0x7E)+0);
				set_CPU_bank5((bank & 0x7E)+1);*/
				mmc_bankrom(16,0x8000,(bank & 0x7E)>>1);				
				wb[4] = wb[5] = 8;
			}
			if(page == 6)
			{
				//set_CPU_bank6(bank & 0x7F);
				mmc_bankrom(8,0xC000,(bank & 0x7F));
				wb[6] = 8;
			}
			if(page == 7)
			{
				//set_CPU_bank7(bank & 0x7F);
				mmc_bankrom(8,0xE000,(bank & 0x7F));
			}
		}
		if(prg_size == 3)
		{
			if(page == 4)
			{
				//set_CPU_bank4(bank & 0x7F);
				mmc_bankrom(8,0x8000,(bank & 0x7F));
				wb[4] = 8;
			}
			if(page == 5)
			{
				//set_CPU_bank5(bank & 0x7F);
				mmc_bankrom(8,0xA000,(bank & 0x7F));
				wb[5] = 8;
			}
			if(page == 6)
			{
				//set_CPU_bank6(bank & 0x7F);
				mmc_bankrom(8,0xC000,(bank & 0x7F));
				wb[6] = 8;
			}
			if(page == 7)
			{
				//set_CPU_bank7(bank & 0x7F);
				mmc_bankrom(8,0xE000,(bank & 0x7F));
			}
		}
	}
	else
	{
		if(prg_size == 1)
		{
			if(page == 5)
			{
				MMC5_set_WRAM_bank(4, (bank & 0x06)+0);
				MMC5_set_WRAM_bank(5, (bank & 0x06)+1);
			}
		}
		if(prg_size == 2)
		{
			if(page == 5)
			{
				MMC5_set_WRAM_bank(4, (bank & 0x06)+0);
				MMC5_set_WRAM_bank(5, (bank & 0x06)+1);
			}
			if(page == 6)
			{
				MMC5_set_WRAM_bank(6, bank & 0x07);
			}
		}
		if(prg_size == 3)
		{
			if(page == 4)
			{
				MMC5_set_WRAM_bank(4, bank & 0x07);
			}
			if(page == 5)
			{
				MMC5_set_WRAM_bank(5, bank & 0x07);
			}
			if(page == 6)
			{
				MMC5_set_WRAM_bank(6, bank & 0x07);
			}
		}
	}
}


static void map5_Reset()
{
	uint32 i;
	wram_size = 0;

	if(mmc_getinfo()->var.crc == 0x2b548d75 || // Bandit Kings of Ancient China (J)
	        mmc_getinfo()->var.crc == 0xf4cd4998 || // Dai Koukai Jidai (J)
	        mmc_getinfo()->var.crc == 0x8fa95456 || // Ishin no Arashi (J)
	        mmc_getinfo()->var.crc == 0x98c8e090 || // Nobunaga no Yabou - Sengoku Gunyuu Den (J)
	        mmc_getinfo()->var.crc == 0x57e3218b || // L'Empereur (J)
	        mmc_getinfo()->var.crc == 0x2f50bd38 || // L'Empereur (U)
	        mmc_getinfo()->var.crc == 0x8e9a5e2f || // L'Empereur (Alt)(U)
	        mmc_getinfo()->var.crc == 0xb56958d1 || // Nobunaga's Ambition 2 (J)
	        mmc_getinfo()->var.crc == 0xe6c28c5f || // Suikoden - Tenmei no Chikai (J)
	        mmc_getinfo()->var.crc == 0xcd35e2e9)   // Uncharted Waters (J)
	{
		wram_size = 2;
	}

	if(mmc_getinfo()->var.crc == 0xf4120e58 || // Aoki Ookami to Shiroki Mejika - Genchou Hishi (J)
	        mmc_getinfo()->var.crc == 0x286613d8 || // Nobunaga no Yabou - Bushou Fuuun Roku (J)
	        mmc_getinfo()->var.crc == 0x11eaad26 || // Romance of the 3 Kingdoms 2 (J)
	        mmc_getinfo()->var.crc == 0x95ba5733)   // Sangokushi 2 (J)
	{
		wram_size = 3;
	}
	wram = (uint8*)mmc_getinfo()->mapper_extram;
	mmc_getinfo()->mapper_extramsize = 0x10000;

	// set SaveRAM
	for(i = 0; i < 0x10000; i++)
	{
		wram[i] = mmc_getinfo()->NESSRAM[i] = 0;//parent_NES->ReadSaveRAM(i);
	}
	MMC5_set_WRAM_bank(3,0);

	// Init ExSound
	//parent_NES->apu->SelectExSound(8);
	//exsound apu_setexchip(8);

	// set CPU bank pointers
	/*set_CPU_bank4(num_8k_ROM_banks-1);
	set_CPU_bank5(num_8k_ROM_banks-1);
	set_CPU_bank6(num_8k_ROM_banks-1);
	set_CPU_bank7(num_8k_ROM_banks-1);*/
	mmc_bankromALL4(num_8k_ROM_banks-1,num_8k_ROM_banks-1,num_8k_ROM_banks-1,num_8k_ROM_banks-1);

	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);	

	for(i = 0; i < 8; i++)
	{
		chr_reg[i][0] = i;
		chr_reg[i][1] = (i & 0x03) + 4;
	}
	wb[3] = 0;
	wb[4] = wb[5] = wb[6] = 8;

	prg_size = 3;
	wram_protect0 = 0x02;
	wram_protect1 = 0x01;
	chr_size = 3;
	gfx_mode = 0;

	irq_enabled = 0;
	irq_status = 0;
	irq_line = 0;

	split_control = 0;
	split_bank = 0;

	ppu_set_Latch_RenderScreen(map5_PPU_Latch_RenderScreen);
}

static uint8 map5_MemoryReadLow(uint32 addr)
{
	uint8 ret = (uint8)(addr >> 8);
	if(addr == 0x5204)
	{
		ret = irq_status;
		irq_status &= ~0x80;
	}
	else if(addr == 0x5205)
	{
		ret = (uint8)((value0*value1) & 0x00FF);
	}
	else if(addr == 0x5206)
	{
		ret = (uint8)(((value0*value1) & 0xFF00) >> 8);
	}
	else if(addr >= 0x5C00 && addr <=0x5FFF)
	{
		if(gfx_mode == 2 || gfx_mode == 3)
		{
			//uint8* nametable2 = parent_NES->ppu->get_namt() + 0x800;
			uint8* nametable2 = (uint8*)(mmc_getinfo()->PPU_nametables);
			ret = nametable2[(addr & 0x3FF)|0x800];
		}
	}
	return ret;
}

static void map5_MemoryWriteLow(uint32 addr, uint8 data)
{
	uint32 i;

	switch(addr)
	{
	case 0x5100:
		{
			prg_size = data & 0x03;
		}
		break;

	case 0x5101:
		{
			chr_size = data & 0x03;
		}
		break;

	case 0x5102:
		{
			wram_protect0 = data & 0x03;
		}
		break;

	case 0x5103:
		{
			wram_protect1 = data & 0x03;
		}
		break;

	case 0x5104:
		{
			gfx_mode = data & 0x03;
		}
		break;

	case 0x5105:
		{
			for(i = 0; i < 4; i++)
			{
				//set_VRAM_bank(8+i, data & 0x03);
				mmc_VRAM_bank(8+i, data & 0x03);
				data >>= 2;
			}
		}
		break;

	case 0x5106:
		{
			//uint8* nametable3 = parent_NES->ppu->get_namt() + 0xC00;
			uint8* nametable3 = (uint8*)(mmc_getinfo()->PPU_nametables);
			for(i = 0; i < 0x3C0; i++)
			{
				nametable3[i|0xC00] = data;
			}
		}
		break;

	case 0x5107:
		{
			//uint8* nametable3 = parent_NES->ppu->get_namt() + 0xC00;
			uint8* nametable3 = (uint8*)(mmc_getinfo()->PPU_nametables);
			data &= 0x03;
			data = data | (data<<2) | (data<<4) | (data<<6);
			for(i = 0x3C0; i < 0x400; i++)
			{
				nametable3[i|0xC00] = data;
			}
		}
		break;

	case 0x5113:
		{
			MMC5_set_WRAM_bank(3, data & 0x07);
		}
		break;

	case 0x5114:
	case 0x5115:
	case 0x5116:
	case 0x5117:
		{
			MMC5_set_CPU_bank(addr & 0x07, data);
		}
		break;

	case 0x5120:
	case 0x5121:
	case 0x5122:
	case 0x5123:
	case 0x5124:
	case 0x5125:
	case 0x5126:
	case 0x5127:
		{
			chr_reg[addr & 0x07][0] = data;
			sync_Chr_banks(0);
		}
		break;

	case 0x5128:
	case 0x5129:
	case 0x512A:
	case 0x512B:
		{
			chr_reg[(addr & 0x03) + 0][1] = data;
			chr_reg[(addr & 0x03) + 4][1] = data;
		}
		break;

	case 0x5200:
		{
			split_control = data;
		}
		break;

	case 0x5201:
		{
			//split_scroll = data;
		}
		break;

	case 0x5202:
		{
			split_bank = data & 0x3F;
		}
		break;

	case 0x5203:
		{
			irq_line = data;
		}
		break;

	case 0x5204:
		{
			irq_enabled = data;
		}
		break;

	case 0x5205:
		{
			value0 = data;
		}
		break;

	case 0x5206:
		{
			value1 = data;
		}
		break;

	default:
		{
			if(addr >= 0x5000 && addr <= 0x5015)
			{
				//parent_NES->apu->ExWrite(addr, data);
				//exsound ex_write(addr,data);
			}
			else if(addr >= 0x5C00 && addr <= 0x5FFF)
			{
				if(gfx_mode != 3)
				{
					//uint8* nametable2 = parent_NES->ppu->get_namt() + 0x800;
					uint8* nametable2 = (uint8*)(mmc_getinfo()->PPU_nametables);
					
					nametable2[(addr & 0x3FF)|0x800] = data; //(irq_status & 0) ? data : 0x40;
				}
			}
		}
		break;
	}
}

static void map5_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(wram_protect0 == 0x02 && wram_protect1 == 0x01)
	{
		if(wb[3] != 8)
		{
			wram[wb[3]*0x2000+(addr&0x1FFF)] = data;
			//parent_NES->WriteSaveRAM(wb[3]*0x2000+(addr&0x1FFF), data);
			mmc_getinfo()->NESSRAM[wb[3]*0x2000+(addr&0x1FFF)]=data;
		}
	}
	//parent_NES->WriteSaveRAM(addr&0x1FFF, wram[addr&0x1FFF]);
	mmc_getinfo()->NESSRAM[addr&0x1FFF]=wram[addr&0x1FFF];
	
}

static void map5_MemoryWrite(uint32 addr, uint8 data)
{
	if(wram_protect0 == 0x02 && wram_protect1 == 0x01)
	{
		if(addr >= 0x8000 && addr <= 0x9FFF)
		{
			if(wb[4] != 8)
			{
				wram[wb[4]*0x2000+(addr&0x1FFF)] = data;
				//parent_NES->WriteSaveRAM(wb[4]*0x2000+(addr&0x1FFF), data);
				mmc_getinfo()->NESSRAM[wb[4]*0x2000+(addr&0x1FFF)]=data;
			}
		}
		else if(addr >= 0xA000 && addr <= 0xBFFF)
		{
			if(wb[5] != 8)
			{
				wram[wb[5]*0x2000+(addr&0x1FFF)] = data;
				//parent_NES->WriteSaveRAM(wb[5]*0x2000+(addr&0x1FFF), data);
				mmc_getinfo()->NESSRAM[wb[5]*0x2000+(addr&0x1FFF)]=data;
			}
		}
		else if(addr >= 0xC000 && addr <= 0xDFFF)
		{
			if(wb[6] != 8)
			{
				wram[wb[6]*0x2000+(addr&0x1FFF)] = data;
				//parent_NES->WriteSaveRAM(wb[6]*0x2000+(addr&0x1FFF), data);
				mmc_getinfo()->NESSRAM[wb[6]*0x2000+(addr&0x1FFF)]=data;
			}
		}
	}
}

static void map5_HSync(uint32 scanline)
{
	if(scanline <= 240)
	{
		if(scanline == irq_line)
		{
			//if(parent_NES->ppu->spr_enabled() && parent_NES->ppu->bg_enabled())
			if (ppu_bothenabled())
			{
				irq_status |= 0x80;
			}
		}
		if((irq_status & 0x80) && (irq_enabled & 0x80))
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
	}
	else
	{
		irq_status |= 0x40;
	}
}





// incomplete SNSS definition 
static void map5_setstate(SnssMapperBlock *state)
{
	irq_line = state->extraData.mapper5.irqLineLowByte;
	irq_line |= state->extraData.mapper5.irqLineHighByte << 8;
	irq_enabled = state->extraData.mapper5.irqEnabled;
	irq_status = state->extraData.mapper5.irqStatus;
	wram_protect0 = state->extraData.mapper5.wramWriteProtect0;
	wram_protect1 = state->extraData.mapper5.wramWriteProtect1;
	prg_size = state->extraData.mapper5.romBankSize;
	chr_size = state->extraData.mapper5.vromBankSize;
	gfx_mode = state->extraData.mapper5.gfxMode;
	split_control = state->extraData.mapper5.splitControl;
	split_bank = state->extraData.mapper5.splitBank;
	value0 = state->extraData.mapper5.last5205Write;
	value1 = state->extraData.mapper5.last5206Write;
	wb[3] = state->extraData.mapper5.wramBank3;
	wb[4] = state->extraData.mapper5.wramBank4;
	wb[5] = state->extraData.mapper5.wramBank5;
	wb[6] = state->extraData.mapper5.wramBank6;
	chr_reg[0][0] = state->extraData.mapper5.vromBank[0][0];
	chr_reg[1][0] = state->extraData.mapper5.vromBank[1][0];
	chr_reg[2][0] = state->extraData.mapper5.vromBank[2][0];
	chr_reg[3][0] = state->extraData.mapper5.vromBank[3][0];
	chr_reg[4][0] = state->extraData.mapper5.vromBank[4][0];
	chr_reg[5][0] = state->extraData.mapper5.vromBank[5][0];
	chr_reg[6][0] = state->extraData.mapper5.vromBank[6][0];
	chr_reg[7][0] = state->extraData.mapper5.vromBank[7][0];
	chr_reg[0][1] = state->extraData.mapper5.vromBank[0][1];
	chr_reg[1][1] = state->extraData.mapper5.vromBank[1][1];
	chr_reg[2][1] = state->extraData.mapper5.vromBank[2][1];
	chr_reg[3][1] = state->extraData.mapper5.vromBank[3][1];
	chr_reg[4][1] = state->extraData.mapper5.vromBank[4][1];
	chr_reg[5][1] = state->extraData.mapper5.vromBank[5][1];
	chr_reg[6][1] = state->extraData.mapper5.vromBank[6][1];
	chr_reg[7][1] = state->extraData.mapper5.vromBank[7][1];
}

static void map5_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper5.irqLineLowByte = irq_line & 0x00ff;
	state->extraData.mapper5.irqLineHighByte = (irq_line & 0xff00) >> 8;
	state->extraData.mapper5.irqEnabled = irq_enabled;
	state->extraData.mapper5.irqStatus = irq_status;
	state->extraData.mapper5.wramWriteProtect0 = wram_protect0;
	state->extraData.mapper5.wramWriteProtect1 = wram_protect1;
	state->extraData.mapper5.romBankSize = prg_size;
	state->extraData.mapper5.vromBankSize = chr_size;
	state->extraData.mapper5.gfxMode = gfx_mode;
	state->extraData.mapper5.splitControl = split_control;
	state->extraData.mapper5.splitBank = split_bank;
	state->extraData.mapper5.last5205Write = value0;
	state->extraData.mapper5.last5206Write = value1;
	state->extraData.mapper5.wramBank3 = wb[3];
	state->extraData.mapper5.wramBank4 = wb[4];
	state->extraData.mapper5.wramBank5 = wb[5];
	state->extraData.mapper5.wramBank6 = wb[6];
	state->extraData.mapper5.vromBank[0][0] = chr_reg[0][0];
	state->extraData.mapper5.vromBank[1][0] = chr_reg[1][0];
	state->extraData.mapper5.vromBank[2][0] = chr_reg[2][0];
	state->extraData.mapper5.vromBank[3][0] = chr_reg[3][0];
	state->extraData.mapper5.vromBank[4][0] = chr_reg[4][0];
	state->extraData.mapper5.vromBank[5][0] = chr_reg[5][0];
	state->extraData.mapper5.vromBank[6][0] = chr_reg[6][0];
	state->extraData.mapper5.vromBank[7][0] = chr_reg[7][0];
	state->extraData.mapper5.vromBank[0][1] = chr_reg[0][1];
	state->extraData.mapper5.vromBank[1][1] = chr_reg[1][1];
	state->extraData.mapper5.vromBank[2][1] = chr_reg[2][1];
	state->extraData.mapper5.vromBank[3][1] = chr_reg[3][1];
	state->extraData.mapper5.vromBank[4][1] = chr_reg[4][1];
	state->extraData.mapper5.vromBank[5][1] = chr_reg[5][1];
	state->extraData.mapper5.vromBank[6][1] = chr_reg[6][1];
	state->extraData.mapper5.vromBank[7][1] = chr_reg[7][1];
}

mapintf_t map5_intf =
{
   5, /* mapper number */
   "MMC5", /* mapper name */
   map5_Reset, /* init routine */
   NULL, /* vblank callback */
   map5_HSync, /* hblank callback */
   map5_getstate, /* get state (snss) */
   map5_setstate, /* set state (snss) */
   map5_MemoryReadLow,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map5_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   map5_MemoryWriteSaveRAM,  /*Write saveram*/
   map5_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   map5_PPU_Latch_RenderScreen,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/
};


/////////////////////////////////////////////////////////////////////
