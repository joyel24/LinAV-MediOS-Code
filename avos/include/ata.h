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

#define ATA_BSY 0x80
#define ATA_RDY 0x40
#define ATA_DF  0x20
#define ATA_DRQ 0x08
#define ATA_ERR 0x01

#define ATA_ERROR_NONE 0
#define ATA_ERROR_TIMEOUT -1

extern void ataSelectHDD();
extern void ataSelectMemoryCard();
extern void ataPowerUpHDD();
extern void ataPowerDownHDD();
extern u32 ataStatus();
extern u32 ataWaitForReady();
