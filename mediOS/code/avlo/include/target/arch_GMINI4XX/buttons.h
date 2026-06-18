/*
*   include/kernel/target/arch_AV3XX/buttons.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __BUTTONS_ARCH_H
#define __BUTTONS_ARCH_H

#include <kernel/buttons.h>

#define BTMASK_UP            0x0001
#define BTMASK_DOWN          0x0002
#define BTMASK_LEFT          0x0004
#define BTMASK_RIGHT         0x0008
#define BTMASK_MENU1         0x0010
#define BTMASK_MENU2         0x0020
#define BTMASK_MENU3         0x0040
#define BTMASK_SQUARE        0x0080
#define BTMASK_CROSS         0x0100
#define BTMASK_ON            0x0200
#define BTMASK_OFF           0x0400

#define BUTTON_UP            0x0000
#define BUTTON_DOWN          0x0001
#define BUTTON_LEFT          0x0002
#define BUTTON_RIGHT         0x0003
#define BUTTON_MENU1         0x0004
#define BUTTON_MENU2         0x0005
#define BUTTON_MENU3         0x0006
#define BUTTON_SQUARE        0x0007
#define BUTTON_CROSS         0x0008
#define BUTTON_ON            0x0009
#define BUTTON_OFF           0x000a

#define NB_BUTTONS            11

#endif
