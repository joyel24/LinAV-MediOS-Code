/*
*   include/kernel/target/arch_XXX/sound.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SOUND_ARCH_H
#define __SOUND_ARCH_H

#include <kernel/cpld.h>

#define SPCKR_ON()  {CPLD_CLEAR_PORT3(CPLD_SPKR);}
#define SPCKR_OFF() {CPLD_SET_PORT3(CPLD_SPKR);}

#endif
