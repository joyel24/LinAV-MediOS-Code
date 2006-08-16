/***********************************************************
  Little John GP32, a NES emulator.
  File : unes_ppu.c
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003


  Heavily powered by shatbox from Matthew Conte  
************************************************************/

#include "medios.h"
#include "aones.h"


#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"
#include "unes_io.h"

#include "nesregs.h"

extern void ppu_cacheOAMa(uint32 PPUPageIndex,uint32 maxobj);
extern void ppu_renderOAMLinea(uint32 buffer);
//extern void ppu_renderBGLinea(uint32 buffer,uint32 PPUPageIndex);

extern uint16 *lj_curRenderingScreenPtr;

extern VirtualNES Vnes;
extern int nflip;                   // Flip Index

//extern Mapper_Func mapfunc;



__IRAM_DATA PPU_Latch_FDFE_t ppu_Latch_FDFE;
__IRAM_DATA PPU_Latch_Address_t ppu_Latch_Address;
__IRAM_DATA PPU_Latch_RenderScreen_t ppu_Latch_RenderScreen;

#define CHECK_MMC2(addr) \
  if( (addr & 0x0FC0) == 0x0FC0) \
  { \
    if( ((addr & 0x0FF0) == 0x0FD0) || ((addr & 0x0FF0) == 0x0FE0) ) \
    { \
      if (ppu_Latch_FDFE) ppu_Latch_FDFE(addr); \
    } \
  }


__IRAM_DATA uint8 ppuRegs[8];
__IRAM_DATA uint32 ppu_sprite0;
__IRAM_DATA uint32 ppu_StrikeCycle;
__IRAM_DATA uint32 ppu_lastStrikeScanline;

__IRAM_DATA uint32   ppuVAddr;
__IRAM_DATA uint32   ppuVAddrLatch;
__IRAM_DATA uint32   ppuTileXOfs;
__IRAM_DATA uint32   ppuFlipFlop;
__IRAM_DATA uint32   lazy_bgnoclip;
__IRAM_DATA uint32   lazy_objnoclip;

__IRAM_DATA uint32 bg_pattern_table_addr;
__IRAM_DATA uint32 spr_pattern_table_addr;
__IRAM_DATA uint32 ppu_addr_inc;
__IRAM_DATA uint32 obj_height;

//uint8    ppuLatch;
__IRAM_DATA uint8    ppuVDataLatch;

__IRAM_DATA uint32   ppuScanline;

uint8 patternTOpix[65536*4*8];


// lazy updating stuff
__IRAM_DATA uint32 patterntables_switched;
__IRAM_DATA uint32 nametables_switched;
__IRAM_DATA uint8* lazy_PPUPageIndex[12];

__IRAM_DATA uint32 lazy_bg_start_line, lazy_spr_start_line;
__IRAM_DATA uint32 lazy_bg_pattern_table_addr;
__IRAM_DATA uint32 lazy_spr_pattern_table_addr;
__IRAM_DATA uint32 lazy_ppuVAddr, lazy_ppuVAddrLatch;
__IRAM_DATA uint32  lazy_ppuTileXOfs;
__IRAM_DATA uint32  lazy_bg_enabled, lazy_spr_enabled;

__IRAM_DATA uint32  lazy_bg_need_update, lazy_spr_need_update;
__IRAM_DATA uint8  lazy_pal[32];


/******************************************************************************/
/*  OAM */
/******************************************************************************/

#define OAM_SIZE 256
#define OAM_DMA_CYCLES 512 // cpu cycles per dma
//#define MAX_CACHED 8

#define  MAX_OBJ        64
// oam attribute byte bitmasks
#define  OAMF_VFLIP     0x80
#define  OAMF_HFLIP     0x40
#define  OAMF_BEHIND    0x20

// we use this flag for evil, not good.
#define  OAMF_SPRITE_0  0x10

typedef struct
{
   uint8    y;
   uint8    tile;
   uint8    attrib;
   uint8    x;
}  obj_t;

typedef struct
{
   uint8    attrib;  // 3 bits from hardware, plus sprite 0 flag
   uint8    x;
   uint8    pat0;
   uint8    pat1;
} oamtemp_t;

typedef union
   {
      uint8 memory[OAM_SIZE];
      obj_t obj[MAX_OBJ];
   } oam_t;



__IRAM_DATA oamtemp_t   oamCache[MAX_OBJ]; //CACHED];
__IRAM_DATA oam_t       oamMemory;
__IRAM_DATA uint32         oamNumCached;

/**LAZY stuff ***/
__IRAM_DATA oam_t       lazy_oamMemory;

/******************************************************************************/

__IRAM_CODE void oam_dma(uint8 value)
{
   // a cpu write to the ppu registers triggers oam dma.
   // since oam dma steals cycles away from the cpu, we must
   // release the cpu's current timeslice.   
   //uint32 cpuAddress = (uint32) value * 256;
   uint8 *cpuAddress;
   uint32 cpuaddr;
   uint8 addr;

#ifndef  __asmcpu__
   nes6502_release();
   nes6502_burn(OAM_DMA_CYCLES);
#else
   cpu_release();
   cpu_burn(OAM_DMA_CYCLES);
#endif   


   // oam dma starts at current oam address
   if (value<0x20) value&=0x07;
   
   cpuaddr =  (uint32) value << 8;
   
   if ((cpuaddr<0x2000)||(cpuaddr>=0x6000))
   {    
        addr=0;
        for (;;)
        {
           oamMemory.memory[addr++] = Rd6502(cpuaddr++);
           if (!addr) break;
        }

   }
   else
   {
    cpuAddress = (uint8 *)(&Vnes.CPUPageIndex[cpuaddr>>13][cpuaddr&0x1FFF]);
    memcpy(oamMemory.memory,cpuAddress,0x100);       
   }
}


/******************************************************************************/

uint32 ppu_currentscanline(void)
{
    return ppuScanline;
}

bool ppu_vbl(void)
{
    return (ppuRegs[REG_CTRL0] & REGF_CTRL0_NMI);
}

bool ppu_enabled(void)
{
    return (ppuRegs[REG_CTRL1] & (REGF_CTRL1_BGON | REGF_CTRL1_OBJON));
}

bool ppu_bothenabled(void)
{
    return ((ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON) && (ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJON));
}

bool ppu_vramInaccessible(void)
{
   return ((ppuRegs[REG_CTRL1] & (REGF_CTRL1_BGON | REGF_CTRL1_OBJON)) && ppuScanline < 240);
   //ppu_enabled and not in vblank
}

__IRAM_CODE void ppu_dma(uint8 Value)
{
    //oam_dma(ppuRegs[REG_OAMADDR],Value);
    oam_dma(Value);
/*lazy stuff*/
    lazy_spr_need_update = 1;
/**/
}



__IRAM_CODE uint8 ppu_readreg(uint32 Addr)
{
  uint8 value;
  switch (Addr&7)
  {
     case REG_STAT:
    // only top 3 bits readable     
    #ifndef __asmcpu__
    if (ppu_sprite0 && nes6502_getcycles(false) >= ppu_StrikeCycle)
    #else
    //TODO : more precision
    if (ppu_sprite0)
    #endif
    {
      ppu_sprite0=0;
      ppuRegs[REG_STAT] |=REGF_STAT_STRIKE;        
        }
        //value = (ppuRegs[REG_STAT] & 0xe0) | (ppuLatch & 0x1f);
        value = ppuRegs[REG_STAT];
        
        // acknowledge vbl interrupt and reset flip flop
        ppuRegs[REG_STAT] &= 0x7F; //~REGF_STAT_VBLINT;
        ppuFlipFlop = 0;            
    return(value);  
     case REG_VDATA:
     {
        uint32 address=ppuVAddr&0x3FFF;
        // buffered vram reads
        ppuVAddr += ppu_addr_inc;
        ppuVAddr &= 0xffff;
        
        if (address> 0x3f00)
        {
           return Vnes.PPU_palette[address&0x1F];
        }
        
        value = ppuVDataLatch;
        if (address>=0x3000) address&=0xEFFF;
        ppuVDataLatch = Vnes.PPUPageIndex[address>>10][address&0x3ff];                                              
        return value;       
     }
   }
     /*case REG_OAMDATA:
     case REG_CTRL0:
     case REG_CTRL1:
     case REG_OAMADDR:
     case REG_SCROLL:
     case REG_VADDR:
     default:*/
  return ppuRegs[Addr&7];
        //value = ppuLatch;        
        //return value;     
}

__IRAM_CODE void ppu_writereg(uint32 Addr,uint8 Value)
{
// writes are always latched
   //ppuLatch = Value;
   ppuRegs[Addr&7] = Value;

   switch (Addr & 7)
   {
     case REG_CTRL0:
      //ppuRegs[REG_CTRL0] = Value;
     {
/*lazy stuff*/     
      uint32 ppuVAddrLatch_old=ppuVAddrLatch;
/**/      
      bg_pattern_table_addr  = (Value & 0x10) ? 0x1000 : 0x0000;
      spr_pattern_table_addr = (Value & 0x08) ? 0x1000 : 0x0000;
      obj_height = (ppuRegs[REG_CTRL0] & REGF_CTRL0_OBJ16) ? 16 : 8;
      ppu_addr_inc = (Value & 0x04) ? 32 : 1;
      ppuVAddrLatch = (ppuVAddrLatch&0xF3FF)|(((uint16)(Value & 3)) << 10);
      
/*lazy stuff*/
      if (bg_pattern_table_addr != lazy_bg_pattern_table_addr || ppuVAddrLatch != ppuVAddrLatch_old) 
      {
          lazy_bg_need_update = 1;
      }
      if (spr_pattern_table_addr != lazy_spr_pattern_table_addr) 
      {
          lazy_spr_need_update = 1;
      }
/**/      
      return;
     }
     case REG_CTRL1:      
      //TODO yoyo
      //do some special stuff for rgb and so...
      
/*lazy stuff*/
      if ((ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON)!=lazy_bg_enabled) lazy_bg_need_update=1;
      if ((ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJON)!=lazy_spr_enabled) lazy_spr_need_update=1;
/**/      
      return;
/*     case REG_STAT:
      // read-only
      return;
     case REG_OAMADDR:
      return;*/
     case REG_OAMDATA:                 
/*lazy stuff*/     
      {
       uint32 addr_spr = ppuRegs[REG_OAMADDR]&0xFC;
       //access to 'y' attribute
       if (lazy_spr_enabled && (ppuScanline<240) )
       //sprite enabled and not in vblank
         if ( (oamMemory.memory[addr_spr]+obj_height>=lazy_spr_start_line) && (oamMemory.memory[addr_spr]<ppuScanline) )
         // sprite has to be draw next lazy_update
            lazy_spr_need_update = 1;
      }
/**/
      oamMemory.memory[ppuRegs[REG_OAMADDR]++]=Value;            
      return;
     case REG_SCROLL:
      ppuFlipFlop ^= 1;
      if (ppuFlipFlop)
      {
/*lazy stuff*/      
        uint32 ppuVAddrLatch_old = ppuVAddrLatch;
        uint32 ppuTileXOfs_old = ppuTileXOfs;
/**/        
        // x=d:00000111
        ppuTileXOfs = Value & 7;    
         // t:0000000000011111=d:11111000
        ppuVAddrLatch = (ppuVAddrLatch&0xFFE0)|(((uint16)(Value&0xF8)) >> 3);

/*lazy stuff*/         
        if (ppuVAddrLatch != ppuVAddrLatch_old || ppuTileXOfs != ppuTileXOfs_old)
          lazy_bg_need_update = 1;
/**/          
      }
      else
      {      
/*lazy stuff*/      
        uint32 ppuVAddrLatch_old = ppuVAddrLatch;
/**/        

        // t:0000001111100000=d:11111000
        ppuVAddrLatch = (ppuVAddrLatch & 0xFC1F) | (((uint16)(Value & 0xF8)) << 2);        
        // t:0111000000000000=d:00000111
        ppuVAddrLatch = (ppuVAddrLatch & 0x8FFF) | (((uint16)(Value & 0x07)) << 12);
     
/*lazy stuff*/         
        if (ppuVAddrLatch != ppuVAddrLatch_old)
          lazy_bg_need_update = 1;
/**/         
      }      
      return;

   case REG_VADDR:
      ppuFlipFlop ^= 1;
      
      if (ppuFlipFlop)
      {         
         // t:0011111100000000=d:00111111
        // t:1100000000000000=0
        ppuVAddrLatch = (ppuVAddrLatch & 0x00FF) | (((uint16)(Value & 0x3F)) << 8);
      }
      else
      {       
         // t:0000000011111111=d:11111111
        ppuVAddrLatch = (ppuVAddrLatch & 0xFF00) | ((uint16)Value);
        // v=t
        ppuVAddr = ppuVAddrLatch;        

        //map 96
        if (ppu_Latch_Address) ppu_Latch_Address(ppuVAddr);
      }

/*lazy stuff*/
      lazy_bg_need_update = 1;
/**/
      
      return;

   case REG_VDATA:
   {
      uint32 addr=ppuVAddr&0x3FFF;
      ppuVAddr += ppu_addr_inc;
      ppuVAddr &= 0xffff;
      if (addr < 0x3f00)
      {         
        if (addr>=0x3000) addr&=0xEFFF;         
//          if (!(Vnes.PPUPageIndexProtect[addr>>10]&&(addr<0x2000)))
        if (!Vnes.PPUPageIndexProtect[addr>>10])
         {
           // only allow writes if not protected
           if (ppu_vramInaccessible())
           {
                //fprintf(stderr, "inaccessible vram write at $%04x, scanline %d\n", 
                //        mVAddr, mScanline);
                Value = 0xff; // corrupt
           }
           Vnes.PPUPageIndex[addr>>10][addr & 0x3FF]=Value;
         }
      }
      else
      {
         Value&=0x3f;

         if (0 == (addr & 0xf))
         {
         /*palette mirror spr-bg*/
            Vnes.PPU_palette[0]=Value;
            Vnes.PPU_palette[0x10]=Value;
/*lazy stuff*/
            lazy_bg_need_update = lazy_spr_need_update = 1;
/**/
         }
         else /*if (addr & 3)*/
         {
/*lazy stuff*/
            if (Vnes.PPU_palette[addr&0x1f]!=Value)
            {
//         printk("pal %.2d = %.2d\n",addr&0x1f,Value);

                Vnes.PPU_palette[addr&0x1f]=Value;
                if (addr&0x10) lazy_spr_need_update=1;
                else lazy_bg_need_update=1;
            }
/**/
         }
      }      
      return;
   }
   default:
      return;
   }
}

void ppu_mirror(uint32 nt0, uint32 nt1, uint32 nt2, uint32 nt3)
{
   Vnes.var.ppu_mirror0=nt0;
   Vnes.var.ppu_mirror1=nt1;
   Vnes.var.ppu_mirror2=nt2;
   Vnes.var.ppu_mirror3=nt3;
   
   Vnes.PPUPageIndex[8] = (byte *)(Vnes.PPU_nametables + (nt0<<10));//(nt0 * PPU_BANKSIZE) ;//- (8 * PPU_BANKSIZE);
   Vnes.PPUPageIndex[9] = (byte *)(Vnes.PPU_nametables+ (nt1<<10));// * PPU_BANKSIZE) ;//- (9 * PPU_BANKSIZE);
   Vnes.PPUPageIndex[10] = (byte *)(Vnes.PPU_nametables+ (nt2<<10));// * PPU_BANKSIZE) ;//- (10 * PPU_BANKSIZE);
   Vnes.PPUPageIndex[11] = (byte *)(Vnes.PPU_nametables+ (nt3<<10));// * PPU_BANKSIZE) ;//- (11 * PPU_BANKSIZE);
   
   Vnes.PPUPageIndexProtect[8] = 0;
   Vnes.PPUPageIndexProtect[9] = 0;
   Vnes.PPUPageIndexProtect[10] = 0;
   Vnes.PPUPageIndexProtect[11] = 0;

   nametables_switched = 1;
   //ppu_syncHighPages();
}

uint8 miniRand(uint8 value)
{
   value = (value << 1) | (value >> 7);
   value ^= 0xff;
   return value;
}

void oam_reset(bool hardReset)
{
    int i ;
   oamNumCached = 0;
   
   if (hardReset)
   {
      // just trash whatever is in OAM
      for (i= 0; i < OAM_SIZE; i++)
      {
         oamMemory.memory[i] = 0;//miniRand(oamMemory.memory[i]);
      }
   }
}

void ppu_deinit()
{
    //free(patternTOpix);
}

void ppu_init()
{
    int i,j,pattern,pat0,pat1;
    //patternTOpix = malloc(65536*4*8);
    memset(patternTOpix,0,65536*4*8);
    for (i=0;i<65536;i++)
    for (j=0;j<4;j++)
    {
        pat0 = i&0xFF;
        pat1 = (i>>8)&0xFF;
        pattern = ((pat1 & 0xaa) << 8) | ((pat1 & 0x55) << 1)
                    | ((pat0 & 0xaa) << 7) | (pat0 & 0x55);
        if ((pattern >> 14) & 3) patternTOpix[i*4*8+j*8+0]= (j<<2) | ((pattern >> 14) & 3);
        if ((pattern >> 6) & 3) patternTOpix[i*4*8+j*8+1]= (j<<2) | ((pattern >> 6) & 3);
        if ((pattern >> 12) & 3) patternTOpix[i*4*8+j*8+2]= (j<<2) | ((pattern >> 12) & 3);
        if ((pattern >> 4) & 3) patternTOpix[i*4*8+j*8+3]= (j<<2) | ((pattern >> 4) & 3);
        if ((pattern >> 10) & 3) patternTOpix[i*4*8+j*8+4]= (j<<2) | ((pattern >> 10) & 3);
        if ((pattern >> 2) & 3) patternTOpix[i*4*8+j*8+5]= (j<<2) | ((pattern >> 2) & 3);
        if ((pattern >> 8) & 3) patternTOpix[i*4*8+j*8+6]= (j<<2) | ((pattern >> 8) & 3);
        if (pattern & 3) patternTOpix[i*4*8+j*8+7]= (j<<2) | (pattern & 3);
    }
    ppu_Latch_FDFE=NULL;
    ppu_Latch_Address=NULL;
    ppu_Latch_RenderScreen=NULL;
}

void ppu_reset(bool hardReset)
{
   int i;
   oam_reset(hardReset);   

   
   ppuVAddr = 0;
   ppuVAddrLatch = 0;
   ppuTileXOfs = 0;
   ppuFlipFlop = 0;
   ppuVDataLatch = 0;   
   Vnes.CPUMemory[0x4014]=0;
   ppuScanline = 0;         

   memset(ppuRegs, 0, sizeof(ppuRegs));
   memset(Vnes.PPU_patterntables, 0, 0x8000);
   memset(Vnes.PPU_nametables, 0, 0x1000);
   
   Vnes.PPUPageIndex[0x0] = Vnes.PPU_patterntables+(0<<10);
   Vnes.PPUPageIndex[0x1] = Vnes.PPU_patterntables+(1<<10);
   Vnes.PPUPageIndex[0x2] = Vnes.PPU_patterntables+(2<<10);
   Vnes.PPUPageIndex[0x3] = Vnes.PPU_patterntables+(3<<10);
   Vnes.PPUPageIndex[0x4] = Vnes.PPU_patterntables+(4<<10);
   Vnes.PPUPageIndex[0x5] = Vnes.PPU_patterntables+(5<<10);
   Vnes.PPUPageIndex[0x6] = Vnes.PPU_patterntables+(6<<10);
   Vnes.PPUPageIndex[0x7] = Vnes.PPU_patterntables+(7<<10);
   
   /*Vnes.PPUPageIndex[0x8] = Vnes.PPU_nametables;
   Vnes.PPUPageIndex[0x9] = Vnes.PPU_nametables;
   Vnes.PPUPageIndex[0xA] = Vnes.PPU_nametables;
   Vnes.PPUPageIndex[0xB] = Vnes.PPU_nametables;*/
   ppu_mirror(0,0,0,0);
   for (i=0;i<12;i++)  Vnes.PPUPageIndexProtect[i] = 0;//(i<8?true:false);
      
   ppu_sprite0 = 0;
   ppu_StrikeCycle = (uint32)(-1);   
   
   
   ppu_addr_inc = 0;
   bg_pattern_table_addr = 0;
   spr_pattern_table_addr = 0;
   patterntables_switched = 0;
   nametables_switched = 0;
   lazy_PPUPageIndex[0]=Vnes.PPUPageIndex[0];lazy_PPUPageIndex[1]=Vnes.PPUPageIndex[1];
   lazy_PPUPageIndex[2]=Vnes.PPUPageIndex[2];lazy_PPUPageIndex[3]=Vnes.PPUPageIndex[3];
   lazy_PPUPageIndex[4]=Vnes.PPUPageIndex[4];lazy_PPUPageIndex[5]=Vnes.PPUPageIndex[5];
   lazy_PPUPageIndex[6]=Vnes.PPUPageIndex[6];lazy_PPUPageIndex[7]=Vnes.PPUPageIndex[7];
   lazy_PPUPageIndex[8]=Vnes.PPUPageIndex[8];lazy_PPUPageIndex[9]=Vnes.PPUPageIndex[9];
   lazy_PPUPageIndex[10]=Vnes.PPUPageIndex[10];lazy_PPUPageIndex[11]=Vnes.PPUPageIndex[11];
         
   for (i=0;i<0x20;i++) 
   {
    Vnes.PPU_palette[i]=0;
    lazy_pal[i]=0;
   }
   
   InitMirroring();
}

/*INLINE void ppu_cacheOAM()
{   
   int count;
   obj_t *pObj;
   oamtemp_t temp;
   int offset;
   uint8 *data_ptr;
   uint32 vram_base = (ppuRegs[REG_CTRL0] & REGF_CTRL0_OBJADDR) ? 0x1000 : 0;
   uint32 vram_addr;
   

   ppuRegs[REG_STAT] &= ~REGF_STAT_MAXSPRITE;

   oamNumCached = 0;   
   for (count= 0; count < MAX_OBJ; count++)
   {
      pObj = &oamMemory.obj[count];

      // TODO: cleaner range check?
      if (pObj->y < 240 
          && pObj->y <= ppuScanline 
          && pObj->y + obj_height > ppuScanline)
      {
                  
         temp.attrib = pObj->attrib;
         if (count == 0)
            temp.attrib |= OAMF_SPRITE_0;
         else
            temp.attrib &= ~OAMF_SPRITE_0;
         temp.x = pObj->x;

         offset = ppuScanline - pObj->y;

         if (16 == obj_height)
         {
            if (offset > 7)
               offset += 8;

            if (pObj->attrib & OAMF_VFLIP)
               offset ^= 23;

            vram_addr = ((pObj->tile & 1) << 12) | ((pObj->tile & 0xfe) << 4);
         }
         else
         {
            if (pObj->attrib & OAMF_VFLIP)
               offset ^= 7;

            vram_addr = vram_base + pObj->tile * 16;
         }

         //data_ptr = &Vnes.PPUPageIndex[(vram_addr)>>10][(vram_addr)&0x3ff] + offset;
         data_ptr = Vnes.PPUPageIndex[(vram_addr)>>10] + ((vram_addr)&0x3ff) + offset;

         temp.pat0 = data_ptr[0];
         temp.pat1 = data_ptr[8];

     if (oamNumCached < MAX_CACHED) 
     {
       oamCache[oamNumCached++] = temp;
           if (oamNumCached == MAX_CACHED)
           {
              // sprite limit has been reached.
              ppuRegs[REG_STAT] |= REGF_STAT_MAXSPRITE;
              break;
           }
         }
      }
   }
}*/


void ppu_cacheOAM0()
{   
   obj_t *pObj;
   oamtemp_t temp;
   int offset;
   uint8 *data_ptr;
   uint32 vram_base = spr_pattern_table_addr;
   uint32 vram_addr;   

   ppuRegs[REG_STAT] &= ~REGF_STAT_MAXSPRITE;

   oamNumCached = 0;   
   
   pObj = &(oamMemory.obj[0]);

   // TODO: cleaner range check?
   if (pObj->y < 240 && pObj->y <= ppuScanline && pObj->y + obj_height > ppuScanline)
   {                  
         temp.attrib = pObj->attrib;
         temp.attrib |= OAMF_SPRITE_0;
         temp.x = pObj->x;
         offset = ppuScanline - pObj->y;
         if (16 == obj_height)
         {
            if (offset > 7)
               offset += 8;
            if (pObj->attrib & OAMF_VFLIP)
               offset ^= 23;
            vram_addr = ((pObj->tile & 1) << 12) | ((pObj->tile & 0xfe) << 4);
         }
         else
         {
            if (pObj->attrib & OAMF_VFLIP)
               offset ^= 7;
            vram_addr = vram_base + pObj->tile * 16;
         }                           
         data_ptr = (uint8 *)(Vnes.PPUPageIndex[(vram_addr)>>10] + ((vram_addr)&0x3ff) + offset);
         temp.pat0 = data_ptr[0];
         temp.pat1 = data_ptr[8];
         
         CHECK_MMC2(vram_addr);
         
     oamCache[oamNumCached++] = temp;                
   }
}




/*INLINE  void draw_bgtile(uint8 *buf, uint8 pat0, uint8 pat1, uint8 colRow)
{
   uint32 pattern = ((pat1 & 0xaa) << 8) | ((pat1 & 0x55) << 1)
                    | ((pat0 & 0xaa) << 7) | (pat0 & 0x55);

   *buf++ = (colRow | ((pattern >> 14) & 3));
   *buf++ = (colRow | ((pattern >> 6) & 3));
   *buf++ = (colRow | ((pattern >> 12) & 3));
   *buf++ = (colRow | ((pattern >> 4) & 3));
   *buf++ = (colRow | ((pattern >> 10) & 3));
   *buf++ = (colRow | ((pattern >> 2) & 3));
   *buf++ = (colRow | ((pattern >> 8) & 3));
   *buf = (colRow | (pattern & 3));
}*/

void draw_bgtile(uint8 *buf, uint32 pattern, uint32 colRow)
{
//      return;
   *((long long*)buf)=*((long long*)(patternTOpix+((pattern<<5)|(colRow<<1))));
/*
   pattern=(pattern<<5)|(colRow<<1);
   *buf++ = patternTOpix[pattern];
   *buf++ = patternTOpix[pattern|1];
   *buf++ = patternTOpix[pattern|2];
   *buf++ = patternTOpix[pattern|3];
   *buf++ = patternTOpix[pattern|4];
   *buf++ = patternTOpix[pattern|5];
   *buf++ = patternTOpix[pattern|6];
   *buf = patternTOpix[pattern|7];
*/
/*
    asm("                                                                    \n\
          stmfd sp!,{r12}                                                    \n\
          mov   R12,R0 @R12 = buf                                           \n\
                                                                             \n\
          ldr   R0,= patternTOpix                                            \n\
          mov   a,r0                                                         \n\
          mov   b,R10                                                        \n\
          ldr   R0,[R0,R10]                                                  \n\
          @    R1=pattern                                                   \n\
          @    R2=colRow                                                    \n\
          mov   R1,R1,lsl #5                                                 \n\
          orr   R1,R1,R2,lsl #1                                              \n\
          add   R0,R1,R0                                                     \n\
                                                                             \n\
          ldmia R0,{R1,R2}                                                   \n\
          stmia R12,{R1,R2}                                                  \n\
                                                                             \n\
          ldmfd sp!,{r12}                                                    \n\
    ");
*/
}


__IRAM_CODE /*__attribute__((noinline))*/ void draw_bgtileL(uint8 *buf, uint8 *dataptr, uint32 colRow, uint32 lines)
{
//      return;
#if 1
      uint32 *pToPix=(uint32*)(&(patternTOpix[colRow<<1]));
      uint32 *dest=(uint32*)buf;

      long long locdata[2];
      uint8 * locdataptr=(uint8 *)locdata;

      locdata[0]=((long long*)dataptr)[0];
      locdata[1]=((long long*)dataptr)[1];

      for (;lines;lines--,dest+=(272>>2),locdataptr++)
      {
          *((long long*)dest)=*((long long*)(pToPix+(((locdataptr[8]<<8)|(locdataptr[0]))<<3)));
      }

# else

    asm("                                                                      \n\
        stmfd sp!,{r0,r2-r7,lr}                                                 \n\
        ldr r4, =patternTOpix                                                \n\
        add r4,r4,r2,lsl #1              @ r4=patternTOpix[colRow<<1]        \n\
        sub sp,sp,#16\n\
        ldmia r1,{r2,r5,r6,r7}                                              \n\
        stmia sp,{r2,r5,r6,r7}                                              \n\
        mov r1,sp                                                                     \n\
                                                                             \n\
        looplines:                                                             \n\
            ldrb r2,[r1,#8]                  @ r6=dataptr[8]                     \n\
            ldrb r5,[r1],#1                  @ r5=dataptr[0] ; dataptr++         \n\
            orr r5,r5,r2,lsl #8              @ r5=(dataptr[0]) | (dataptr[8]<<8) \n\
            add r2,r4,r5,lsl #5              @ r6=pToPix+ofs<<3                  \n\
            ldmia r2,{r2,r5}                                                     \n\
            stmia r0,{r2,r5}                                                     \n\
            add   r0,r0,#272                                                     \n\
            subs  r3,r3,#1                                                       \n\
            bhi   looplines                                                      \n\
        add sp,sp,#16 \n\
        ldmfd sp!,{r0,r2-r7,pc}                                                  \n\
        .pool                                                                 \n\
   ");

#endif
}

#include "unes_ppu_render.h"

void ppu_renderScanline()
{
   uint8 *buffer;
   buffer = (uint8*)(Vnes.var.Vbuffer + ppuScanline*(256+16));
   /*if (!Vnes.var.asmBG) ppu_renderBGLine(buffer);
   else ppu_renderBGLinea(buffer,Vnes.PPUPageIndex);   */
   ppu_renderBGLine(buffer);
   //ppu_renderOAMLine(buffer);
   if (oamNumCached)
   {
     ppu_renderOAMLinea((uint32)(buffer));
   }
}

__IRAM_CODE void ppu_lazy_renderScanline(uint32 palmode)
{
    uint32 updated=0;
    uint32 startline;

    if ((palmode&&(ppuScanline==239))||(ppuScanline==(Vnes.var.DrawAllLines?239:231)))
    {
        lazy_bg_need_update=1;
        lazy_spr_need_update=1;
    }

    if (patterntables_switched)
    {

        if ( (Vnes.PPUPageIndex[0]!=lazy_PPUPageIndex[0])
            ||(Vnes.PPUPageIndex[1]!=lazy_PPUPageIndex[1])
            ||(Vnes.PPUPageIndex[2]!=lazy_PPUPageIndex[2])
            ||(Vnes.PPUPageIndex[3]!=lazy_PPUPageIndex[3])
            ||(Vnes.PPUPageIndex[4]!=lazy_PPUPageIndex[4])
            ||(Vnes.PPUPageIndex[5]!=lazy_PPUPageIndex[5])
            ||(Vnes.PPUPageIndex[6]!=lazy_PPUPageIndex[6])
            ||(Vnes.PPUPageIndex[7]!=lazy_PPUPageIndex[7])
           )
        {
            lazy_bg_need_update = 1;
            lazy_spr_need_update = 1;
        }
        else
        {
            patterntables_switched = 0;
        }
    }

    if (nametables_switched)
    {
        if ( (Vnes.PPUPageIndex[8]!=lazy_PPUPageIndex[8])
            ||(Vnes.PPUPageIndex[9]!=lazy_PPUPageIndex[9])
            ||(Vnes.PPUPageIndex[10]!=lazy_PPUPageIndex[10])
            ||(Vnes.PPUPageIndex[11]!=lazy_PPUPageIndex[11])
           )
        {
            lazy_bg_need_update = 1;
        }
        else nametables_switched=0;
    }

    if (lazy_bg_need_update || lazy_spr_need_update)
    {
        updated=1;
        startline=lazy_bg_start_line;
/*      if (lazy_bg_enabled) */
        {
            ppu_lazy_renderBGLines();
        }
        lazy_bg_start_line = ppuScanline;
        lazy_bg_need_update = 0;
        lazy_bg_enabled = ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON;
        lazy_bg_pattern_table_addr = bg_pattern_table_addr;
        lazy_ppuVAddr = ppuVAddr;
        lazy_ppuVAddrLatch = ppuVAddrLatch;
//      lazy_ppuTileXOfs = ppuTileXOfs;

        lazy_bgnoclip = ppuRegs[REG_CTRL1] & REGF_CTRL1_BGNOCLIP;
/*  }

    if (lazy_spr_need_update)
    {*/
        updated=2;
        if (lazy_spr_enabled)
        {
            ppu_lazy_renderSPRLines();
        }
        lazy_spr_start_line = ppuScanline;
        lazy_spr_need_update = 0;
        lazy_spr_enabled = ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJON;
        lazy_spr_pattern_table_addr = spr_pattern_table_addr;
        memcpy(lazy_oamMemory.memory, oamMemory.memory, 0x100);      
        
        lazy_objnoclip = ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJNOCLIP;
    }
    

    if (updated)
    {
#ifndef SCREEN_USE_DSP

        if (startline>7||(Vnes.var.DrawAllLines))
            XlatNESBufferLineLazy((uint8*)(Vnes.var.Vbuffer + startline*(256+16) + 8+(lazy_ppuTileXOfs)),(uint8*)(lj_curRenderingScreenPtr+startline*256*2),(uint8*)lazy_pal,startline);
        else
        {
          if (ppuScanline>8) XlatNESBufferLineLazy((uint8*)(Vnes.var.Vbuffer + 8*(256+16) + 8+(lazy_ppuTileXOfs)),(uint8*)(lj_curRenderingScreenPtr+8*256*2),(uint8*)lazy_pal,8);
        }




        if (updated==2)
        //both were updated
            memcpy(lazy_pal, Vnes.PPU_palette, 0x20);
        else
        //only bg was updated
            memcpy(lazy_pal, Vnes.PPU_palette, 0x10);

#endif
        lazy_ppuTileXOfs = ppuTileXOfs;
    }


    if (patterntables_switched)
    {
        lazy_PPUPageIndex[0]=Vnes.PPUPageIndex[0];lazy_PPUPageIndex[1]=Vnes.PPUPageIndex[1];
        lazy_PPUPageIndex[2]=Vnes.PPUPageIndex[2];lazy_PPUPageIndex[3]=Vnes.PPUPageIndex[3];
        lazy_PPUPageIndex[4]=Vnes.PPUPageIndex[4];lazy_PPUPageIndex[5]=Vnes.PPUPageIndex[5];
        lazy_PPUPageIndex[6]=Vnes.PPUPageIndex[6];lazy_PPUPageIndex[7]=Vnes.PPUPageIndex[7];
        patterntables_switched = 0;
    }

    if (nametables_switched) {
        lazy_PPUPageIndex[8]=Vnes.PPUPageIndex[8];lazy_PPUPageIndex[9]=Vnes.PPUPageIndex[9];
        lazy_PPUPageIndex[10]=Vnes.PPUPageIndex[10];lazy_PPUPageIndex[11]=Vnes.PPUPageIndex[11];
        nametables_switched = 0;
    }

    /* gli */
    Vnes.var.LineOffset[ppuScanline]=8+ppuTileXOfs;
}


void ppu_renderScanline0()
{
   uint8 *buffer;
   if (oamNumCached)
   {
    
    buffer = (uint8*)(Vnes.var.Vbuffer + ppuScanline*(256+16));
    /*if (!Vnes.var.asmBG) ppu_renderBGLine(buffer);   
    else ppu_renderBGLinea(buffer,Vnes.PPUPageIndex);   */
    ppu_renderBGLine0(buffer);
    //ppu_renderOAMLine(buffer);
    ppu_renderOAMLinea((uint32)(buffer));
   }   
}



__IRAM_CODE void ppu_beginScanline()
{
   if (ppu_enabled())
   {
      if (0 == ppuScanline)
      {
         // transfer whole latch
         ppuVAddr = ppuVAddrLatch;         
      }
      else
      {
         // reload scanline params
         //ppuVAddr = (ppuVAddr & 0x041f) | (ppuVAddrLatch & 0x041f);
         ppuVAddr = (ppuVAddr & 0xFBE0) | (ppuVAddrLatch & 0x041f);
      }
   }
}

__IRAM_CODE void ppu_endScanline()
{
   if (ppu_enabled())
   {      
      if((ppuVAddr & 0x7000) == 0x7000) /* is subtile y offset == 7? */ 
      {
        ppuVAddr &= 0x8FFF; /* subtile y offset = 0 */ 
        if((ppuVAddr & 0x03E0) == 0x03A0) /* name_tab line == 29? */ 
        { 
            ppuVAddr ^= 0x0800;  /* switch nametables (bit 11) */ 
            ppuVAddr &= 0xFC1F;  /* name_tab line = 0 */
        } 
        else 
        { 
          if((ppuVAddr & 0x03E0) == 0x03E0) /* line == 31? */ 
          { 
            ppuVAddr &= 0xFC1F;  /* name_tab line = 0 */ \
          }
          else 
          { 
            ppuVAddr += 0x0020; 
          } 
        } 
      } 
      else 
      { 
        ppuVAddr += 0x1000; /* next subtile y offset */ 
      } 
   }
}



void ppu_scanlineNTSC()
{
   if (ppuScanline < 240)
   {
      ppu_beginScanline();
      if (Vnes.var.DrawCframe) 
      {
        if ( (((uint32)(ppuScanline-8))<224)||(Vnes.var.DrawAllLines) )
        {
            ppu_renderScanline();
            XlatNESBufferLine((uint8*)(Vnes.var.Vbuffer + ppuScanline*(256+16) + 8 + ppuTileXOfs),(uint8*)(lj_curRenderingScreenPtr+ppuScanline*256),(uint8*)Vnes.PPU_palette,ppuScanline);
        }
        else ppu_renderScanline0();
      }
      else if (!ppu_sprite0) ppu_renderScanline0();
      
      ppu_endScanline();

      if (Vnes.var.DrawCframe) 
      {
        if (Vnes.var.morethan8spr) 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,16);//ppu_cacheOAM();
        }           
        else 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,8);//ppu_cacheOAM();
        }
      }
      else if (!ppu_sprite0) 
      {
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
        ppu_cacheOAM0();
      }

   }
   else if (ppuScanline == 241)
   {
      ppuRegs[REG_STAT] |= REGF_STAT_VBLINT;
   }
   else if (ppuScanline == 261)
   {
      ppuRegs[REG_STAT] &= ~REGF_STAT_VBLINT; // does this go down?
      
      ppu_sprite0 = 0;
      ppu_StrikeCycle = (uint32)(-1);
      
      ppuRegs[REG_STAT] &= ~REGF_STAT_STRIKE;

      // special case: garbage rendered scanline with sole
      // intent of caching sprite data for next scanline
      //beginScanline();
      //endScanline();

      if (Vnes.var.DrawCframe) 
      {
        if (Vnes.var.morethan8spr) 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,16);//ppu_cacheOAM();
        }
          else 
          {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,8);//ppu_cacheOAM();
          }
      }
      else if (!ppu_sprite0) 
      {
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
        ppu_cacheOAM0();
      }
      
   }

   ppuScanline++;
   if (ppuScanline >= 262)
      ppuScanline = 0;
}


__IRAM_CODE void ppu_lazy_scanlineNTSC()
{
   if (ppuScanline < 240)
   {
      /*lazy stuff*/
      if (ppuScanline==0)
      {
        lazy_bg_start_line = 0;
        lazy_spr_start_line = 0;
        lazy_bg_need_update = 0;
        lazy_spr_need_update = 0;
        if (patterntables_switched)
        {
            patterntables_switched = 0;
            lazy_PPUPageIndex[0]=Vnes.PPUPageIndex[0];lazy_PPUPageIndex[1]=Vnes.PPUPageIndex[1];
            lazy_PPUPageIndex[2]=Vnes.PPUPageIndex[2];lazy_PPUPageIndex[3]=Vnes.PPUPageIndex[3];
            lazy_PPUPageIndex[4]=Vnes.PPUPageIndex[4];lazy_PPUPageIndex[5]=Vnes.PPUPageIndex[5];
            lazy_PPUPageIndex[6]=Vnes.PPUPageIndex[6];lazy_PPUPageIndex[7]=Vnes.PPUPageIndex[7];
        }
        if (nametables_switched)
        {
            lazy_PPUPageIndex[8]=Vnes.PPUPageIndex[8];lazy_PPUPageIndex[9]=Vnes.PPUPageIndex[9];
            lazy_PPUPageIndex[10]=Vnes.PPUPageIndex[10];lazy_PPUPageIndex[11]=Vnes.PPUPageIndex[11];
            nametables_switched = 0;
        }
        lazy_ppuVAddr = ppuVAddr;
        lazy_ppuVAddrLatch = ppuVAddrLatch;
        lazy_ppuTileXOfs = ppuTileXOfs;
        lazy_bg_pattern_table_addr = bg_pattern_table_addr;
        lazy_spr_pattern_table_addr = spr_pattern_table_addr;

        lazy_objnoclip = ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJNOCLIP;
        lazy_bgnoclip = ppuRegs[REG_CTRL1] & REGF_CTRL1_BGNOCLIP;

        memcpy(lazy_pal, Vnes.PPU_palette, 0x20);
        memcpy(lazy_oamMemory.memory, oamMemory.memory, 0x100);

        lazy_bg_enabled = ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON;
        lazy_spr_enabled = ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJON;

        if (ppu_enabled)
        {
            lazy_ppuVAddr=lazy_ppuVAddrLatch;
        }
      }
      /*******/

      if (Vnes.var.DrawCframe)
      {
        if ( (ppuScanline<232)||Vnes.var.DrawAllLines)
        {
            ppu_lazy_renderScanline(0);
        }
      }

      ppu_beginScanline();
      if (!ppu_sprite0) ppu_renderScanline0();

      ppu_endScanline();

      if (!ppu_sprite0)
      {
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);
        ppu_cacheOAM0();
      }
   }
   else if (ppuScanline == 241)
   {
      ppuRegs[REG_STAT] |= REGF_STAT_VBLINT;
   }
   else if (ppuScanline == 261)
   {
      ppuRegs[REG_STAT] &= ~REGF_STAT_VBLINT; // does this go down?

      ppu_sprite0 = 0;
      ppu_StrikeCycle = (uint32)(-1);

      ppuRegs[REG_STAT] &= ~REGF_STAT_STRIKE;

      // special case: garbage rendered scanline with sole
      // intent of caching sprite data for next scanline
      //beginScanline();
      //endScanline();
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);
        ppu_cacheOAM0();

   }
   ppuScanline++;
   if (ppuScanline >= 262)
      ppuScanline = 0;
}





void ppu_scanlinePAL()
{
   if (ppuScanline < 240)
   {
      ppu_beginScanline();      
      if (Vnes.var.DrawCframe) 
      {
        ppu_renderScanline();
        XlatNESBufferLine((uint8*)(Vnes.var.Vbuffer + ppuScanline*(256+16) + 8 + ppuTileXOfs),(uint8*)(lj_curRenderingScreenPtr+ppuScanline*256),(uint8*)Vnes.PPU_palette ,ppuScanline);
      }
      else ppu_renderScanline0();            
      ppu_endScanline();

      if (Vnes.var.DrawCframe)
      {
        if (Vnes.var.morethan8spr) 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);
            ppu_cacheOAMa((uint32)(Vnes.PPUPageIndex),16);//ppu_cacheOAM();
        }
        else 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,8);//ppu_cacheOAM();
        }
      }
      else if (!ppu_sprite0) 
      {
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
        ppu_cacheOAM0();        
      }
   }
   else if (ppuScanline == 241)
   {
      ppuRegs[REG_STAT] |= REGF_STAT_VBLINT;
   }
   else if (ppuScanline == 311)
   {
      ppuRegs[REG_STAT] &= ~REGF_STAT_VBLINT; // does this go down?
      
      ppu_sprite0 = 0;
      ppu_StrikeCycle = (uint32)(-1);
      
      ppuRegs[REG_STAT] &= ~REGF_STAT_STRIKE;

      // special case: garbage rendered scanline with sole
      // intent of caching sprite data for next scanline
      //beginScanline();
      //endScanline();

      if (Vnes.var.DrawCframe) 
      {
        if (Vnes.var.morethan8spr) 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,16);//ppu_cacheOAM();
        }
        else 
        {
            if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
            ppu_cacheOAMa((uint32)Vnes.PPUPageIndex,8);//ppu_cacheOAM();
        }
      }
      else if (!ppu_sprite0)
      {
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
        ppu_cacheOAM0();
      }
   }

   ppuScanline++;
   if (ppuScanline >= 312)
      ppuScanline = 0;
}


void ppu_lazy_scanlinePAL()
{
   if (ppuScanline < 240)
   {                         
      /*lazy stuff*/
      if (ppuScanline==0)
      {      
        lazy_bg_start_line = 0;
        lazy_spr_start_line = 0;
        lazy_bg_need_update = 0;
        lazy_spr_need_update = 0;
        if (patterntables_switched) 
        {           
            patterntables_switched = 0;
            lazy_PPUPageIndex[0]=Vnes.PPUPageIndex[0];lazy_PPUPageIndex[1]=Vnes.PPUPageIndex[1];
            lazy_PPUPageIndex[2]=Vnes.PPUPageIndex[2];lazy_PPUPageIndex[3]=Vnes.PPUPageIndex[3];
            lazy_PPUPageIndex[4]=Vnes.PPUPageIndex[4];lazy_PPUPageIndex[5]=Vnes.PPUPageIndex[5];
            lazy_PPUPageIndex[6]=Vnes.PPUPageIndex[6];lazy_PPUPageIndex[7]=Vnes.PPUPageIndex[7];
        }   
        if (nametables_switched) 
        {
            lazy_PPUPageIndex[8]=Vnes.PPUPageIndex[8];lazy_PPUPageIndex[9]=Vnes.PPUPageIndex[9];
            lazy_PPUPageIndex[10]=Vnes.PPUPageIndex[10];lazy_PPUPageIndex[11]=Vnes.PPUPageIndex[11];
            nametables_switched = 0;
        }
        lazy_ppuVAddr = ppuVAddr;
        lazy_ppuVAddrLatch = ppuVAddrLatch;
        lazy_ppuTileXOfs = ppuTileXOfs;
        lazy_bg_pattern_table_addr = bg_pattern_table_addr;
        lazy_spr_pattern_table_addr = spr_pattern_table_addr;
        
        lazy_objnoclip = ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJNOCLIP;
        lazy_bgnoclip = ppuRegs[REG_CTRL1] & REGF_CTRL1_BGNOCLIP;

        memcpy(lazy_pal, Vnes.PPU_palette, 0x20);
        memcpy(lazy_oamMemory.memory, oamMemory.memory, 0x100);      

        lazy_bg_enabled = ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON;
        lazy_spr_enabled = ppuRegs[REG_CTRL1] & REGF_CTRL1_OBJON;
        
        if (ppu_enabled)
        {
            lazy_ppuVAddr=lazy_ppuVAddrLatch;
        }
      }
      /*******/

      if ((Vnes.var.DrawCframe) && ( (ppuScanline<232)||(Vnes.var.DrawAllLines)))
      {         
        ppu_lazy_renderScanline(1);
      }


      ppu_beginScanline();
      if (!ppu_sprite0) ppu_renderScanline0();
      
      ppu_endScanline();

      if (!ppu_sprite0) 
      {
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
        ppu_cacheOAM0();
      }
      
   }
   else if (ppuScanline == 241)
   {
      ppuRegs[REG_STAT] |= REGF_STAT_VBLINT;
   }
   else if (ppuScanline == 311)
   {
      ppuRegs[REG_STAT] &= ~REGF_STAT_VBLINT; // does this go down?
      
      ppu_sprite0 = 0;
      ppu_StrikeCycle = (uint32)(-1);
      
      ppuRegs[REG_STAT] &= ~REGF_STAT_STRIKE;

      // special case: garbage rendered scanline with sole
      // intent of caching sprite data for next scanline
      //beginScanline();
      //endScanline();
        if(ppu_Latch_RenderScreen) ppu_Latch_RenderScreen(0,0);          
        ppu_cacheOAM0();

   }
   ppuScanline++;
   if (ppuScanline >= 312)
      ppuScanline = 0;
}


void InitMirroring()
{
  switch (Vnes.var.mirroring)
   {
   case 1://MIRROR_VERT:
      ppu_mirror(0, 1, 0, 1);
      break;
   case 2://MIRROR_HORIZ:
      ppu_mirror(0, 0, 1, 1);      
      break;
   case 3://MIRROR_ONE_LOW:
  //    ppu_mirror(0, 0, 0, 0);
      break;
   case 4://MIRROR_ONE_HIGH:
//      ppu_mirror(1, 1, 1, 1);
      break;
   case 5://MIRROR_FOUR:
      ppu_mirror(0, 1, 2, 3);
      break;
   default:
   //not defined!!!!      
      ppu_mirror(0, 1, 2, 3);
      break;
   }
    
}


void ppu_savestate(SnssBaseBlock *baseBlock)
{
  baseBlock->reg2000=ppuRegs[0];
  baseBlock->reg2001=ppuRegs[1];
  baseBlock->vramAddress=ppuVAddr;
  baseBlock->spriteRamAddress=ppuRegs[3];  
  baseBlock->tileXOffset=ppuTileXOfs;
  memcpy(baseBlock->spriteRam,oamMemory.memory,256);
}

void ppu_loadstate(SnssBaseBlock *baseBlock)
{
  ppuRegs[0]=baseBlock->reg2000;
  ppuRegs[1]=baseBlock->reg2001;  
  ppuVAddr=baseBlock->vramAddress;
  ppuRegs[3]=baseBlock->spriteRamAddress;
  ppuTileXOfs=baseBlock->tileXOffset;
  memcpy(oamMemory.memory,baseBlock->spriteRam,256);
}



void ppu_set_Latch_FDFE(PPU_Latch_FDFE_t func)
{
   ppu_Latch_FDFE = func;
}

void ppu_set_Latch_Address(PPU_Latch_Address_t func)
{
   ppu_Latch_Address = func;
}

void ppu_set_Latch_RenderScreen(PPU_Latch_RenderScreen_t func)
{
   ppu_Latch_RenderScreen = func;
}

uint8 ppu_noLatchFunc(void)
{
  return ((!ppu_Latch_FDFE)&&(!ppu_Latch_Address)&&(!ppu_Latch_RenderScreen));
}
