// avOS - http://avos.sourceforge.net
// Copyright (c) 2003 by Jimmy Moore
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.
//
// Date:     04/01/2004
// Author:   By DoggerMoore

#include "i2c.h"
#include "rtc.h"

#define RTC_ADDR 0xd0

static struct tm tmStatic = {1,0,0,0,0,0,0,0,0};

struct tm rtcGetTime() {
    int c = i2cRead(RTC_ADDR, 0, &tmStatic, 9);
    
    // TODO - Handle i2c errors...
    
    return tmStatic;
}

void rtcSetTime(struct tm newTime) {
    int c = i2cWrite(RTC_ADDR, 0, &newTime, 9);

    // TODO - Handle i2c errors...

}

void rtcInit() {
    unsigned char cb[1];
    int c;
    
    c = i2cRead(RTC_ADDR, 0x0c, cb, 1);
    cb[0] &= ~0x40;                         // Make sure RTC is running!
    c = i2cWrite(RTC_ADDR, 0x0c, cb, 1);
    
    
    // TODO - Handle i2c errors...

}
