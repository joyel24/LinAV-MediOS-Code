/* buttons.h
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

#define        BUTTONS_AV300_UP        0x0001
#define        BUTTONS_AV300_LEFT      0x0002
#define        BUTTONS_AV300_RIGHT     0x0010
#define        BUTTONS_AV300_DOWN      0x0020
#define        BUTTONS_AV300_MENU3     0x0040
#define        BUTTONS_AV300_MENU1     0x0100
#define        BUTTONS_AV300_MENU2     0x0200
#define        BUTTONS_AV300_JOYPRESS  0x0400
#define        BUTTONS_AV300_ON        0x1000
#define        BUTTONS_AV300_OFF       0x2000
#define        BUTTONS_AV300_ANY       0x3777
        
#define        BUTTONS_JBMM_UP         0x0080
#define        BUTTONS_JBMM_DOWN       0x0008
#define        BUTTONS_JBMM_LEFT       0x0001
#define        BUTTONS_JBMM_RIGHT      0x0004
#define        BUTTONS_JBMM_MENU1      0x0010
#define        BUTTONS_JBMM_MENU2      0x0020
#define        BUTTONS_JBMM_MENU3      0x0040
#define        BUTTONS_JBMM_PLAY       0x0002
#define        BUTTONS_JBMM_ANY        0x00ff

extern u32 buttonsGetStatusA();

