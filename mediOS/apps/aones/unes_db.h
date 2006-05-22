#ifndef __unes_db_h__
#define __unes_db_h__

#include "datatypes.h"
#include "unes.h"


typedef struct
{
	uint32 crcall;
	uint32 crc;
	char title[128];
	uint8 header1;
	uint8 header2;
	uint8 prg_rom_nb;
	uint8 chr_rom_nb;
	char country[8];
	char publisher[128];
	char date[16];	
} nestoy_info;

uint8 db_getinfo(uint32 crc,uint32 crcall,nestoy_info *ninfo);
uint8 init_memdb(void);
void free_memdb(void);
uint8 dbmem_getinfo(uint32 crc,uint32 crcall,nestoy_info *ninfo);

#endif
