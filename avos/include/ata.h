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

extern void ataSelect(u32 val);
extern void ataSelectHDD();
extern void ataSelectMemoryCard();
extern void ataPowerUpHDD();
extern void ataPowerDownHDD();
extern void ataIdentify();
extern u32 ataStatus();
extern u32 ataWaitForReady();
extern u32 ataWaitForXfer();
extern void ataReadData(char *buffer, u32 numHalfWords);
extern void ataWriteData(char *buffer, u32 numHalfWords);
extern void ataRead(u32 lba, u32 count);
extern void ataWrite(u32 lba, u32 count);
extern u32 ataReadSectors(u32 lba, u32 count, char *buffer);
extern u32 ataWriteSectors(u32 lba, u32 count, char *buffer);
extern u32 ataIdentifyDevice(char *buffer);

extern void ataSelectA(u32 val);
extern void ataSelectHDDA();
extern void ataSelectMemoryCardA();
extern void ataPowerUpHDDA();
extern void ataPowerDownHDDA();
extern void ataIdentifyA();
extern u32 ataStatusA();
extern u32 ataWaitForReadyA();
extern u32 ataWaitForXferA();
extern void ataReadDataA(char *buffer, u32 numHalfWords);
extern void ataWriteDataA(char *buffer, u32 numHalfWords);
extern void ataReadA(u32 lba, u32 count);
extern void ataWriteA(u32 lba, u32 count);
extern u32 ataReadSectorsA(u32 lba, u32 count, char *buffer);
extern u32 ataWriteSectorsA(u32 lba, u32 count, char *buffer);
extern u32 ataIdentifyDeviceA(char *buffer);
