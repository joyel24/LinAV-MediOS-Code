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
typedef unsigned long u32;

extern void ataGetStatusT();
extern void ataIdentifyT( char* buffer );
extern void ataInitT();
extern void ataReadLBAT( char* buffer, u32 lba, u32 n );
extern void ataWaitForBSYT();
extern void ataWaitForRDYT();
extern void ataWaitForStartOfXferT();
extern void batteryIsDCT();
extern void buttonsGetStatusT();
extern u32  colorPackYCbCrT( u32 Y, u32 Cr, u32 Cb );
extern void fatInitT( u32 lba );
extern void fatReadCT( u32 cluster, char* buffer );
extern void fatTraceCCT( u32 cluster );
extern void hex2T( char* dest, u32 value );
extern void hex4T( char* dest, u32 value );
extern void hex8T( char* dest, u32 value );
extern void i2cAckT();
extern void i2cInitT();
extern void i2cStartT();
extern void i2cStopT();
extern u32  lcdGetPixelT( u32 x, u32 y );
extern void lcdInitT();
extern void lcdPlotT( u32 x, u32 y, u32 c );
extern void lcdPutchT( u32 x, u32 y, u32 character, u32 color );
extern void lcdPutsT( u32 x, u32 y, char* string, u32 color );
extern void lcdRectfST( u32 x, u32 y, u32 width, u32 height, u32 color );
extern void lcdScrollUpT( u32 lines, u32 color );
extern void lcdSpriteT( u32 x, u32 y, u32 width, u32 height, char* data );
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
extern void terminalPutchT( u32 chr );
extern void terminalPutsCT( char* string, u32 color );
extern void terminalPutsT( char* string );
extern void terminalSetColorFT( u32 color );
extern void terminalSetPosT( u32 x, u32 y );
extern u32  uartGetchT();
extern void uartOutchT( u32 c );
extern void uartOutsT( char* string );
extern u32  usbIsConnectedT();
extern u32  visorConvertToAscii( u32 scan_code );
