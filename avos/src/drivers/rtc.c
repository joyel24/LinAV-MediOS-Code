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
static unsigned char tmBuffer[100];

struct tm rtcGetTime() {
    //int c = i2cRead(RTC_ADDR, 0, tmBuffer, 9);  //&tmStatic, 9);
    
    // TODO - Handle i2c errors...
    
    //return tmStatic;
}

void rtcSetTime(struct tm newTime) {
    int c = i2cWrite(RTC_ADDR, 0, &newTime, 9);
    
    // TODO - Handle i2c errors...

}
