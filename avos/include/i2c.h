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

extern u32 i2cWrite(u32 device, u32 address, void * buffer, u32 count);
extern u32 i2cWriteRaw(u32 device, void * buffer, u32 count);
extern u32 i2cRead(u32 device, u32 address, void * buffer, u32 count);
extern u32 i2cOutb(u32 data);
extern u32 i2cInb();
extern u32 i2cGetAck();
extern void i2cStop();
extern void i2cStart();
extern void i2cAck();
extern void i2cAckEnd();

extern u32 i2cWriteA(u32 device, u32 address, void * buffer, u32 count);
extern u32 i2cWriteRawA(u32 device, void * buffer, u32 count);
extern u32 i2cReadA(u32 device, u32 address, void * buffer, u32 count);
extern u32 i2cOutbA(u32 data);
extern u32 i2cInbA();
extern u32 i2cGetAckA();
extern void i2cStopA();
extern void i2cStartA();
extern void i2cAckA();
extern void i2cAckEndA();
