/* hex.h
	By Christophe THOMAS (aka oxygen)
   Copyright 2004, the Avos project.
   fev 2004

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/


#ifndef _HEX_H_
#define _HEX_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

extern void hex2T( char* dest, u32 value );
extern void hex4T( char* dest, u32 value );
extern void hex8T( char* dest, u32 value );
extern u32 read2T(char * var);
extern u32 read4T(char * var);

#endif
