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

#define CMD_TEMP0                0x00
#define CMD_BAT0                 0x10
#define CMD_IN0                  0x20

#define CMD_TEMP1                0x40
#define CMD_BAT1                 0x50
#define CMD_IN1                  0x60

#define INTERNAL_ON              0x08
#define ADC_ON                   0x04

extern u32 powerGetStatus();
extern u32 powerIsDCConnected();

extern u32 powerGetStatusA(u32 cmd);
extern u32 powerIsDCConnectedA();
