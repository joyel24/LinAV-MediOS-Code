/* terminal.h
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


#ifndef _TERMINAL_H_
#define _TERMINAL_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

extern void terminalPutchT( u32 chr );
extern void terminalPutsCT( char* string, u32 color );
extern void terminalPutsT( char* string );
extern void terminalSetColorFT( u32 color );
extern void terminalSetPosT( u32 x, u32 y );


#endif
