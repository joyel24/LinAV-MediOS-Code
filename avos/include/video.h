/* video.h
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/
#ifndef _VIDEO_H
#define _VIDEO_H 1

#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

#define VIDEO_MODE_NONINTERLACED    0x4000
#define VIDEO_MODE_INTERLACED       0x0000
#define VIDEO_MODE_PAL              0x8000
#define VIDEO_MODE_NTSC             0x0000
               // Unknown flags - LCD = 71
               //                 VID = 7d

extern void videoLCDEnableA();
extern void videoLCDDisableA();
extern void videoSetBrightnessA(u32 v);
extern void videoSetContrastA(u32 v);
extern void videoSetModeA(u32 mode);

#endif

