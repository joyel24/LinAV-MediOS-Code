/* 
*   include/kernel/rtc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __KERNEL_RTC_H
#define __KERNEL_RTC_H

#include <sys_def/time.h>
#include <kernel/errors.h>

#define RTC_MS       0
#define RTC_S        1
#define RTC_M        2
#define RTC_H        3
#define RTC_DW       4   /* day of week */
#define RTC_DM       5   /* day of month */
#define RTC_MO       6
#define RTC_Y        7

#define RTC_MK_MS(ini,new)       (new & 0xFF)
#define RTC_MK_S(ini,new)        (new & 0x7F)
#define RTC_MK_M(ini,new)        (new & 0x7F)
#define RTC_MK_H(ini,new)        ((ini & 0xC0) | (new & 0x3F))
#define RTC_MK_DW(ini,new)       (new & 0x07)
#define RTC_MK_DM(ini,new)       (new & 0x3F)
#define RTC_MK_MO(ini,new)       (new & 0x1F)
#define RTC_MK_Y(ini,new)        (new & 0xFF)


MED_RET_T init_rtc(void);
int getDayOfWeek(int day,int month,int year);
MED_RET_T rtc_getTime(struct med_tm * valTime);
MED_RET_T rtc_setTime(struct med_tm * newTime);

#endif
