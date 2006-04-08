/* 
*   apps/avboy/defs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
* 
*  Date:     18/10/2005
* Author:   GliGli

*  Modified by CjNr11 06/12/2005
*/

#ifndef __DEFS_H__
#define __DEFS_H__


#ifdef BUILD_STDALONE
#include <kernel/graphics.h>
#include <kernel/stdfs.h>
#include <kernel/malloc.h>

#include <kernel/buttons.h>
#include <kernel/timer.h>
#include <kernel/delay.h>
#else
#include <api.h>
#include <sys_def/malloc.h>
#endif

#include <sys_def/section_types.h>


#define IS_LITTLE_ENDIAN


#ifdef IS_LITTLE_ENDIAN
#define LO 0
#define HI 1
#else
#define LO 1
#define HI 0
#endif

typedef unsigned char byte;

typedef unsigned char un8;
typedef unsigned short un16;
typedef unsigned int un32;

typedef signed char n8;
typedef signed short n16;
typedef signed int n32;

typedef un16 word;
typedef word addr;

#endif
