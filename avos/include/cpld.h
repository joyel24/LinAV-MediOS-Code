/* cpld.h
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/
#ifndef _CPLD_H
#define _CPLD_H 1

#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

#define CPLD_MODE_DEFAULT               0
#define CPLD_MODE_COMPACTFLASHCARD      1
#define CPLD_MODE_DVR                   4
#define CPLD_MODE_SMARTMEDIACARD        8
#define CPLD_MODE_MULTIMEDIACARD        9

extern void cpldSetModeA(u32 mode);

#endif

