/* rtc.h
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

struct tm {
    unsigned char tm_ms;
    unsigned char tm_sec;
    unsigned char tm_min;
    unsigned char tm_hour;
    unsigned char tm_mday;
    unsigned char tm_mon;
    unsigned char tm_year;
    unsigned char tm_wday;
    unsigned char tm_yday;
};

extern void rtcInit();
extern struct tm* rtcGetTime();
extern void rtcSetTime(struct tm newTime);
