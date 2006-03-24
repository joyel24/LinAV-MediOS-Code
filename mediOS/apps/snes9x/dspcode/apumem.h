/*
 * Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
 *
 * (c) Copyright 1996 - 2000 Gary Henderson (gary@daniver.demon.co.uk) and
 *                           Jerremy Koot (jkoot@snes9x.com)
 *
 * Super FX C emulator code 
 * (c) Copyright 1997 - 1999 Ivar (Ivar@snes9x.com) and
 *                           Gary Henderson.
 * Super FX assembler emulator code (c) Copyright 1998 zsKnight and _Demo_.
 *
 * DSP1 emulator code (c) Copyright 1998 Ivar, _Demo_ and Gary Henderson.
 * DOS port code contains the works of other authors. See headers in
 * individual files.
 *
 * Snes9x homepage: www.snes9x.com
 *
 * Permission to use, copy, modify and distribute Snes9x in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Snes9x is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Snes9x or software derived from Snes9x.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so everyone can benefit from the modifications
 * in future versions.
 *
 * Super NES and Super Nintendo Entertainment System are trademarks of
 * Nintendo Co., Limited and its subsidiary companies.
 */

#ifndef _apumemory_h_
#define _apumemory_h_

#include "mmu.h"

START_EXTERN_C
extern uint8 W4;
extern uint8 APUROM[64];
END_EXTERN_C

/* VP : defined in libspc.cpp */
extern int SPC_slowdown_instructions;
extern int SPC_slowdown_instructions2;

extern short timers[3];

INLINE uint8 S9xAPUGetByteZ (uint8 Address)
{
    if (Address >= 0xf0 && IAPU.DirectPage == 0/*IAPU.RAM*/)
    {
	if (Address >= 0xf4 && Address <= 0xf7)
	{
#ifdef SPC700_SHUTDOWN
	    IAPU.WaitAddress2 = IAPU.WaitAddress1;
	    IAPU.WaitAddress1 = IAPU.PC;
#endif	    
	    if (IAPU.Slowdown < SPC_slowdown_instructions2)
	      IAPU.Slowdown = SPC_slowdown_instructions2; /* VP : start a slowdown of given number of instructions */
	    return GET(Address);
	}
	if (Address >= 0xfd)
	{
#ifdef SPC700_SHUTDOWN
	    IAPU.WaitAddress2 = IAPU.WaitAddress1;
	    IAPU.WaitAddress1 = IAPU.PC;
#endif	    
	    IAPU.Slowdown = SPC_slowdown_instructions; /* VP : start a slowdown of given number of instructions */
/* 	    uint8 t = GET(Address); */
/* 	    SET(Address, 0); */
	    Address -= 0xfd;
	    uint8 t = timers[Address];
	    timers[Address] = 0;
	    return (t);
	}
	else
	if (Address == 0xf3)
	    return (S9xGetAPUDSP ());

	return GET(Address);
    }
    else
      return GET(IAPU.DirectPage + Address);
}

INLINE void S9xAPUSetByteZ (uint8 byte, uint8 Address)
{
    //byte &= 0xff;
    if (Address >= 0xf0 && IAPU.DirectPage == 0/*IAPU.RAM*/)
    {
	if (Address == 0xf3)
	    S9xSetAPUDSP (byte);
	else
	if (Address >= 0xf4 && Address <= 0xf7)
	    APU.OutPorts [Address - 0xf4] = byte;
	else
	if (Address == 0xf1)
	    S9xSetAPUControl (byte);
	else
	if (Address < 0xfd)
	{
	    SET(Address, byte);
	    if (Address >= 0xfa)
	    {
		if (byte == 0)
		    APU.TimerTarget [Address - 0xfa] = 0x100;
		else
		    APU.TimerTarget [Address - 0xfa] = byte;
	    }
	}
    }
    else
      SET(IAPU.DirectPage + Address, byte);
}

INLINE uint8 S9xAPUGetByte (uint16 Address)
{
    if (Address <= 0xff && Address >= 0xf0)
    {
	if (Address >= 0xf4 && Address <= 0xf7)
	{
#ifdef SPC700_SHUTDOWN
	    IAPU.WaitAddress2 = IAPU.WaitAddress1;
	    IAPU.WaitAddress1 = IAPU.PC;
#endif
	    if (IAPU.Slowdown < SPC_slowdown_instructions2)
	      IAPU.Slowdown = SPC_slowdown_instructions2; /* VP : start a slowdown of given number of instructions */
	    return GET(Address);
	}
	else
	if (Address == 0xf3)
	    return (S9xGetAPUDSP ());
	if (Address >= 0xfd)
	{
#ifdef SPC700_SHUTDOWN
	    IAPU.WaitAddress2 = IAPU.WaitAddress1;
	    IAPU.WaitAddress1 = IAPU.PC;
#endif
	    IAPU.Slowdown = SPC_slowdown_instructions; /* VP : start a slowdown of given number of instructions */
/* 	    uint8 t = GET(Address); */
/* 	    SET(Address, 0); */
	    Address -= 0xfd;
	    uint8 t = timers[Address];
	    timers[Address] = 0;
	    return (t);
	}
	return GET(Address);
    }
    else
	return GET(Address);
}

INLINE void S9xAPUSetByte (uint8 byte, uint16 Address)
{
    //byte &= 0xff;
    if (Address <= 0xff && Address >= 0xf0)
    {
	if (Address == 0xf3)
	    S9xSetAPUDSP (byte);
	else
	if (Address >= 0xf4 && Address <= 0xf7)
	    APU.OutPorts [Address - 0xf4] = byte;
	else
	if (Address == 0xf1)
	    S9xSetAPUControl (byte);
	else
	if (Address < 0xfd)
	{
	    SET(Address, byte);
	    if (Address >= 0xfa)
	    {
		if (byte == 0)
		    APU.TimerTarget [Address - 0xfa] = 0x100;
		else
		    APU.TimerTarget [Address - 0xfa] = byte;
	    }
	}
    }
    else
    {
	if (Address < 0xffc0)
	  SET(Address, byte);
	else
	{
	    APU.ExtraRAM [Address - 0xffc0] = byte;
	    if (!APU.ShowROM)
	      SET(Address, byte);
	}
    }
}

#endif
