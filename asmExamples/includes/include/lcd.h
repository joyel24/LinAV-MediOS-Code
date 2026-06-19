/* lcd.h
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


#ifndef _LCD_H_
#define _LCD_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

extern u32  lcdGetPixelT( u32 x, u32 y );
extern void lcdInitT();
extern void lcdPlotT( u32 x, u32 y, u32 c );
extern void lcdPutchT( u32 x, u32 y, u32 character, u32 color );
extern void lcdPutsT( u32 x, u32 y, char* string, u32 color );
extern void lcdRectfST( u32 x, u32 y, u32 width, u32 height, u32 color );
extern void lcdScrollUpT( u32 lines, u32 color );
extern void lcdSpriteT( u32 x, u32 y, u32 width, u32 height, char* data );
extern void lcdSetBgT(u32 bgColor);
extern void lcdSetCol(u32 fColor);
extern void lcdClsT();

extern u32 loadImg(char * buffer, u32 offset,u32 count);

#endif
