
// if we have spare ram, it is worth using the INLINE_GET_SET for a small speed gain

// size optimization
#define SOPT


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

extern "C" {
  //#include <stdio.h>
}

#include "snes9x.h"
#include "spc700.h"
#include "memmap.h"
// #include "display.h"
// #include "cpuexec.h"
#include "apu.h"

// SPC700/Sound DSP chips have a 24.57MHz crystal on their PCB.

#ifdef NO_INLINE_SET_GET
uint8 S9xAPUGetByteZ (uint8 address);
uint8 S9xAPUGetByte (uint16 address);
void S9xAPUSetByteZ (uint8, uint8 address);
void S9xAPUSetByte (uint8, uint16 address);
#include "mmu.h"
#else
#undef INLINE
#define INLINE inline
#include "apumem.h"
#endif

START_EXTERN_C
// extern uint8 Work8;
// extern uint16 Work16;
// extern uint32 Work32;
// extern signed char Int8;
// extern short Int16;
// extern long Int32;
// extern uint8 W1;
// extern uint8 W2;

END_EXTERN_C

//#define OP0 (GET(IAPU.PC))
uint8 OP0;
#if 0
uint8 OP1;
uint8 OP2;
#else
#define OP1 (GET(IAPU.PC + 1))
#define OP2 (GET(IAPU.PC + 2))
#endif

#define ES(a) ( (a) - ( ((a)&0x80)<<1 ) )

#ifdef SPC700_SHUTDOWN
#define APUShutdown() \
    if (Settings.Shutdown && (IAPU.PC == IAPU.WaitAddress1 || IAPU.PC == IAPU.WaitAddress2)) \
    { \
	if (IAPU.WaitCounter == 0) \
	{ \
	    /*if (!ICPU.CPUExecuting)*/ \
		/*APU.Cycles = CPU.Cycles = CPU.NextEvent;*/ \
	    /*else*/ \
		IAPU.APUExecuting = FALSE; \
	} \
	else \
	if (IAPU.WaitCounter >= 2) \
	    IAPU.WaitCounter = 1; \
	else \
	    IAPU.WaitCounter--; \
    }
#else
#define APUShutdown()
#endif

#define APUSetZN8(b)\
    IAPU._Zero = (b)&0xff;

#define APUSetZN16(w)\
  IAPU._Zero = ((w) != 0) | ((w) >> 8);

#if 0
void STOP (char *s)
{

#ifdef DEBUGGER
    char buffer[100];
    S9xAPUOPrint (buffer, IAPU.PC /*- 0*/);
#endif

    //    sprintf (String, "Sound CPU in unknown state executing %s at %04X\n%s\n", s, IAPU.PC /*- 0*/, buffer);
//     S9xMessage (S9X_ERROR, S9X_APU_STOPED, String);
    APU.TimerEnabled[0] = APU.TimerEnabled[1] = APU.TimerEnabled[2] = FALSE;
    IAPU.APUExecuting = FALSE;

#ifdef DEBUGGER
    CPU.Flags |= DEBUG_MODE_FLAG;
#else
    // S9xExit ();
#endif
}
#endif

#define TCALL(n)\
{\
    PushW (IAPU.PC /*- 0*/ + 1); \
    IAPU.PC = /*0 +*/ (APU.ExtraRAM [((15 - n) << 1)] + \
	     (APU.ExtraRAM [((15 - n) << 1) + 1] << 8)); \
}

// XXX: HalfCarry
#define SBC(a,b)\
  { short Int16 = (short) (a) - (short) (b) + (short) (APUCheckCarry ()) - 1;	\
APUClearHalfCarry ();\
IAPU._Carry = Int16 >= 0;\
  if ((((a) ^ (b)) & 0x80) && (((a) ^ (Int16 /* VP TO CHECK */)) & 0x80))	\
    APUSetOverflow ();\
else \
    APUClearOverflow (); \
(a) = Int16&0xff;\
APUSetZN8 (Int16&0xff); \
  }

// XXX: HalfCarry
#define ADC(a,b)\
  {uint16 Work16 = (a) + (b) + APUCheckCarry();	\
APUClearHalfCarry ();\
IAPU._Carry = Work16 >= 0x100; \
  if (~((a) ^ (b)) & ((b) ^ (Work16 /* VP TO CHECK */)) & 0x80)	\
    APUSetOverflow ();\
else \
    APUClearOverflow (); \
(a) = Work16&0xff;\
APUSetZN8 (Work16&0xff); \
  }

// VP can it be optimized ?
#define CMP(a,b)\
  { short Int16 = (short) (a) - (short) (b);	\
IAPU._Carry = Int16 >= 0;\
APUSetZN8 (Int16&0xff); \
  }

// #define ASL(b)\
//     IAPU._Carry = ((b) & 0x80) != 0; \
//     (b) = ((b) << 1)&0xff;	     \
//     APUSetZN8 (b);
#define ASL(b)\
    IAPU._Carry = ((b)/* & 0x80*/) >> 7; \
    (b) = ((b) << 1)&0xff;	     \
    APUSetZN8 (b);
#define LSR(b)\
    IAPU._Carry = (b) & 1;\
    (b) >>= 1;\
    APUSetZN8 (b);
// #define ROL(b)\
//   {uint16 Work16 = ((b) << 1) | APUCheckCarry ();	\
//     IAPU._Carry = Work16 >= 0x100; \
//     (b) = Work16&0xff; \
//     APUSetZN8 (b); \
//   }
#define ROL(b)\
  {uint16 Work16 = ((b) << 1) | APUCheckCarry ();	\
    IAPU._Carry = (b)>>7;				\
    (b) = Work16&0xff; \
    APUSetZN8 (b); \
  }
// #define ROR(b)\
//     Work16 = (b) | ((uint16) APUCheckCarry () << 8); \
//     IAPU._Carry = Work16 & 1; \
//     Work16 >>= 1; \
//     (b) = Work16&0xff; \
//     APUSetZN8 (b);
// VP simplified
#define ROR(b)\
  { uint16 Work16 = ((b)>>1) | (APUCheckCarry () << 7);		\
    IAPU._Carry = (b) & 1;				\
    (b) = Work16;					\
    APUSetZN8 (b); \
  }

#define Push(b)\
  SET(/*0 +*/ 0x100 + APURegisters.S, b);	\
  APURegisters.S = (APURegisters.S - 1)&0xff;

#define Pop(b)\
  APURegisters.S = (APURegisters.S + 1)&0xff;	\
  (b) = GET(/*0 +*/ 0x100 + APURegisters.S);

#ifdef FAST_LSB_WORD_ACCESS
wrong !!
#define PushW(w)\
    *(uint16 *) (/*0 +*/ 0xff + APURegisters.S) = w;\
    APURegisters.S -= 2;
#define PopW(w)\
    APURegisters.S += 2;\
    w = *(uint16 *) (/*0 +*/ 0xff + APURegisters.S);
#else
#define PushW(w)\
  SET(/*0 +*/ 0xff + APURegisters.S, w);		\
  SET(/*0 +*/ 0x100 + APURegisters.S, w >> 8);	\
    APURegisters.S -= 2;
#define PopW(w)\
    APURegisters.S += 2; \
    (w) = GET(/*0 +*/ 0xff + APURegisters.S) + (GET(/*0 +*/ 0x100 + APURegisters.S) << 8);
#endif

#define Relative()\
  short Int8 = OP1; Int8 = ES(Int8);				\
  signed short Int16 = ((int) (IAPU.PC + 2 /*- 0*/) + Int8);
// #define Relative()\
//   signed char Int8 = /*ES*/(OP1);							\
//   signed short Int16 = Int8<0x80? ((int) (IAPU.PC + 2 /*- 0*/) + Int8) : ((int) (IAPU.PC + 2 /*- 0*/) + (Int8 - 0x100));

#define Relative2()\
  short Int8 = OP2; Int8 = ES(Int8);				\
  signed short Int16 = ((int) (IAPU.PC + 3 /*- 0*/) + Int8);
// #define Relative2()\
//   signed char Int8 = /*ES*/(OP2);							\
//   signed short Int16 = Int8<0x80? ((int) (IAPU.PC + 3 /*- 0*/) + Int8) : ((int) (IAPU.PC + 3 /*- 0*/) + (Int8 - 0x100));

//  Int16 = (int) (IAPU.PC + 3 /*- 0*/) + Int8;

#ifdef FAST_LSB_WORD_ACCESS
wrong !!
#define IndexedXIndirect()\
    IAPU.Address = *(uint16 *) (IAPU.DirectPage + ((OP1 + APURegisters.X) & 0xff));

#define Absolute()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1);

#define AbsoluteX()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1) + APURegisters.X;

#define AbsoluteY()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1) + APURegisters.YA.B.Y;

#define MemBit()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1);\
    IAPU.Bit = (IAPU.Address >> 13);\
    IAPU.Address &= 0x1fff;

#define IndirectIndexedY()\
    IAPU.Address = *(uint16 *) (IAPU.DirectPage + OP1) + APURegisters.YA.B.Y;
#else
#define IndexedXIndirect()\
  {uint8 temp = OP1;							\
    IAPU.Address = GET(IAPU.DirectPage + ((temp + APURegisters.X) & 0xff)) + \
      (GET(IAPU.DirectPage + ((temp + APURegisters.X + 1) & 0xff)) << 8); }
#define Absolute()\
    IAPU.Address = OP1 + (OP2 << 8);

#define AbsoluteX()\
    IAPU.Address = OP1 + (OP2 << 8) + APURegisters.X;

#define AbsoluteY()\
    IAPU.Address = OP1 + (OP2 << 8) + APURegisters.YA.B.Y;

#define MemBit()\
    IAPU.Address = OP1 + (OP2 << 8);\
    IAPU.Bit = (int8) (IAPU.Address >> 13);\
    IAPU.Address &= 0x1fff;

#define IndirectIndexedY()\
  {uint8 temp = OP1;				 \
    IAPU.Address = GET(IAPU.DirectPage + temp) +	   \
      (GET(IAPU.DirectPage + temp + 1) << 8) +		   \
      APURegisters.YA.B.Y; }
#endif

void Apu00 ()
{
// NOP
    IAPU.PC++;
}

#ifdef SOPT
#define Apu11 Apu01
#define Apu21 Apu01
#define Apu31 Apu01
#define Apu41 Apu01
#define Apu51 Apu01
#define Apu61 Apu01
#define Apu71 Apu01
#define Apu81 Apu01
#define Apu91 Apu01
#define ApuA1 Apu01
#define ApuB1 Apu01
#define ApuC1 Apu01
#define ApuD1 Apu01
#define ApuE1 Apu01
#define ApuF1 Apu01
void Apu01 () { TCALL ((OP0>>4)) }
#else
#endif

void Apu3F () // CALL absolute
{
    Absolute ();
    PushW (IAPU.PC + 3 /*- 0*/);
    IAPU.PC = /*0 +*/ IAPU.Address;
}

void Apu4F () // PCALL $XX
{
    uint8 Work8 = OP1;
    PushW (IAPU.PC + 2 /*- 0*/);
    IAPU.PC = /*0 +*/ 0xff00 + Work8;
}

#define MSET(b) \
uint8 temp = OP1; \
S9xAPUSetByteZ (S9xAPUGetByteZ (temp ) | (1 << (b)), temp); \
IAPU.PC += 2

#ifdef SOPT
#define Apu22 Apu02
#define Apu42 Apu02
#define Apu62 Apu02
#define Apu82 Apu02
#define ApuA2 Apu02
#define ApuC2 Apu02
#define ApuE2 Apu02
void Apu02 ()
{
  MSET ((OP0>>5));
}
#else
#endif

#define CLR(b) \
uint8 temp = OP1; \
S9xAPUSetByteZ (S9xAPUGetByteZ (temp) & ~(1 << (b)), temp);	\
IAPU.PC += 2;

#ifdef SOPT
#define Apu32 Apu12
#define Apu52 Apu12
#define Apu72 Apu12
#define Apu92 Apu12
#define ApuB2 Apu12
#define ApuD2 Apu12
#define ApuF2 Apu12
void Apu12 ()
{
  CLR ((OP0>>5));
}
#else
#endif

#define BBS(b) \
  {\
uint8 Work8 = OP1;					\
if (S9xAPUGetByteZ (Work8) & (1 << (b))) \
{ \
    Relative2 (); \
    IAPU.PC = /*0 +*/ (uint16) Int16; \
    APU.Cycles += IAPU.TwoCycles; \
} \
else \
     IAPU.PC += 3; \
  }		   \

#ifdef SOPT
#define Apu23 Apu03
#define Apu43 Apu03
#define Apu63 Apu03
#define Apu83 Apu03
#define ApuA3 Apu03
#define ApuC3 Apu03
#define ApuE3 Apu03
void Apu03 ()
{
  BBS (OP0>>5);
}
#else
#endif

#define BBC(b) \
  { uint8 Work8 = OP1;					\
if (!(S9xAPUGetByteZ (Work8) & (1 << (b)))) \
{ \
    Relative2 (); \
    IAPU.PC = /*0 +*/ (uint16) Int16; \
    APU.Cycles += IAPU.TwoCycles; \
} \
else \
     IAPU.PC += 3; \
  }

#ifdef SOPT
#define Apu33 Apu13
#define Apu53 Apu13
#define Apu73 Apu13
#define Apu93 Apu13
#define ApuB3 Apu13
#define ApuD3 Apu13
#define ApuF3 Apu13
void Apu13 ()
{
  BBC ((OP0>>5));
}
#else
#endif

static uint8 op2;
static uint8 gop(int mode, uint8 * opb)
{
  switch(mode) {
  case 4:
    *opb = S9xAPUGetByteZ (OP1);
    IAPU.PC += 2;
    break;
  case 5:
    Absolute ();
    *opb = S9xAPUGetByte (IAPU.Address);
    IAPU.PC += 3;
    break;
  case 6:
    *opb = S9xAPUGetByteZ (APURegisters.X);
    IAPU.PC++;
    break;
  case 7:
    IndexedXIndirect ();
    *opb = S9xAPUGetByte (IAPU.Address);
    IAPU.PC += 2;
    break;
  case 8:
    *opb = OP1;
    IAPU.PC += 2;
    break;
  case 9:
    *opb = S9xAPUGetByteZ (OP1);
    op2 = OP2;
    return S9xAPUGetByteZ (op2);
  case 0x14:
    *opb = S9xAPUGetByteZ (OP1 + APURegisters.X);
    IAPU.PC += 2;
    break;
  case 0x15:
    AbsoluteX ();
    *opb = S9xAPUGetByte (IAPU.Address);
    IAPU.PC += 3;
    break;
  case 0x16:
    AbsoluteY ();
    *opb = S9xAPUGetByte (IAPU.Address);
    IAPU.PC += 3;
    break;
  case 0x17:
    IndirectIndexedY ();
    *opb = S9xAPUGetByte (IAPU.Address);
    IAPU.PC += 2;
    break;
  case 0x18:
    *opb = OP1;
    op2 = OP2;
    return S9xAPUGetByteZ (op2);
  default:
    //case 0x19:
    *opb = S9xAPUGetByteZ (APURegisters.YA.B.Y);
    IAPU.PC++;
    return S9xAPUGetByteZ (APURegisters.X);
//   default:
//     printf("!!!\n");
  }
  return APURegisters.YA.B.A;
}

static void sop(int mode, int8 Work8)
{
  switch(mode) {
  case 9:
    S9xAPUSetByteZ (Work8, op2);
    IAPU.PC += 3;
    break;
  case 0x18:
    S9xAPUSetByteZ (Work8, op2);
    IAPU.PC += 3;
    break;
  case 0x19:
    S9xAPUSetByteZ (Work8, APURegisters.X);
    break;
//   default:
//     printf("???\n");
  default:
//   case 4:
//   case 5:
//   case 6:
//   case 7:
//   case 8:
//   case 0x14:
//   case 0x15:
//   case 0x16:
//   case 0x17:
    APURegisters.YA.B.A = Work8;
    break;
  }
  APUSetZN8 (Work8);
}

#ifdef SOPT
#define Apu05 Apu04
#define Apu06 Apu04
#define Apu07 Apu04
#define Apu08 Apu04
#define Apu09 Apu04
#define Apu14 Apu04
#define Apu15 Apu04
#define Apu16 Apu04
#define Apu17 Apu04
#define Apu18 Apu04
#define Apu19 Apu04
void Apu04 ()
{
// OR
  uint8 a, b;
  int mode = OP0;
  a = gop(mode, &b);
  sop(mode, a|b);
}
#else
#endif

void Apu0A ()
{
// OR1 C,membit
    MemBit ();
    if (!APUCheckCarry ())
    {
	if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
	    APUSetCarry ();
    }
    IAPU.PC += 3;
}

void Apu2A ()
{
// OR1 C,not membit
    MemBit ();
    if (!APUCheckCarry ())
    {
	if (!(S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit)))
	    APUSetCarry ();
    }
    IAPU.PC += 3;
}

void Apu4A ()
{
// AND1 C,membit
    MemBit ();
    if (APUCheckCarry ())
    {
	if (!(S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit)))
	    APUClearCarry ();
    }
    IAPU.PC += 3;
}

void Apu6A ()
{
// AND1 C, not membit
    MemBit ();
    if (APUCheckCarry ())
    {
	if ((S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit)))
	    APUClearCarry ();
    }
    IAPU.PC += 3;
}

void Apu8A ()
{
// EOR1 C, membit
    MemBit ();
    if (APUCheckCarry ())
    {
	if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
	    APUClearCarry ();
    }
    else
    {
	if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
	    APUSetCarry ();
    }
    IAPU.PC += 3;
}

void ApuAA ()
{
// MOV1 C,membit
    MemBit ();
    if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
	APUSetCarry ();
    else
	APUClearCarry ();
    IAPU.PC += 3;
}

void ApuCA ()
{
// MOV1 membit,C
    MemBit ();
    if (APUCheckCarry ())
    {
	S9xAPUSetByte (S9xAPUGetByte (IAPU.Address) | (1 << IAPU.Bit), IAPU.Address);
    }
    else
    {
	S9xAPUSetByte (S9xAPUGetByte (IAPU.Address) & ~(1 << IAPU.Bit), IAPU.Address);
    }
    IAPU.PC += 3;
}

void ApuEA ()
{
// NOT1 membit
    MemBit ();
    S9xAPUSetByte (S9xAPUGetByte (IAPU.Address) ^ (1 << IAPU.Bit), IAPU.Address);
    IAPU.PC += 3;
}

void Apu0B ()
{
// ASL dp
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1);
    ASL (Work8);
    S9xAPUSetByteZ (Work8, op1);
    IAPU.PC += 2;
}

void Apu0C ()
{
// ASL abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    ASL (Work8);
    S9xAPUSetByte (Work8, IAPU.Address);
    IAPU.PC += 3;
}

void Apu1B ()
{
// ASL dp+X
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1 + APURegisters.X);
    ASL (Work8);
    S9xAPUSetByteZ (Work8, op1 + APURegisters.X);
    IAPU.PC += 2;
}

void Apu1C ()
{
// ASL A
    ASL (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu0D ()
{
// PUSH PSW
    S9xAPUPackStatus ();
    Push (APURegisters.P);
    IAPU.PC++;
}

void Apu2D ()
{
// PUSH A
    Push (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu4D ()
{
// PUSH X
    Push (APURegisters.X);
    IAPU.PC++;
}

void Apu6D ()
{
// PUSH Y
    Push (APURegisters.YA.B.Y);
    IAPU.PC++;
}

void Apu8E ()
{
// POP PSW
    Pop (APURegisters.P);
    S9xAPUUnpackStatus ();
    if (APUCheckDirectPage ())
	IAPU.DirectPage = /*0 +*/ 0x100;
    else
	IAPU.DirectPage = 0;
    IAPU.PC++;
}

void ApuAE ()
{
// POP A
    Pop (APURegisters.YA.B.A);
    IAPU.PC++;
}

void ApuCE ()
{
// POP X
    Pop (APURegisters.X);
    IAPU.PC++;
}

void ApuEE ()
{
// POP Y
    Pop (APURegisters.YA.B.Y);
    IAPU.PC++;
}

void Apu0E ()
{
// TSET1 abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    S9xAPUSetByte (Work8 | APURegisters.YA.B.A, IAPU.Address);
    Work8 &= APURegisters.YA.B.A;
    APUSetZN8 (Work8);
    IAPU.PC += 3;
}

void Apu4E ()
{
// TCLR1 abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    S9xAPUSetByte (Work8 & ~APURegisters.YA.B.A, IAPU.Address);
    Work8 &= APURegisters.YA.B.A;
    APUSetZN8 (Work8);
    IAPU.PC += 3;
}

void Apu0F ()
{
// BRK

#if 0
    STOP ("BRK");
#else
    PushW (IAPU.PC + 1 /*- 0*/);
    S9xAPUPackStatus ();
    Push (APURegisters.P);
    APUSetBreak ();
    APUClearInterrupt ();
// XXX:Where is the BRK vector ???
    IAPU.PC = /*0 +*/ APU.ExtraRAM[0x20] + (APU.ExtraRAM[0x21] << 8);
#endif
}

void ApuEF ()
{
// SLEEP
    // XXX: sleep
    // STOP ("SLEEP");
    IAPU.APUExecuting = FALSE;
    IAPU.PC++;
}

void ApuFF ()
{
// STOP
    // STOP ("STOP");
    IAPU.APUExecuting = FALSE;
    IAPU.PC++;
}

void Apu10 ()
{
// BPL
    if (!APUCheckNegative ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 2;
}

void Apu30 ()
{
// BMI
    if (APUCheckNegative ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 2;
}

void Apu90 ()
{
// BCC
    if (!APUCheckCarry ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 2;
}

void ApuB0 ()
{
// BCS
    if (APUCheckCarry ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 2;
}

void ApuD0 ()
{
// BNE
    if (!APUCheckZero ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 2;
}

void ApuF0 ()
{
// BEQ
    if (APUCheckZero ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 2;
}

void Apu50 ()
{
// BVC
    if (!APUCheckOverflow ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
    }
    else
	IAPU.PC += 2;
}

void Apu70 ()
{
// BVS
    if (APUCheckOverflow ())
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
    }
    else
	IAPU.PC += 2;
}

void Apu2F ()
{
// BRA
    Relative ();
    IAPU.PC = /*0 +*/ (uint16) Int16;
}

void Apu80 ()
{
// SETC
    APUSetCarry ();
    IAPU.PC++;
}

void ApuED ()
{
// NOTC
    IAPU._Carry ^= 1;
    IAPU.PC++;
}

void Apu40 ()
{
// SETP
    APUSetDirectPage ();
    IAPU.DirectPage = /*0 +*/ 0x100;
    IAPU.PC++;
}

void Apu1A ()
{
// DECW dp
    uint8 op1 = OP1;
    uint16 Work16 = S9xAPUGetByteZ (op1) + (S9xAPUGetByteZ (op1 + 1) << 8);
    Work16--;
    S9xAPUSetByteZ (Work16&0xff, op1);
    S9xAPUSetByteZ (Work16 >> 8, op1 + 1);
    APUSetZN16 (Work16);
    IAPU.PC += 2;
}

void Apu5A ()
{
// CMPW YA,dp
    uint8 op1 = OP1;
    uint16 Work16 = S9xAPUGetByteZ (op1) + (S9xAPUGetByteZ (op1 + 1) << 8);
    long Int32 = (long) GETW - (long) Work16;
    IAPU._Carry = Int32 >= 0;
    APUSetZN16 ((uint16) Int32);
//     IAPU._Carry = ( ((short)GETW) >= Work16 ); // VP ?
//     APUSetZN16 (GETW - Work16);
    IAPU.PC += 2;
}

void Apu3A ()
{
// INCW dp
    uint8 op1 = OP1;
    uint16 Work16 = S9xAPUGetByteZ (op1) + (S9xAPUGetByteZ (op1 + 1) << 8);
    Work16++;
    S9xAPUSetByteZ (Work16&0xff, op1);
    S9xAPUSetByteZ (Work16 >> 8, op1 + 1);
    APUSetZN16 (Work16);
    IAPU.PC += 2;
}

void Apu7A ()
{
// ADDW YA,dp
    uint8 op1 = OP1;
    uint16 Work16 = S9xAPUGetByteZ (op1) + (S9xAPUGetByteZ (op1 + 1) << 8);
    uint32 Work32 = (uint32) GETW + Work16;
    APUClearHalfCarry ();
    IAPU._Carry = Work32 >= 0x10000;
    if (~(GETW ^ Work16) & (Work16 ^ (uint16) Work32) & 0x8000)
	APUSetOverflow ();
    else
	APUClearOverflow ();
    Work16 = (uint16) Work32;
    SETW(Work16);
    APUSetZN16 (Work16);
    IAPU.PC += 2;
}

void Apu9A ()
{
// SUBW YA,dp
    uint8 op1 = OP1;
    uint16 Work16 = S9xAPUGetByteZ (op1) + (S9xAPUGetByteZ (op1 + 1) << 8);
    long Int32 = (long) GETW - (long) Work16;
    APUClearHalfCarry ();
    IAPU._Carry = Int32 >= 0;
    if (((GETW ^ Work16) & 0x8000) &&
	    ((GETW ^ (uint16) Int32) & 0x8000))
	APUSetOverflow ();
    else
	APUClearOverflow ();
    if (((GETW ^ Work16) & 0x0080) &&
	    ((GETW ^ (uint16) Int32) & 0x0080))
	APUSetHalfCarry ();
    Work16 = (uint16)Int32;
    SETW(Work16);
    APUSetZN16 (Work16);
    IAPU.PC += 2;
}

void ApuBA ()
{
// MOVW YA,dp
    uint8 op1 = OP1;
    APURegisters.YA.B.A = S9xAPUGetByteZ (op1);
    APURegisters.YA.B.Y = S9xAPUGetByteZ (op1 + 1);
    APUSetZN16 (GETW);
    IAPU.PC += 2;
}

void ApuDA ()
{
// MOVW dp,YA
    uint8 op1 = OP1;
    S9xAPUSetByteZ (APURegisters.YA.B.A, op1);
    S9xAPUSetByteZ (APURegisters.YA.B.Y, op1 + 1);
    IAPU.PC += 2;
}

#ifdef SOPT
#define Apu65 Apu64
#define Apu66 Apu64
#define Apu67 Apu64
#define Apu68 Apu64
#define Apu69 Apu64
#define Apu74 Apu64
#define Apu75 Apu64
#define Apu76 Apu64
#define Apu77 Apu64
#define Apu78 Apu64
#define Apu79 Apu64
void Apu64 ()
{
  uint8 a, b;
  int mode = OP0&0x1f;
  a = gop(mode, &b);
  CMP(a, b);
  if (mode == 0x9 || mode == 0x18)
    IAPU.PC += 3;
}
#else
#endif

void Apu1E ()
{
// CMP X,abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    CMP (APURegisters.X, Work8);
    IAPU.PC += 3;
}

void Apu3E ()
{
// CMP X,dp
    uint8 Work8 = S9xAPUGetByteZ (OP1);
    CMP (APURegisters.X, Work8);
    IAPU.PC += 2;
}

void ApuC8 ()
{
// CMP X,#00
    CMP (APURegisters.X, OP1);
    IAPU.PC += 2;
}

void Apu5E ()
{
// CMP Y,abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    CMP (APURegisters.YA.B.Y, Work8);
    IAPU.PC += 3;
}

void Apu7E ()
{
// CMP Y,dp
    uint8 Work8 = S9xAPUGetByteZ (OP1);
    CMP (APURegisters.YA.B.Y, Work8);
    IAPU.PC += 2;
}

void ApuAD ()
{
// CMP Y,#00
    uint8 Work8 = OP1;
    CMP (APURegisters.YA.B.Y, Work8);
    IAPU.PC += 2;
}

void Apu1F ()
{
// JMP (abs+X)
    Absolute ();
    IAPU.PC = /*0 +*/ S9xAPUGetByte (IAPU.Address + APURegisters.X) +
	(S9xAPUGetByte (IAPU.Address + APURegisters.X + 1) << 8);
// XXX: HERE:
    // APU.Flags |= TRACE_FLAG;
}

void Apu5F ()
{
// JMP abs
    Absolute ();
    IAPU.PC = /*0 +*/ IAPU.Address;
}

void Apu20 ()
{
// CLRP
    APUClearDirectPage ();
    IAPU.DirectPage = 0;
    IAPU.PC++;
}

void Apu60 ()
{
// CLRC
    APUClearCarry ();
    IAPU.PC++;
}

void ApuE0 ()
{
// CLRV
    APUClearHalfCarry ();
    APUClearOverflow ();
    IAPU.PC++;
}

#ifdef SOPT
#define Apu25 Apu24
#define Apu26 Apu24
#define Apu27 Apu24
#define Apu28 Apu24
#define Apu29 Apu24
#define Apu34 Apu24
#define Apu35 Apu24
#define Apu36 Apu24
#define Apu37 Apu24
#define Apu38 Apu24
#define Apu39 Apu24
void Apu24 ()
{
  uint8 a, b;
  int mode = OP0&0x1f;
  a = gop(mode, &b);
  sop(mode, a&b);
}
#else
#endif

void Apu2B ()
{
// ROL dp
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1);
    ROL (Work8);
    S9xAPUSetByteZ (Work8, op1);
    IAPU.PC += 2;
}

void Apu2C ()
{
// ROL abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    ROL (Work8);
    S9xAPUSetByte (Work8, IAPU.Address);
    IAPU.PC += 3;
}

void Apu3B ()
{
// ROL dp+X
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1 + APURegisters.X);
    ROL (Work8);
    S9xAPUSetByteZ (Work8, op1 + APURegisters.X);
    IAPU.PC += 2;
}

void Apu3C ()
{
// ROL A
    ROL (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu2E ()
{
// CBNE dp,rel
    uint8 Work8 = OP1;
    
    if (S9xAPUGetByteZ (Work8) != APURegisters.YA.B.A)
    {
        Relative2 ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 3;
}

void ApuDE ()
{
// CBNE dp+X,rel
    uint8 Work8 = OP1 + APURegisters.X;

    if (S9xAPUGetByteZ (Work8) != APURegisters.YA.B.A)
    {
        Relative2 ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
	APUShutdown ();
    }
    else
	IAPU.PC += 3;
}

void Apu3D ()
{
// INC X
    APURegisters.X = (APURegisters.X+1)&0xff;
    APUSetZN8 (APURegisters.X);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC++;
}

void ApuFC ()
{
// INC Y
    APURegisters.YA.B.Y = (APURegisters.YA.B.Y+1)&0xff;
    APUSetZN8 (APURegisters.YA.B.Y);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC++;
}

void Apu1D ()
{
// DEC X
    APURegisters.X = (APURegisters.X-1)&0xff;
    APUSetZN8 (APURegisters.X);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC++;
}

void ApuDC ()
{
// DEC Y
    APURegisters.YA.B.Y = (APURegisters.YA.B.Y-1)&0xff;
    APUSetZN8 (APURegisters.YA.B.Y);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC++;
}

void ApuAB ()
{
// INC dp
    uint8 op1 = OP1;
    uint8 Work8 = (S9xAPUGetByteZ (op1) + 1)&0xff;
    S9xAPUSetByteZ (Work8, op1);
    APUSetZN8 (Work8);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC += 2;
}

void ApuAC ()
{
// INC abs
    Absolute ();
    uint8 Work8 = (S9xAPUGetByte (IAPU.Address) + 1)&0xff;
    S9xAPUSetByte (Work8, IAPU.Address);
    APUSetZN8 (Work8);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC += 3;
}

void ApuBB ()
{
// INC dp+X
    uint8 op1 = OP1;
    uint8 Work8 = (S9xAPUGetByteZ (op1 + APURegisters.X) + 1)&0xff;
    S9xAPUSetByteZ (Work8, op1 + APURegisters.X);
    APUSetZN8 (Work8);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC += 2;
}

void ApuBC ()
{
// INC A
  //APURegisters.YA.B.A++; // TOMASK
  APURegisters.YA.B.A = (APURegisters.YA.B.A+1)&0xff;
  APUSetZN8 (APURegisters.YA.B.A);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC++;
}

void Apu8B ()
{
// DEC dp
    uint8 op1 = OP1;
    uint8 Work8 = (S9xAPUGetByteZ (op1) - 1)&0xff;
    S9xAPUSetByteZ (Work8, op1);
    APUSetZN8 (Work8);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC += 2;
}

void Apu8C ()
{
// DEC abs
    Absolute ();
    uint8 Work8 = (S9xAPUGetByte (IAPU.Address) - 1)&0xff;
    S9xAPUSetByte (Work8, IAPU.Address);
    APUSetZN8 (Work8);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC += 3;
}

void Apu9B ()
{
// DEC dp+X
    uint8 op1 = OP1;
    uint8 Work8 = (S9xAPUGetByteZ (op1 + APURegisters.X) - 1)&0xff;
    S9xAPUSetByteZ (Work8, op1 + APURegisters.X);
    APUSetZN8 (Work8);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC += 2;
}

void Apu9C ()
{
// DEC A
  //APURegisters.YA.B.A--; // TOMASK
  APURegisters.YA.B.A = (APURegisters.YA.B.A-1)&0xff;
  APUSetZN8 (APURegisters.YA.B.A);

#ifdef SPC700_SHUTDOWN
    IAPU.WaitCounter++;
#endif

    IAPU.PC++;
}

#ifdef SOPT
#define Apu45 Apu44
#define Apu46 Apu44
#define Apu47 Apu44
#define Apu48 Apu44
#define Apu49 Apu44
#define Apu54 Apu44
#define Apu55 Apu44
#define Apu56 Apu44
#define Apu57 Apu44
#define Apu58 Apu44
#define Apu59 Apu44
void Apu44 ()
{
  uint8 a, b;
  int mode = OP0&0x1f;
  a = gop(mode, &b);
  sop(mode, (a^b)&0xff);
}
#else
#endif

void Apu4B ()
{
// LSR dp
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1);
    LSR (Work8);
    S9xAPUSetByteZ (Work8, op1);
    IAPU.PC += 2;
}

void Apu4C ()
{
// LSR abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    LSR (Work8);
    S9xAPUSetByte (Work8, IAPU.Address);
    IAPU.PC += 3;
}

void Apu5B ()
{
// LSR dp+X
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1 + APURegisters.X);
    LSR (Work8);
    S9xAPUSetByteZ (Work8, op1 + APURegisters.X);
    IAPU.PC += 2;
}

void Apu5C ()
{
// LSR A
    LSR (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu7D ()
{
// MOV A,X
  //APURegisters.YA.B.A = APURegisters.X; // TOMASK
  APURegisters.YA.B.A = APURegisters.X&0xff;
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

void ApuDD ()
{
// MOV A,Y
    APURegisters.YA.B.A = APURegisters.YA.B.Y;
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu5D ()
{
// MOV X,A
    APURegisters.X = APURegisters.YA.B.A;
    APUSetZN8 (APURegisters.X);
    IAPU.PC++;
}

void ApuFD ()
{
// MOV Y,A
    APURegisters.YA.B.Y = APURegisters.YA.B.A;
    APUSetZN8 (APURegisters.YA.B.Y);
    IAPU.PC++;
}

void Apu9D ()
{
//MOV X,SP
    APURegisters.X = APURegisters.S;
    APUSetZN8 (APURegisters.X);
    IAPU.PC++;
}

void ApuBD ()
{
// MOV SP,X
    APURegisters.S = APURegisters.X;
    IAPU.PC++;
}

void Apu6B ()
{
// ROR dp
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1);
    ROR (Work8);
    S9xAPUSetByteZ (Work8, op1);
    IAPU.PC += 2;
}

void Apu6C ()
{
// ROR abs
    Absolute ();
    uint8 Work8 = S9xAPUGetByte (IAPU.Address);
    ROR (Work8);
    S9xAPUSetByte (Work8, IAPU.Address);
    IAPU.PC += 3;
}

void Apu7B ()
{
// ROR dp+X
    uint8 op1 = OP1;
    uint8 Work8 = S9xAPUGetByteZ (op1 + APURegisters.X);
    ROR (Work8);
    S9xAPUSetByteZ (Work8, op1 + APURegisters.X);
    IAPU.PC += 2;
}

void Apu7C ()
{
// ROR A
    ROR (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu6E ()
{
// DBNZ dp,rel
    uint8 Work8 = OP1;
    uint8 W1 = (S9xAPUGetByteZ (Work8) - 1)&0xff;
    S9xAPUSetByteZ (W1, Work8);
    if (W1 != 0)
    {
        Relative2 ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
    }
    else
	IAPU.PC += 3;
}

void ApuFE ()
{
// DBNZ Y,rel
    APURegisters.YA.B.Y--;
    if (APURegisters.YA.B.Y != 0)
    {
        Relative ();
	IAPU.PC = /*0 +*/ (uint16) Int16;
	APU.Cycles += IAPU.TwoCycles;
    }
    else
	IAPU.PC += 2;
}

void Apu6F ()
{
// RET
    PopW (APURegisters.PC);
    IAPU.PC = /*0 +*/ APURegisters.PC;
}

void Apu7F ()
{
// RETI
    // STOP ("RETI");
    Pop (APURegisters.P);
    S9xAPUUnpackStatus ();
    PopW (APURegisters.PC);
    IAPU.PC = /*0 +*/ APURegisters.PC;
}

// HERE
#ifdef SOPT
#define Apu85 Apu84
#define Apu86 Apu84
#define Apu87 Apu84
#define Apu88 Apu84
#define Apu89 Apu84
#define Apu94 Apu84
#define Apu95 Apu84
#define Apu96 Apu84
#define Apu97 Apu84
#define Apu98 Apu84
#define Apu99 Apu84
void Apu84 ()
{
  uint8 a, b;
  int mode = OP0&0x1f;
  a = gop(mode, &b);
  ADC(a, b);
  sop(mode, a&0xff);
}
#else
#endif

void Apu8D ()
{
// MOV Y,#00
    APURegisters.YA.B.Y = OP1;
    APUSetZN8 (APURegisters.YA.B.Y);
    IAPU.PC += 2;
}

void Apu8F ()
{
// MOV dp,#00
    uint8 Work8 = OP1;
    S9xAPUSetByteZ (Work8, OP2);
    IAPU.PC += 3;
}

void Apu9E ()
{
// DIV YA,X
    if (APURegisters.X == 0)
    {
	APUSetOverflow ();
	APURegisters.YA.B.Y = 0xff;
	APURegisters.YA.B.A = 0xff;
    }
    else
    {
	APUClearOverflow ();
	uint8 Work8 = (GETW / APURegisters.X)&0xff;
	APURegisters.YA.B.Y = (GETW % APURegisters.X)&0xff;
	APURegisters.YA.B.A = Work8;
    }
// XXX How should Overflow, Half Carry, Zero and Negative flags be set??
    // APUSetZN16 (GETW);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

void Apu9F ()
{
// XCN A
    APURegisters.YA.B.A = ((APURegisters.YA.B.A >> 4) | (APURegisters.YA.B.A << 4))&0xff;
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

#ifdef SOPT
#define ApuA5 ApuA4
#define ApuA6 ApuA4
#define ApuA7 ApuA4
#define ApuA8 ApuA4
#define ApuA9 ApuA4
#define ApuB4 ApuA4
#define ApuB5 ApuA4
#define ApuB6 ApuA4
#define ApuB7 ApuA4
#define ApuB8 ApuA4
#define ApuB9 ApuA4
void ApuA4 ()
{
  uint8 a, b;
  int mode = OP0&0x1f;
  a = gop(mode, &b);
  SBC(a, b);
  sop(mode, a&0xff);
}
#else
#endif

void ApuAF ()
{
// MOV (X)+, A
    S9xAPUSetByteZ (APURegisters.YA.B.A, APURegisters.X++);
    IAPU.PC++;
}

void ApuBE ()
{
// DAS
    // XXX:
    IAPU.PC++;
}

void ApuBF ()
{
// MOV A,(X)+
    APURegisters.YA.B.A = S9xAPUGetByteZ (APURegisters.X++);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

void ApuC0 ()
{
// DI
    APUClearInterrupt ();
    IAPU.PC++;
}

void ApuA0 ()
{
// EI
    APUSetInterrupt ();
    IAPU.PC++;
}

void ApuC4 ()
{
// MOV dp,A
    S9xAPUSetByteZ (APURegisters.YA.B.A, OP1);
    IAPU.PC += 2;
}

void ApuC5 ()
{
// MOV abs,A
    Absolute ();
    S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
    IAPU.PC += 3;
}

void ApuC6 ()
{
// MOV (X), A
    S9xAPUSetByteZ (APURegisters.YA.B.A, APURegisters.X);
    IAPU.PC++;
}

void ApuC7 ()
{
// MOV (dp+X),A
    IndexedXIndirect ();
    S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
    IAPU.PC += 2;
}

void ApuC9 ()
{
// MOV abs,X
    Absolute ();
    S9xAPUSetByte (APURegisters.X, IAPU.Address);
    IAPU.PC += 3;
}

void ApuCB ()
{
// MOV dp,Y
    S9xAPUSetByteZ (APURegisters.YA.B.Y, OP1);
    IAPU.PC += 2;
}

void ApuCC ()
{
// MOV abs,Y
    Absolute ();
    S9xAPUSetByte (APURegisters.YA.B.Y, IAPU.Address);
    IAPU.PC += 3;
}

void ApuCD ()
{
// MOV X,#00
    APURegisters.X = OP1;
    APUSetZN8 (APURegisters.X);
    IAPU.PC += 2;
}

void ApuCF ()
{
// MUL YA
    uint16 res = APURegisters.YA.B.A * APURegisters.YA.B.Y;
    SETW(res);
    APUSetZN16 (res);
    IAPU.PC++;
}

void ApuD4 ()
{
// MOV dp+X, A
    S9xAPUSetByteZ (APURegisters.YA.B.A, OP1 + APURegisters.X);
    IAPU.PC += 2;
}

void ApuD5 ()
{
// MOV abs+X,A
    AbsoluteX ();
    S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
    IAPU.PC += 3;
}

void ApuD6 ()
{
// MOV abs+Y,A
    AbsoluteY ();
    S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
    IAPU.PC += 3;
}

void ApuD7 ()
{
// MOV (dp)+Y,A
    IndirectIndexedY ();
    S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
    IAPU.PC += 2;
}

void ApuD8 ()
{
// MOV dp,X
    S9xAPUSetByteZ (APURegisters.X, OP1);
    IAPU.PC += 2;
}

void ApuD9 ()
{
// MOV dp+Y,X
    S9xAPUSetByteZ (APURegisters.X, OP1 + APURegisters.YA.B.Y);
    IAPU.PC += 2;
}

void ApuDB ()
{
// MOV dp+X,Y
    S9xAPUSetByteZ (APURegisters.YA.B.Y, OP1 + APURegisters.X);
    IAPU.PC += 2;
}

void ApuDF ()
{
// DAA
    uint8 W1 = APURegisters.YA.B.A & 0xf;
    uint8 W2 = APURegisters.YA.B.A >> 4;
    APUClearCarry ();
    if (W1 > 9)
    {
	W1 -= 10;
	W2++;
    }
    if (W2 > 9)
    {
	W2 -= 10;
	APUSetCarry ();
    }
    APURegisters.YA.B.A = W1 | (W2 << 4);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

void ApuE4 ()
{
// MOV A, dp
    APURegisters.YA.B.A = S9xAPUGetByteZ (OP1);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 2;
}

void ApuE5 ()
{
// MOV A,abs
    Absolute ();
    APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 3;
}

void ApuE6 ()
{
// MOV A,(X)
    APURegisters.YA.B.A = S9xAPUGetByteZ (APURegisters.X);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC++;
}

void ApuE7 ()
{
// MOV A,(dp+X)
    IndexedXIndirect ();
    APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 2;
}

void ApuE8 ()
{
// MOV A,#00
    APURegisters.YA.B.A = OP1;
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 2;
}

void ApuE9 ()
{
// MOV X, abs
    Absolute ();
    APURegisters.X = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.X);
    IAPU.PC += 3;
}

void ApuEB ()
{
// MOV Y,dp
    APURegisters.YA.B.Y = S9xAPUGetByteZ (OP1);
    APUSetZN8 (APURegisters.YA.B.Y);
    IAPU.PC += 2;
}

void ApuEC ()
{
// MOV Y,abs
    Absolute ();
    APURegisters.YA.B.Y = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.YA.B.Y);
    IAPU.PC += 3;
}

void ApuF4 ()
{
// MOV A, dp+X
    APURegisters.YA.B.A = S9xAPUGetByteZ (OP1 + APURegisters.X);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 2;
}

void ApuF5 ()
{
// MOV A, abs+X
    AbsoluteX ();
    APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 3;
}

void ApuF6 ()
{
// MOV A, abs+Y
    AbsoluteY ();
    APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 3;
}

void ApuF7 ()
{
// MOV A, (dp)+Y
    IndirectIndexedY ();
    APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
    APUSetZN8 (APURegisters.YA.B.A);
    IAPU.PC += 2;
}

void ApuF8 ()
{
// MOV X,dp
    APURegisters.X = S9xAPUGetByteZ (OP1);
    APUSetZN8 (APURegisters.X);
    IAPU.PC += 2;
}

void ApuF9 ()
{
// MOV X,dp+Y
    APURegisters.X = S9xAPUGetByteZ (OP1 + APURegisters.YA.B.Y);
    APUSetZN8 (APURegisters.X);
    IAPU.PC += 2;
}

void ApuFA ()
{
// MOV dp(dest),dp(src)
    S9xAPUSetByteZ (S9xAPUGetByteZ (OP1), OP2);
    IAPU.PC += 3;
}

void ApuFB ()
{
// MOV Y,dp+X
    APURegisters.YA.B.Y = S9xAPUGetByteZ (OP1 + APURegisters.X);
    APUSetZN8 (APURegisters.YA.B.Y);
    IAPU.PC += 2;
}

#ifdef NO_INLINE_SET_GET
#undef INLINE
#define INLINE
#include "apumem.h"
#endif

void (*S9xApuOpcodes[256]) (void) =
{
	Apu00, Apu01, Apu02, Apu03, Apu04, Apu05, Apu06, Apu07,
	Apu08, Apu09, Apu0A, Apu0B, Apu0C, Apu0D, Apu0E, Apu0F,
	Apu10, Apu11, Apu12, Apu13, Apu14, Apu15, Apu16, Apu17,
	Apu18, Apu19, Apu1A, Apu1B, Apu1C, Apu1D, Apu1E, Apu1F,
	Apu20, Apu21, Apu22, Apu23, Apu24, Apu25, Apu26, Apu27,
	Apu28, Apu29, Apu2A, Apu2B, Apu2C, Apu2D, Apu2E, Apu2F,
	Apu30, Apu31, Apu32, Apu33, Apu34, Apu35, Apu36, Apu37,
	Apu38, Apu39, Apu3A, Apu3B, Apu3C, Apu3D, Apu3E, Apu3F,
	Apu40, Apu41, Apu42, Apu43, Apu44, Apu45, Apu46, Apu47,
	Apu48, Apu49, Apu4A, Apu4B, Apu4C, Apu4D, Apu4E, Apu4F,
	Apu50, Apu51, Apu52, Apu53, Apu54, Apu55, Apu56, Apu57,
	Apu58, Apu59, Apu5A, Apu5B, Apu5C, Apu5D, Apu5E, Apu5F,
	Apu60, Apu61, Apu62, Apu63, Apu64, Apu65, Apu66, Apu67,
	Apu68, Apu69, Apu6A, Apu6B, Apu6C, Apu6D, Apu6E, Apu6F,
	Apu70, Apu71, Apu72, Apu73, Apu74, Apu75, Apu76, Apu77,
	Apu78, Apu79, Apu7A, Apu7B, Apu7C, Apu7D, Apu7E, Apu7F,
	Apu80, Apu81, Apu82, Apu83, Apu84, Apu85, Apu86, Apu87,
	Apu88, Apu89, Apu8A, Apu8B, Apu8C, Apu8D, Apu8E, Apu8F,
	Apu90, Apu91, Apu92, Apu93, Apu94, Apu95, Apu96, Apu97,
	Apu98, Apu99, Apu9A, Apu9B, Apu9C, Apu9D, Apu9E, Apu9F,
	ApuA0, ApuA1, ApuA2, ApuA3, ApuA4, ApuA5, ApuA6, ApuA7,
	ApuA8, ApuA9, ApuAA, ApuAB, ApuAC, ApuAD, ApuAE, ApuAF,
	ApuB0, ApuB1, ApuB2, ApuB3, ApuB4, ApuB5, ApuB6, ApuB7,
	ApuB8, ApuB9, ApuBA, ApuBB, ApuBC, ApuBD, ApuBE, ApuBF,
	ApuC0, ApuC1, ApuC2, ApuC3, ApuC4, ApuC5, ApuC6, ApuC7,
	ApuC8, ApuC9, ApuCA, ApuCB, ApuCC, ApuCD, ApuCE, ApuCF,
	ApuD0, ApuD1, ApuD2, ApuD3, ApuD4, ApuD5, ApuD6, ApuD7,
	ApuD8, ApuD9, ApuDA, ApuDB, ApuDC, ApuDD, ApuDE, ApuDF,
	ApuE0, ApuE1, ApuE2, ApuE3, ApuE4, ApuE5, ApuE6, ApuE7,
	ApuE8, ApuE9, ApuEA, ApuEB, ApuEC, ApuED, ApuEE, ApuEF,
	ApuF0, ApuF1, ApuF2, ApuF3, ApuF4, ApuF5, ApuF6, ApuF7,
	ApuF8, ApuF9, ApuFA, ApuFB, ApuFC, ApuFD, ApuFE, ApuFF
};
