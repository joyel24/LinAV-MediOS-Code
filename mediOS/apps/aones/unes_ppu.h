/*
  Little John GP32
  File : unes_ppu.h
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003
*/

#ifndef __unes_ppu_h__
#define __unes_ppu_h__


#include "snss.h"



void ppu_init(void);
void ppu_deinit(void);
void ppu_reset(bool hardreset);

void ppu_dma(uint8 Value);

uint32 ppu_currentscanline(void);
bool ppu_vbl(void);
bool ppu_enabled(void);
bool ppu_bothenabled(void);

uint8 ppu_readreg(uint32 Addr);
void ppu_writereg(uint32 Addr,uint8 Value);

void ppu_scanlinePAL(void);
void ppu_scanlineNTSC(void);

void ppu_lazy_scanlinePAL(void);
void ppu_lazy_scanlineNTSC(void);

void ppu_savestate(SnssBaseBlock *baseBlock);
void ppu_loadstate(SnssBaseBlock *baseBlock);


void InitMirroring(void);



void ppu_setscaleMode(uint8 mode);

void ppu_mirror(uint32 nt0, uint32 nt1, uint32 nt2, uint32 nt3);


/* some mappers do *dumb* things */
typedef void (*PPU_Latch_FDFE_t)(uint32 address);
typedef void (*PPU_Latch_Address_t)(uint32 address);
typedef uint8 (*PPU_Latch_RenderScreen_t)(uint8 value,uint32 address);

/*xlat func*/
typedef void (*xlatfunc_t)(unsigned char *src, unsigned char *dest, uint8 *nesppupal,uint32 scanl);
typedef void (*xlatfuncLazy_t)(unsigned char *src, unsigned char *dest, uint8 *nesppupal,uint32 startscanl);

uint8 ppu_noLatchFunc(void);
void ppu_set_Latch_FDFE(PPU_Latch_FDFE_t func);
void ppu_set_Latch_Address(PPU_Latch_Address_t func);
void ppu_set_Latch_RenderScreen(PPU_Latch_RenderScreen_t func);


#endif
