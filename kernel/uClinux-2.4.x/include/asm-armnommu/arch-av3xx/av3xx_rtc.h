/*
* asm/arch/av3xx_rtc.h
*
* based on avos2 rtc.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#ifndef __ASM_ARCH_AV3XX_RTC_H
#define __ASM_ARCH_AV3XX_RTC_H

#include <asm/arch/av3xx_common.h>

int av3xx_rtc_init(void);
int av3xx_rtc_getTime(struct av_tm * valTime);
int av3xx_rtc_setTime(struct av_tm * newTime);

#endif
