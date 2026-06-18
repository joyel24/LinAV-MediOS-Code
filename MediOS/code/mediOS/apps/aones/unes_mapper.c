/*
  Little John GP32
  File : unes_mapper.c
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003
*/

#include "medios.h"

#include "unes.h"

#include "unes_mapper.h"

#include "mmclist.h"

#ifndef NES6502_NUMBANKS
#define  NES6502_NUMBANKS  8
#define  NES6502_BANKSHIFT 13
#define  NES6502_BANKSIZE  (0x10000 / NES6502_NUMBANKS)
#define  NES6502_BANKMASK  (NES6502_BANKSIZE - 1)
#endif



extern VirtualNES Vnes;

extern uint32 nametables_switched;
extern uint32 patterntables_switched;

#ifndef __asmcpu__
static nes6502_context _NEScontext;
#endif


uint32 num_8k_ROM_banks,num_16k_ROM_banks,num_32k_ROM_banks;
uint32 num_1k_VROM_banks,num_2k_VROM_banks,num_4k_VROM_banks,num_8k_VROM_banks;
uint32 VROM_mask1,VROM_mask2,VROM_mask4,VROM_mask8;
uint32 ROM_mask8,ROM_mask16,ROM_mask32;

#define MASK_BANK(bank,mask) (bank) = ((bank) & (mask))

#define VALIDATE_ROM_BANK8(bank) \
    MASK_BANK(bank,ROM_mask8); \
    if((bank) >= num_8k_ROM_banks) return;

#define VALIDATE_ROM_BANK16(bank) \
    MASK_BANK(bank,ROM_mask16); \
    if((bank) >= num_16k_ROM_banks) return;

#define VALIDATE_ROM_BANK32(bank) \
    MASK_BANK(bank,ROM_mask32); \
    if((bank) >= num_32k_ROM_banks) return;


#define VALIDATE_VROM_BANK1(bank) \
    MASK_BANK(bank,VROM_mask1); \
    if((bank) >= num_1k_VROM_banks) return;

#define VALIDATE_VROM_BANK2(bank) \
    MASK_BANK(bank,VROM_mask2); \
    if((bank) >= num_2k_VROM_banks) return;    
    
#define VALIDATE_VROM_BANK4(bank) \
    MASK_BANK(bank,VROM_mask4); \
    if((bank) >= num_4k_VROM_banks) return;    
    
#define VALIDATE_VROM_BANK8(bank) \
    MASK_BANK(bank,VROM_mask8); \
    if((bank) >= num_8k_VROM_banks) return;    
    


static mmc_t mmc;


VirtualNES *mmc_getinfo(void)
{
   return mmc.cart;
}

void mmc_setcontext(mmc_t *src_mmc)
{   
   mmc = *src_mmc;
}

void mmc_getcontext(mmc_t *dest_mmc)
{
   *dest_mmc = mmc;
}

/* VROM bankswitching */
void mmc_bankvrom(uint8 size, uint32 address, uint32 bank)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
         
   switch (size)
   {
   case 1:
      if (bank == MMC_LASTBANK)         
         bank = num_1k_VROM_banks-1;
         //bank = MMC_LAST1KVROM;
      VALIDATE_VROM_BANK1(bank);
      Vnes.PPUPageIndex[address >> 10] = (byte*)(Vnes.var.chr_beg + (bank << 10));
      Vnes.PPUPageIndexProtect[address>>10]=1;
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
	  return;

   case 2:
      if (bank == MMC_LASTBANK)         
         bank = num_2k_VROM_banks - 1 ;
         //bank = MMC_LAST2KVROM;
      VALIDATE_VROM_BANK2(bank);
      Vnes.PPUPageIndex[address >> 10] = (byte*)(Vnes.var.chr_beg + (bank << 11));
      Vnes.PPUPageIndex[(address >> 10)+1] = (byte*)(Vnes.PPUPageIndex[address >> 10] + 0x0400);
      Vnes.PPUPageIndexProtect[address>>10]=1;
      Vnes.PPUPageIndexProtect[(address>>10) + 1]=1;
/*lazy stuff*/
  patterntables_switched = 1;
/**/  
      return;

   case 4:
      if (bank == MMC_LASTBANK)
      bank = num_4k_VROM_banks-1;
         //bank = MMC_LAST4KVROM;         
      VALIDATE_VROM_BANK4(bank);
	  Vnes.PPUPageIndex[address >> 10] = (byte*)(Vnes.var.chr_beg + (bank << 12));
      Vnes.PPUPageIndex[(address >> 10)+1] = (byte*)(Vnes.PPUPageIndex[address >> 10] + 0x0400);
      Vnes.PPUPageIndex[(address >> 10)+2] = (byte*)(Vnes.PPUPageIndex[address >> 10] + 0x0800);
      Vnes.PPUPageIndex[(address >> 10)+3] = (byte*)(Vnes.PPUPageIndex[address >> 10] + 0x0C00);
      Vnes.PPUPageIndexProtect[(address>>10)]=1;
      Vnes.PPUPageIndexProtect[(address>>10)+1]=1;
      Vnes.PPUPageIndexProtect[(address>>10)+2]=1;
      Vnes.PPUPageIndexProtect[(address>>10)+3]=1;
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
      return;

   case 8:
      if (bank == MMC_LASTBANK)         
         bank = num_8k_VROM_banks - 1 ;
         //bank = MMC_LAST8KVROM;
      VALIDATE_VROM_BANK8(bank);
	  Vnes.PPUPageIndex[0] = (byte*)(Vnes.var.chr_beg + (bank << 13));      
	  Vnes.PPUPageIndex[1] = (byte*)(Vnes.PPUPageIndex[0] + 0x0400);
	  Vnes.PPUPageIndex[2] = (byte*)(Vnes.PPUPageIndex[0] + 0x0800);
	  Vnes.PPUPageIndex[3] = (byte*)(Vnes.PPUPageIndex[0] + 0x0C00);
	  Vnes.PPUPageIndex[4] = (byte*)(Vnes.PPUPageIndex[0] + 0x0400);
	  Vnes.PPUPageIndex[5] = (byte*)(Vnes.PPUPageIndex[0] + 0x1000);	  
	  Vnes.PPUPageIndex[6] = (byte*)(Vnes.PPUPageIndex[0] + 0x1800);
	  Vnes.PPUPageIndex[7] = (byte*)(Vnes.PPUPageIndex[0] + 0x1C00);
      Vnes.PPUPageIndexProtect[0]=1;
      Vnes.PPUPageIndexProtect[1]=1;
      Vnes.PPUPageIndexProtect[2]=1;
      Vnes.PPUPageIndexProtect[3]=1;
      Vnes.PPUPageIndexProtect[4]=1;
      Vnes.PPUPageIndexProtect[5]=1;
      Vnes.PPUPageIndexProtect[6]=1;
      Vnes.PPUPageIndexProtect[7]=1;
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
      return;  
   }
}

void mmc_bankvromALL8(uint32 bank0,uint32 bank1,uint32 bank2,uint32 bank3,uint32 bank4,uint32 bank5,uint32 bank6,uint32 bank7)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank0);
  VALIDATE_VROM_BANK1(bank1);
  VALIDATE_VROM_BANK1(bank2);
  VALIDATE_VROM_BANK1(bank3);
  VALIDATE_VROM_BANK1(bank4);
  VALIDATE_VROM_BANK1(bank5);
  VALIDATE_VROM_BANK1(bank6);
  VALIDATE_VROM_BANK1(bank7);

  Vnes.PPUPageIndex[0] = Vnes.var.chr_beg + (bank0 << 10);
  Vnes.PPUPageIndexProtect[0]=1;
  Vnes.PPUPageIndex[1] = Vnes.var.chr_beg + (bank1 << 10);
  Vnes.PPUPageIndexProtect[1]=1;
  Vnes.PPUPageIndex[2] = Vnes.var.chr_beg + (bank2 << 10);
  Vnes.PPUPageIndexProtect[2]=1;
  Vnes.PPUPageIndex[3] = Vnes.var.chr_beg + (bank3 << 10);
  Vnes.PPUPageIndexProtect[3]=1;
  Vnes.PPUPageIndex[4] = Vnes.var.chr_beg + (bank4 << 10);
  Vnes.PPUPageIndexProtect[4]=1;
  Vnes.PPUPageIndex[5] = Vnes.var.chr_beg + (bank5 << 10);
  Vnes.PPUPageIndexProtect[5]=1;
  Vnes.PPUPageIndex[6] = Vnes.var.chr_beg + (bank6 << 10);
  Vnes.PPUPageIndexProtect[6]=1;
  Vnes.PPUPageIndex[7] = Vnes.var.chr_beg + (bank7 << 10);
  Vnes.PPUPageIndexProtect[7]=1;
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
}

void mmc_bankvrom0(uint32 bank0)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank0);
  Vnes.PPUPageIndex[0] = Vnes.var.chr_beg + (bank0 << 10);
  Vnes.PPUPageIndexProtect[0]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
}

void mmc_bankvrom1(uint32 bank1)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank1);
  Vnes.PPUPageIndex[1] = Vnes.var.chr_beg + (bank1 << 10);
  Vnes.PPUPageIndexProtect[1]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
}

void mmc_bankvrom2(uint32 bank2)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank2);
  Vnes.PPUPageIndex[2] = Vnes.var.chr_beg + (bank2 << 10);
  Vnes.PPUPageIndexProtect[2]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/    
}

void mmc_bankvrom3(uint32 bank3)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank3);
  Vnes.PPUPageIndex[3] = Vnes.var.chr_beg + (bank3 << 10);
  Vnes.PPUPageIndexProtect[3]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/  
}

void mmc_bankvrom4(uint32 bank4)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank4);
  Vnes.PPUPageIndex[4] = Vnes.var.chr_beg + (bank4 << 10);
  Vnes.PPUPageIndexProtect[4]=1; 
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/     
}

void mmc_bankvrom5(uint32 bank5)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank5);
  Vnes.PPUPageIndex[5] = Vnes.var.chr_beg + (bank5 << 10);
  Vnes.PPUPageIndexProtect[5]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/    
}

void mmc_bankvrom6(uint32 bank6)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank6);
  Vnes.PPUPageIndex[6] = Vnes.var.chr_beg + (bank6 << 10);
  Vnes.PPUPageIndexProtect[6]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/    
}

void mmc_bankvrom7(uint32 bank7)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank7);
  Vnes.PPUPageIndex[7] = Vnes.var.chr_beg + (bank7 << 10);
  Vnes.PPUPageIndexProtect[7]=1;  
/*lazy stuff*/ 
  patterntables_switched = 1;
/**/    
}

void mmc_bankvrom8(uint32 bank8)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank8);
  Vnes.PPUPageIndex[8] = Vnes.var.chr_beg + (bank8 << 10);
  Vnes.PPUPageIndexProtect[8]=1;  
/*lazy stuff*/ 
  nametables_switched = 1;
/**/    
}

void mmc_bankvrom9(uint32 bank9)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank9);
  Vnes.PPUPageIndex[9] = Vnes.var.chr_beg + (bank9 << 10);
  Vnes.PPUPageIndexProtect[9]=1;  
/*lazy stuff*/ 
  nametables_switched = 1;
/**/    
}

void mmc_bankvrom10(uint32 bank10)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank10);
  Vnes.PPUPageIndex[10] = Vnes.var.chr_beg + (bank10 << 10);
  Vnes.PPUPageIndexProtect[10]=1;  
/*lazy stuff*/ 
  nametables_switched = 1;
/**/      
}

void mmc_bankvrom11(uint32 bank11)
{
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
      return;
  VALIDATE_VROM_BANK1(bank11);
  Vnes.PPUPageIndex[11] = Vnes.var.chr_beg + (bank11 << 10);
  Vnes.PPUPageIndexProtect[11]=1;  
/*lazy stuff*/ 
  nametables_switched = 1;
/**/      
}




/* ROM bankswitching */
void mmc_bankrom(uint8 size, uint32 address, uint32 bank)
{      
/*char ch[256];
sprintf(ch,"s%d a%d b%d",size,address,bank);
DEBUGS(ch);*/
   switch (size)
   {
   case 8:
      if (bank == MMC_LASTBANK)         
         bank = (num_8k_ROM_banks-1);
         //bank = MMC_LAST8KROM;
         VALIDATE_ROM_BANK8(bank);
      {
         int page = address >> NES6502_BANKSHIFT;
         Vnes.CPUPageIndex[page] = Vnes.var.prg_beg + (bank << 13);
      }

      break;

   case 16:
      if (bank == MMC_LASTBANK)         
         bank = num_16k_ROM_banks- 1;
         //bank = MMC_LAST16KROM;
         VALIDATE_ROM_BANK16(bank);
      {
         int page = address >> NES6502_BANKSHIFT;
         Vnes.CPUPageIndex[page] = Vnes.var.prg_beg + (bank << 14);
         Vnes.CPUPageIndex[page + 1] = Vnes.CPUPageIndex[page] + 0x2000;      
      }
      break;

   case 32:
      if (bank == MMC_LASTBANK)         
         bank = (num_32k_ROM_banks - 1);
         //bank = MMC_LAST32KROM;
	VALIDATE_ROM_BANK32(bank);
      Vnes.CPUPageIndex[4] = Vnes.var.prg_beg + (bank << 15);
      Vnes.CPUPageIndex[5] = Vnes.CPUPageIndex[4] + 0x2000;      
      Vnes.CPUPageIndex[6] = Vnes.CPUPageIndex[4] + 0x4000;      
      Vnes.CPUPageIndex[7] = Vnes.CPUPageIndex[4] + 0x6000;
      
      break;

   default:
      //log_printf("invalid ROM bank size %d\n", size);
      break;
   }
#ifdef __asmcpu__
//  DrawMessage("call to set_cpu_bank_full",0);     	
   set_cpu_bank_full(Vnes.CPUPageIndex[4],Vnes.CPUPageIndex[5],Vnes.CPUPageIndex[6],Vnes.CPUPageIndex[7]); 
  //DrawMessage("ok",0);
   /*set_cpu_bank0(Vnes.CPUPageIndex[4]);
   set_cpu_bank1(Vnes.CPUPageIndex[5]);
   set_cpu_bank2(Vnes.CPUPageIndex[6]);
   set_cpu_bank3(Vnes.CPUPageIndex[7]);*/
#else
   nes6502_getcontext(&_NEScontext);
  /*_NEScontext.mem_page[0]=(uint8 *)Vnes.CPUPageIndex[0];
  _NEScontext.mem_page[1]=(uint8 *)Vnes.CPUPageIndex[1];
  _NEScontext.mem_page[2]=(uint8 *)Vnes.CPUPageIndex[2];
  _NEScontext.mem_page[3]=(uint8 *)Vnes.CPUPageIndex[3];*/
  _NEScontext.mem_page[4]=(uint8 *)Vnes.CPUPageIndex[4];
  _NEScontext.mem_page[5]=(uint8 *)Vnes.CPUPageIndex[5];
  _NEScontext.mem_page[6]=(uint8 *)Vnes.CPUPageIndex[6];
  _NEScontext.mem_page[7]=(uint8 *)Vnes.CPUPageIndex[7];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankromALL4(uint32 bank4,uint32 bank5,uint32 bank6,uint32 bank7)
{       
    VALIDATE_ROM_BANK8(bank4);
    VALIDATE_ROM_BANK8(bank5);
    VALIDATE_ROM_BANK8(bank6);
    VALIDATE_ROM_BANK8(bank7);
    
    Vnes.CPUPageIndex[4] = Vnes.var.prg_beg + (bank4 << 13);
    Vnes.CPUPageIndex[5] = Vnes.var.prg_beg + (bank5 << 13);
    Vnes.CPUPageIndex[6] = Vnes.var.prg_beg + (bank6 << 13);
    Vnes.CPUPageIndex[7] = Vnes.var.prg_beg + (bank7 << 13);      
#ifdef __asmcpu__
   set_cpu_bank_full(Vnes.CPUPageIndex[4],Vnes.CPUPageIndex[5],Vnes.CPUPageIndex[6],Vnes.CPUPageIndex[7]);   
#else
   nes6502_getcontext(&_NEScontext);
  /*_NEScontext.mem_page[0]=(uint8 *)Vnes.CPUPageIndex[0];
  _NEScontext.mem_page[1]=(uint8 *)Vnes.CPUPageIndex[1];
  _NEScontext.mem_page[2]=(uint8 *)Vnes.CPUPageIndex[2];
  _NEScontext.mem_page[3]=(uint8 *)Vnes.CPUPageIndex[3];*/
  _NEScontext.mem_page[4]=(uint8 *)Vnes.CPUPageIndex[4];
  _NEScontext.mem_page[5]=(uint8 *)Vnes.CPUPageIndex[5];
  _NEScontext.mem_page[6]=(uint8 *)Vnes.CPUPageIndex[6];
  _NEScontext.mem_page[7]=(uint8 *)Vnes.CPUPageIndex[7];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankromALL5(uint32 bank3,uint32 bank4,uint32 bank5,uint32 bank6,uint32 bank7)
{       
    VALIDATE_ROM_BANK8(bank3);
    VALIDATE_ROM_BANK8(bank4);
    VALIDATE_ROM_BANK8(bank5);
    VALIDATE_ROM_BANK8(bank6);
    VALIDATE_ROM_BANK8(bank7);
    
    Vnes.CPUPageIndex[3] = Vnes.var.prg_beg + (bank3 << 13);
    Vnes.CPUPageIndex[4] = Vnes.var.prg_beg + (bank4 << 13);
    Vnes.CPUPageIndex[5] = Vnes.var.prg_beg + (bank5 << 13);
    Vnes.CPUPageIndex[6] = Vnes.var.prg_beg + (bank6 << 13);
    Vnes.CPUPageIndex[7] = Vnes.var.prg_beg + (bank7 << 13);      
#ifdef __asmcpu__
   set_cpu_bank_full(Vnes.CPUPageIndex[4],Vnes.CPUPageIndex[5],Vnes.CPUPageIndex[6],Vnes.CPUPageIndex[7]);   
   init_sram(Vnes.CPUPageIndex[3]);
#else
   nes6502_getcontext(&_NEScontext);
  /*_NEScontext.mem_page[0]=(uint8 *)Vnes.CPUPageIndex[0];
  _NEScontext.mem_page[1]=(uint8 *)Vnes.CPUPageIndex[1];
  _NEScontext.mem_page[2]=(uint8 *)Vnes.CPUPageIndex[2];*/
  _NEScontext.mem_page[3]=(uint8 *)Vnes.CPUPageIndex[3];
  _NEScontext.mem_page[4]=(uint8 *)Vnes.CPUPageIndex[4];
  _NEScontext.mem_page[5]=(uint8 *)Vnes.CPUPageIndex[5];
  _NEScontext.mem_page[6]=(uint8 *)Vnes.CPUPageIndex[6];
  _NEScontext.mem_page[7]=(uint8 *)Vnes.CPUPageIndex[7];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankrom3(uint32 bank3)
{       
    VALIDATE_ROM_BANK8(bank3);
    
    Vnes.CPUPageIndex[3] = Vnes.var.prg_beg + (bank3 << 13);    
#ifdef __asmcpu__   
   init_sram(Vnes.CPUPageIndex[3]);
   reload_fast_pc();
#else
   nes6502_getcontext(&_NEScontext);
  /*_NEScontext.mem_page[0]=(uint8 *)Vnes.CPUPageIndex[0];
  _NEScontext.mem_page[1]=(uint8 *)Vnes.CPUPageIndex[1];
  _NEScontext.mem_page[2]=(uint8 *)Vnes.CPUPageIndex[2];*/
  _NEScontext.mem_page[3]=(uint8 *)Vnes.CPUPageIndex[3];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankrom4(uint32 bank4)
{       
    VALIDATE_ROM_BANK8(bank4);
    
    Vnes.CPUPageIndex[4] = Vnes.var.prg_beg + (bank4 << 13);
#ifdef __asmcpu__   

   set_cpu_bank0(Vnes.CPUPageIndex[4]);
#else
   nes6502_getcontext(&_NEScontext);  
  _NEScontext.mem_page[4]=(uint8 *)Vnes.CPUPageIndex[4];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankrom5(uint32 bank5)
{       
    VALIDATE_ROM_BANK8(bank5);
    
    Vnes.CPUPageIndex[5] = Vnes.var.prg_beg + (bank5 << 13);
#ifdef __asmcpu__   
   set_cpu_bank1(Vnes.CPUPageIndex[5]);
#else
   nes6502_getcontext(&_NEScontext);  
  _NEScontext.mem_page[5]=(uint8 *)Vnes.CPUPageIndex[5];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankrom6(uint32 bank6)
{       
    VALIDATE_ROM_BANK8(bank6);
    
    Vnes.CPUPageIndex[6] = Vnes.var.prg_beg + (bank6 << 13);
#ifdef __asmcpu__   
   set_cpu_bank2(Vnes.CPUPageIndex[6]);
#else
   nes6502_getcontext(&_NEScontext);  
  _NEScontext.mem_page[6]=(uint8 *)Vnes.CPUPageIndex[6];  
  nes6502_setcontext(&_NEScontext);
#endif
}

void mmc_bankrom7(uint32 bank7)
{       
    VALIDATE_ROM_BANK8(bank7);
    
    Vnes.CPUPageIndex[7] = Vnes.var.prg_beg + (bank7 << 13);
#ifdef __asmcpu__   
   set_cpu_bank3(Vnes.CPUPageIndex[7]);
#else
   nes6502_getcontext(&_NEScontext);  
  _NEScontext.mem_page[7]=(uint8 *)Vnes.CPUPageIndex[7];  
  nes6502_setcontext(&_NEScontext);
#endif
}


/* Check to see if this mapper is supported */
bool mmc_peek(uint32 map_num)
{
   mapintf_t **map_ptr = mappers;

   while (NULL != *map_ptr)
   {
      if ((*map_ptr)->number == map_num)
         return true;
      map_ptr++;
   }

   return false;
}

void mmc_setpages(void)
{

   /* Switch ROM into CPU space, set VROM/VRAM (done for ALL ROMs) */

   mmc_bankrom(16, 0x8000, 0);

   mmc_bankrom(16, 0xC000, MMC_LASTBANK);

   mmc_bankvrom(8, 0x0000, 0);         

   

   /* if we have no VROM, switch in VRAM */
   /* TODO: fix this hack implementation */
   if (0 == Vnes.ROM_Header.chr_rom_pages_nb)
   {      
      /*for (int i=0;i<8;i++) 
   	  {	  
	  	Vnes.PPUPageIndexProtect[i]=0;
   	  }
      ppu_setpage(8, 0, Vnes.PPU_patterntables);*/
   } 
//   InitMirroring();   
}

void map2_Reset(void);

/* Mapper initialization routine */
void mmc_reset(void)
{

   mmc_setpages();   

   if (mmc.intf->Reset)
   {

      mmc.intf->Reset();              
    }
}


void mmc_destroy(mmc_t **nes_mmc)
{
   if (*nes_mmc)
      free(*nes_mmc);
}

mmc_t *mmc_create()
{
   mmc_t *temp;
   mapintf_t **map_ptr;
   uint32 probe;
  
   for (map_ptr = mappers; (*map_ptr)->number != Vnes.Mapper_used; map_ptr++)
   {
      if (NULL == *map_ptr)      
         return NULL; /* Should *never* happen */
   }

   temp = (mmc_t*)malloc(sizeof(mmc_t));
   if (NULL == temp)
      return NULL;

   memset(temp, 0, sizeof(mmc_t));

   temp->intf = *map_ptr;
   temp->cart = &Vnes;

   mmc_setcontext(temp);
   
   num_8k_ROM_banks = Vnes.ROM_Header.prg_rom_pages_nb*2;
   num_16k_ROM_banks = Vnes.ROM_Header.prg_rom_pages_nb;
   num_32k_ROM_banks = Vnes.ROM_Header.prg_rom_pages_nb/2;
   
   num_1k_VROM_banks = Vnes.ROM_Header.chr_rom_pages_nb*8;   
   num_2k_VROM_banks = Vnes.ROM_Header.chr_rom_pages_nb*4;
   num_4k_VROM_banks = Vnes.ROM_Header.chr_rom_pages_nb*2;   
   num_8k_VROM_banks = Vnes.ROM_Header.chr_rom_pages_nb;
   
   ROM_mask8  = 0xFFFF;
   VROM_mask1 = 0xFFFF;

   for(probe = 0x8000; probe; probe >>= 1)
   {
	if((num_8k_ROM_banks-1) & probe) break;
	ROM_mask8 >>= 1;
   }
   for(probe = 0x8000; probe; probe >>= 1)
   {
 	if((num_1k_VROM_banks-1) & probe) break;
	VROM_mask1 >>= 1;
   }
   
   ROM_mask16 = ROM_mask8>>1;
   ROM_mask32 = ROM_mask8>>2;
   
   VROM_mask2 = VROM_mask1>>1;
   VROM_mask4 = VROM_mask1>>2;
   VROM_mask8 = VROM_mask1>>3;
   

   return temp;
}


void mmc_VRAM_bank(uint8 page,uint32 bank)
{
   if (page<8)
   { 
   	Vnes.PPUPageIndex[page]= Vnes.PPU_patterntables + ((bank&0x0f)<<10);   
   	Vnes.PPUPageIndexProtect[page]=0;
/*lazy stuff*/ 
    patterntables_switched = 1;
/**/       	
   }
   else
   { 
   	Vnes.PPUPageIndex[page]= Vnes.PPU_nametables + ((bank&0x03)<<10);
   	Vnes.PPUPageIndexProtect[page]=0;
/*lazy stuff*/ 
    nametables_switched = 1;
/**/    
   }
}

