/* osd.h
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


#ifndef _OSD_H_
#define _OSD_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

extern void osdSetBack2ConfigT( u32 byte_width, u32 config );
extern void osdSetBack2OffsetT( u32 offset );
extern void osdSetBack2PositionT( u32 x, u32 y );
extern void osdSetBack2SizeT( u32 width, u32 height );
extern void osdSetBackConfigT( u32 byte_width, u32 config );
extern void osdSetBackOffsetT( u32 offset );
extern void osdSetBackPositionT( u32 x, u32 y );
extern void osdSetBackSizeT( u32 width, u32 height );
extern void osdSetCursor2PositionT( u32 x, u32 y );
extern void osdSetCursorPositionT( u32 x, u32 y );
extern void osdSetCursorSizeT( u32 width, u32 height );
extern void osdSetMain2ConfigT( u32 byte_width, u32 config );
extern void osdSetMain2OffsetT( u32 offset );
extern void osdSetMain2PositionT( u32 x, u32 y );
extern void osdSetMain2SizeT( u32 width, u32 height );
extern void osdSetMainConfigT( u32 byte_width, u32 config );
extern void osdSetMainOffsetT( u32 offset );
extern void osdSetMainPositionT( u32 x, u32 y );
extern void osdSetMainSizeT( u32 width, u32 height );
extern void osdSetScreenConfigT( u32 config );
extern void osdSetScreenPositionT( u32 x, u32 y );

#endif
