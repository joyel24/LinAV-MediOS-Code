/* libavos.h
   By William Bland (aka awksedgrep)
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/
#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

extern void masReset();
extern u32 masGetVersion();
extern u32 masReadReg(u32 reg);
extern u32 masWriteReg(u32 reg, u32 val);
extern u32 masReadCodecReg(u32 reg);
extern u32 masWriteCodecReg(u32 reg, u32 val);

extern void masResetA();
extern u32 masGetVersionA();
extern u32 masReadRegA(u32 reg);
extern u32 masWriteRegA(u32 reg, u32 val);
extern u32 masReadCodecRegA(u32 reg);
extern u32 masWriteCodecRegA(u32 reg, u32 val);
extern u32 masReadD0(u32 addr, void* buff, u32 size);
extern u32 masReadD1(u32 addr, void* buff, u32 size);
extern u32 masWriteD0(u32 addr, void* buff, u32 size);
extern u32 masWriteD1(u32 addr, void* buff, u32 size);
extern u32 masControlWriteA(u32 val);

extern u32 masReadData(u32 ptr, u32 maxCount);
extern u32 masWriteData(u32 ptr, u32 maxCount);
