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

#define        TIMERS_TMMD_STOP         0x00
#define        TIMERS_TMMD_ONESHOT      0x01
#define        TIMERS_TMMD_FREERUN      0x02

#define        TIMERS_TMCLK_ARM         0x00
#define        TIMERS_TMCLK_EXT         0x01

extern u32 timersGetValue(u32 timer);
extern void timersTrigger(u32 timer);
extern void timersConfig(u32 timer, u32 mode, u32 clockSelect, u32 divide, u32 maxVal);
