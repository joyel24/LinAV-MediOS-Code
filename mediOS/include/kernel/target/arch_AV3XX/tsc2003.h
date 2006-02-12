/* 
*   include/kernel/target/arch_AV3XX/tsc2003.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __TSC2003_H
#define __TSC2003_H

#define CMD_TEMP0                0x00
#define CMD_BAT0                 0x10
#define CMD_IN0                  0x20

#define CMD_TEMP1                0x40
#define CMD_BAT1                 0x50
#define CMD_IN1                  0x60

#define INTERNAL_ON              0x08
#define ADC_ON                   0x04

int tsc2003getVal(char cmdN);

#endif
