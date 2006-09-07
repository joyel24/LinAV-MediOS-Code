/*
*   kernel/driver/cache.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/cache.h>

void cache_enable(int mode){
    CACHE_DISABLE();
    CACHE_ENABLE(mode);
}

void cache_invalidate(int mode){
    CACHE_DISABLE();
    CACHE_INVALIDATE(mode);
    CACHE_ENABLE(CACHE_ALL);
}

