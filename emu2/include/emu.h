/* 
*   emu.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef EMU_H
#define EMU_H

#include "emu_types.h"

//#define DEBUG_MODE
#define DEBUG_MODE_HW
//#define PRINTSTATE

#ifdef DEBUG_MODE
#define DEBUG printf
#define MKDEBUG sprintf
#else
#define DEBUG(...)
#define MKDEBUG(...)
#endif

#ifdef DEBUG_MODE_HW
#define DEBUG_HW printf
#else
#define DEBUG_HW
#endif

#endif
