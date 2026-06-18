/*
  Little John GP32
  File : unes_io.h
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003
*/
#ifndef __unes_io_h__
#define __unes_io_h__

#include "datatypes.h"

byte Op6502(register word A);
uint8 Rd6502(uint32 Addr);
void Wr6502(uint32 Addr, uint8 Value);

#endif
