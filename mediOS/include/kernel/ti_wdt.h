/*
*   kernel/kernel/ti_wdt.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __TI_WDT_H
#define __TI_WDT_H

void wdt_init(void);
void wdt_setup(int prescaler,int divider,bool doreset);
void wdt_stop(void);
void wdt_start(void);

void arch_wdt_init(void);

#endif
