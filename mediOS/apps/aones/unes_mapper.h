/*
  Little John GP32
  File : unes_mapper.h
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003
*/
#ifndef __unes_mapper_h__
#define __unes_mapper_h__

#include "datatypes.h"
#include "snss.h"
#include "unes.h"

//int InitMapper();

/*typedef struct
{
  void (*Mapper_UpdateHBlank)(bool vblank);
  void (*Mapper_UpdateVBlank)(void);
  void (*Mapper_Access)(word Addr,byte Value);
  void (*Mapper_PPU_Latch)(word Addr,byte Value);
  uint32 rom_bank;
  uint8 flags;
} Mapper_Info;*/


/*********************************************************/
/*This comes directly from Nofrendo !!*/
/*********************************************************/
#define  MMC_LASTBANK      -1

typedef struct
{
   uint32 min_range, max_range;
   uint8 (*read_func)(uint32 address);
} map_memread;

typedef struct
{
   uint32 min_range, max_range;
   void (*write_func)(uint32 address, uint8 value);
} map_memwrite;

typedef struct mapintf_s
{
   uint32 number;
   char *name;      
   void  (*Reset)(void);
   void  (*VSync)(void);
   void  (*HSync)(uint32 scanline);   
   void  (*get_state)(SnssMapperBlock *state);
   void  (*set_state)(SnssMapperBlock *state);   
   uint8 (*MemoryReadLow)(uint32 address);
   uint8 (*MemoryReadSaveRAM)(uint32 addr);
   void  (*WriteHighRegs)(uint32 addr, uint8 data);
   void  (*MemoryWriteLow)(uint32 addr, uint8 data);
   void  (*MemoryWriteSaveRAM)(uint32 addr, uint8 data);
   void  (*MemoryWrite)(uint32 addr, uint8 data);      
      

	// for mmc2 & mmc5 & Oekakidds
   void  (*PPU_Latch_FDFE)(uint32 addr);
   uint8 (*PPU_Latch_RenderScreen)(uint8 mode, uint32 addr);
   void  (*PPU_Latch_Address)(uint32 addr);
   void (*SetBarcodeValue)(uint32 value_low, uint32 value_high);
   
} mapintf_t;

typedef struct mmc_s
{
   mapintf_t *intf;
   VirtualNES      *cart;  /* link it back to the cart */   
} mmc_t;

VirtualNES *mmc_getinfo(void);

void mmc_bankvrom(uint8 size, uint32 address, uint32 bank);
void mmc_bankvromALL8(uint32 bank0,uint32 bank1,uint32 bank2,uint32 bank3,uint32 bank4,uint32 bank5,uint32 bank6,uint32 bank7);
void mmc_bankvrom0(uint32 bank0);
void mmc_bankvrom1(uint32 bank1);
void mmc_bankvrom2(uint32 bank2);
void mmc_bankvrom3(uint32 bank3);
void mmc_bankvrom4(uint32 bank4);
void mmc_bankvrom5(uint32 bank5);
void mmc_bankvrom6(uint32 bank6);
void mmc_bankvrom7(uint32 bank7);
void mmc_bankvrom8(uint32 bank8);
void mmc_bankvrom9(uint32 bank9);
void mmc_bankvrom10(uint32 bank10);
void mmc_bankvrom11(uint32 bank11);

void mmc_bankrom(uint8 size, uint32 address, uint32 bank);
void mmc_bankromALL4(uint32 bank4,uint32 bank5,uint32 bank6,uint32 bank7);
void mmc_bankromALL5(uint32 bank3,uint32 bank4,uint32 bank5,uint32 bank6,uint32 bank7);
void mmc_bankrom3(uint32 bank3);
void mmc_bankrom4(uint32 bank4);
void mmc_bankrom5(uint32 bank5);
void mmc_bankrom6(uint32 bank6);
void mmc_bankrom7(uint32 bank7);
void mmc_VRAM_bank(uint8 page,uint32 bank);

mmc_t *mmc_create(void);
void mmc_destroy(mmc_t **nes_mmc);
void mmc_reset(void);


/*********************************************************/

#endif

