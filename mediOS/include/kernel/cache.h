/*
*   include/kernel/cache.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CACHE_H
#define __CACHE_H

#include <sys_def/stddef.h>
#include <sys_def/types.h>

#define CACHE_NONE 0x0000
#define CACHE_CODE 0x0001
#define CACHE_DATA 0x0002
#define CACHE_ALL (CACHE_CODE|CACHE_DATA)

#if defined(DM270)
    #include <kernel/cache_DM270.h>
#elif defined(DM320)
    #include <kernel/cache_DM320.h>
#elif defined(DSC25) || defined(DSC21)
    // no cache on these chips
    #define CACHE_DISABLE() {}
    #define CACHE_ENABLE(mode) {}
    #define CACHE_INVALIDATE(mode) {}
#else
    #error NO CHIP defined
#endif

void cache_enable(int mode);
void cache_invalidate(int mode);

#endif
