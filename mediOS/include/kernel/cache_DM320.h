/*
*   include/kernel/cache_DM320.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CACHE_DM320_H
#define __CACHE_DM320_H

#include <kernel/cache.h>

#define CACHE_DISABLE() {                                  \
    asm("                              \n\
        mrc p15,0,r0,c1,c0,0           \n\
        ldr r1,=0xFFFFEFFB             \n\
        and r0,r0,r1                   \n\
        mcr p15,0,r0,c1,c0,0           \n\
    ":::"r0","r1");                                        \
}

#define CACHE_ENABLE(mode) {                               \
    if(mode&CACHE_CODE){                                   \
        asm("                              \n\
            mrc p15,0,r0,c1,c0,0           \n\
            ldr r1,=0x00001000             \n\
            orr r0,r0,r1                   \n\
            mcr p15,0,r0,c1,c0,0           \n\
        ":::"r0","r1");                                    \
    }                                                      \
    if(mode&CACHE_DATA){                                   \
        asm("                              \n\
            mrc p15,0,r0,c1,c0,0           \n\
            ldr r1,=0x00000004             \n\
            orr r0,r0,r1                   \n\
            mcr p15,0,r0,c1,c0,0           \n\
        ":::"r0","r1");                                    \
    }                                                      \
}

#define CACHE_INVALIDATE(mode) {                           \
    switch(mode){                                          \
        case CACHE_CODE:                                   \
            asm("                      \n\
                mov r0,#0x0            \n\
                mcr p15,0,r0,c7,c5,0   \n\
            ":::"r0");                                     \
            break;                                         \
        case CACHE_DATA:                                   \
            asm("                      \n\
                mov r0,#0x0            \n\
                mcr p15,0,r0,c7,c6,0   \n\
            ":::"r0");                                     \
            break;                                         \
        case CACHE_ALL:                                    \
            asm("                      \n\
                mov r0,#0x0            \n\
                mcr p15,0,r0,c7,c7,0   \n\
            ":::"r0");                                     \
            break;                                         \
    }                                                      \
}

#endif
