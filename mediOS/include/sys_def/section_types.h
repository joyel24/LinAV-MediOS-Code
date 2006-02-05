/* 
*   include/sys_def/section_types.h
*
*   mediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __SECTION_TYPES_H
#define __SECTION_TYPES_H

/* section definition */

#define __IRAM_CODE    __attribute__((section(".core")))
#define __IRAM_DATA    __attribute__((section(".cored")))

#define __SDRAM_CODE   __attribute__((section(".text")))
#define __SDRAM_DATA   __attribute__((section(".data")))

#endif
