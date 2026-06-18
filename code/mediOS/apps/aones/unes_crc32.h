#ifndef __unes_crc32_h__
#define __unes_crc32_h__

#include "datatypes.h"
#include "unes.h"

unsigned long CrcCalc(unsigned char *buf, int len);
unsigned long CrcCalca(unsigned char *buf, int len, unsigned long crc);
uint8 Rom_GetCrc( char *name,uint32 *s);

#endif
