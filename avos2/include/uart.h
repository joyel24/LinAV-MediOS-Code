/* uart.h
   By Doggermoore
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

#define UART_0  0
#define UART_1  1

#define UART_BITRATE_9600       0x15f   //
#define UART_BITRATE_115200     0x1c    //

extern u32 uartGetchA(u32 uart);
extern void uartOutchA(u32 uart, u32 data);
extern void uartOutsA(u32 uart, char* string);

extern void stringPutHexA(char *buffer, u32 value, u32 num);

/*
30300 0000  1C00  0080  0000  1003  E000  0000
30380 0000  5F01  0080  0000  0003  E000  0304
*/
