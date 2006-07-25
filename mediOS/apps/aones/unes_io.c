/*
  Little John GP32
  File : unes_io.c
  Authors : Yoyo
  Version : 0.2 
  Last update : 7th July 2003
*/

#include "medios.h"

#include "aones.h"

#include "unes.h"
#include "unes_io.h"
#include "unes_ppu.h"
#include "unes_mapper.h"
#include "nes_apu.h"

extern uint32 debugison;

extern char chaine[1024];
extern char chaineDEB[1024];

extern mmc_t *mapper;

byte lastPPUread;
word vramlatch=0;

__IRAM_DATA static uint8 pad_strobe;
__IRAM_DATA static uint32 pad1_bits,pad2_bits;

__IRAM_CODE void Wr6502(uint32 Addr, uint8 Value)
{
    Addr&=0xffff;


#ifdef __debug_io__
    if (framecounter>=debug_start_frame) debugison=1;

    if (debugison)
    {
        sprintf(chaine,"write rom byte %08X %02X on sc %d **",Addr,Value,ppu_currentscanline());
        gpprintf(0,0,chaine,0,0,0xe0);
        debug_info();
    }
#endif

  //printf("wr %04X=%d\n",Addr,Value);


    switch (Addr&0xF000)
    {
        case 0x0:  //RAM access
        case 0x1000:
        {
            Vnes.CPUPageIndex[0][Addr&0x7FF]=Value;
            return;
        }

        case 0x2000: //Registers 0x2000 access
        case 0x3000:
            ppu_writereg(Addr,Value);
            return;
        case 0x4000:  //Registers 0x4000 access  TO DO
            Vnes.CPUMemory[Addr]=Value;
            if (Addr<0x4018)
            {
                switch (Addr)
                {
                    case 0x4014:   //DMA transfer to SPRram, takes 512 cpu cycles
                        ppu_dma(Value);
                        break;
                    case 0x4016:  //Joypad
                        // bit 0 == joypad strobe
                        if(Value & 0x01) pad_strobe = 1;
                        else
                        if(pad_strobe)
                        {
                            pad_strobe = 0;
                            //pad3_count=0, pad4_count=0;       //Pad read counter clear
                                // get input states
                            /*pad1_bits = *pad1;
                            pad2_bits = *pad2;
                            pad3_bitsnes = pad3_bits = *pad3;
                            pad4_bitsnes = pad4_bits = *pad4;               */
                            Vnes.var.JoyPad1_BitIndex=0;
                            Vnes.var.JoyPad2_BitIndex=0;
                            pad1_bits=emu_joypad1State();
                            pad2_bits=emu_joypad2State();
                        }
                        break;
                    case 0x4017:
                        if(!Vnes.var.frame_irq_disenabled)
                        {
                            Vnes.var.frame_irq_enabled = Value;
                        }
                        break;
                    default:
                        apu_write(Addr,Value);
                }
                
                if (mapper->intf->WriteHighRegs) mapper->intf->WriteHighRegs(Addr,Value);

                return;
            } else if (mapper->intf->MemoryWriteLow) mapper->intf->MemoryWriteLow(Addr,Value);

            return;
        case 0x5000:
            //Expansion modules : dont know about them
            //Vnes.CPUMemory[Addr]=Value;
            if (mapper->intf->MemoryWriteLow) mapper->intf->MemoryWriteLow(Addr,Value);
            return;
        case 0x6000:
        case 0x7000:
            //SRAM
            //if (Vnes.var.sramEnabled)
            Vnes.NESSRAM[Addr-0x6000]=Value;
            if (mapper->intf->MemoryWriteSaveRAM) mapper->intf->MemoryWriteSaveRAM(Addr,Value);
            //Vnes.var.SaveRAM=1;
            return;
        default:
            // PRG-ROM
            if (mapper->intf->MemoryWrite) mapper->intf->MemoryWrite(Addr,Value);
    }
}

__IRAM_CODE uint8 Rd6502(uint32 Addr)
{
    uint32 Value;
  
    Addr&=0xffff;
  
    switch (Addr&0xF000)
    {
        case 0:
        case 0x1000:
        //RAM access
        {
            return Vnes.CPUPageIndex[0][Addr&0x7FF] ;  //Vnes.var.mirroring
        }
        case 0x2000:
        case 0x3000: //Registers 0x2000 access
            //Value=ppu_readreg(Addr);
            //break;
            return ppu_readreg(Addr);
        case 0x4000:  //Registers 0x4000 access  TO DO
            switch (Addr)
            {
                case 0x4014:  //ppu_dma read reg 4014
                    return Vnes.CPUMemory[0x4014];
                case 0x4015:
                    if  (!(Vnes.var.frame_irq_enabled & 0xC0)) return apu_read(0x4015)|0x40;
                    else return apu_read(0x4015);
                case 0x4016:
                {
                    Value=0;
                    if(Vnes.var.JoyPad1_BitIndex<8)
                    {
                        // joypad 1
                        Value |=  pad1_bits & 0x01;
                        pad1_bits >>= 1;
                        // joypad 3 (Famicom)
                        /*Value |=  (pad3_bits & 0x01)<<1;
                        pad3_bits >>= 1;*/
                    }
                    else if(Vnes.var.JoyPad1_BitIndex<16)
                        {     //NES 4play (joypad 3)
                            /*Value |=  (pad3_bitsnes & 0x01);
                            pad3_bitsnes >>= 1;*/
                        }
                    // mic on joypad 2
                    /*Value |= *mic_bits;
                    Value |= *coin_bits;*/
                    Value |= 0x40;
                    ++Vnes.var.JoyPad1_BitIndex;

                    /*Vnes.CPUMemory[0x4016]=((JoyPad1_State()>>Vnes.var.JoyPad1_BitIndex)&1);//|0x40;
                    Vnes.var.JoyPad1_BitIndex++;
                    if (Vnes.var.JoyPad1_BitIndex==8) Vnes.var.JoyPad1_BitIndex=0;
                    return Vnes.CPUMemory[0x4016];*/
                    return Value;
                }
                case 0x4017:
                {
                    Value=0;
                    if(Vnes.var.JoyPad2_BitIndex<8)
                    {
                        // joypad 1
                        Value |=  pad2_bits & 0x01;
                        pad2_bits >>= 1;
                        // joypad 3 (Famicom)
                        /*Value |=  (pad3_bits & 0x01)<<1;
                        pad3_bits >>= 1;*/
                    }
                    else if(Vnes.var.JoyPad2_BitIndex<16)
                    {     //NES 4play (joypad 3)
                        /*Value |=  (pad3_bitsnes & 0x01);
                        pad3_bitsnes >>= 1;*/
                    }
                    // mic on joypad 2
                    /*Value |= *mic_bits;
                    Value |= *coin_bits;*/
                    ++Vnes.var.JoyPad2_BitIndex;


                    //Value=Vnes.CPUMemory[0x4017];
                    //break;
                    //return Vnes.CPUMemory[0x4017];
                    return Value;
                }
                default:
                   //Value=apu_read(Addr);
                   //Value=Vnes.CPUMemory[Addr];
                   if (Addr<0x4016) return apu_read(Addr); //Value=apu_read(Addr);
                   else
                   if (mapper->intf->MemoryReadLow) return(mapper->intf->MemoryReadLow(Addr));
            }
            return (uint8)(Addr>>8);
        case 0x5000:  //Expansion modules
            if (mapper->intf->MemoryReadLow) return(mapper->intf->MemoryReadLow(Addr));
            return (uint8)(Addr >> 8);
        case 0x6000:
        case 0x7000:  //SRAM
            if (mapper->intf->MemoryReadSaveRAM) mapper->intf->MemoryReadSaveRAM(Addr);
            return Vnes.CPUPageIndex[3][Addr-0x6000];
        default:
            //      printf("rd at %04X, returned : %d\n",Addr,Vnes.CPUPageIndex[(Addr>>13)][Addr&0x1fff]);
            if (mapper->intf->MemoryReadSaveRAM) mapper->intf->MemoryReadSaveRAM(Addr);
            return Vnes.CPUPageIndex[(Addr>>13)][Addr&0x1fff];
    }



#ifdef __debug_io__
    if (framecounter>=debug_start_frame) debugison=1;
    if (debugison)
    {
        sprintf(chaine,"read rom byte %08X, got %02X on sc %d **",Addr,Value,ppu_currentscanline());
        gpprintf(0,0,chaine,0,0,0xe0);
        debug_info();
    }
#endif

    return Value;
}

