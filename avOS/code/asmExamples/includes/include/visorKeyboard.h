/* visorKeyboard.h
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


#ifndef _VISORKEYBOARD_H_
#define _VISORKEYBOARD_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

extern u32  visorConvertToAscii( u32 scan_code );

#endif
