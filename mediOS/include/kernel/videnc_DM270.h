/*
*   include/kernel/videnc_DM270.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __VIDENC_DM270_H
#define __VIDENC_DM270_H

#include "kernel/videnc.h"
#include "kernel/io.h"

#define VIDENC_BASE                      0x00030800

#define VIDENC_SETUP                     (VIDENC_BASE + 0x0000)

#define VIDENC_STP_MODE_PAL              0x8000
#define VIDENC_STP_MODE_NTSC             0x0000
#define VIDENC_STP_INTERLACE_ENABLE      0x0000
#define VIDENC_STP_INTERLACE_DISABLE     0x4000
#define VIDENC_STP_CHROMA_CUTOFF_1_5MHZ  0x0000
#define VIDENC_STP_CHROMA_CUTOFF_3MHZ    0x2000
#define VIDENC_STP_DAC_POWER_ENABLE      0x0040
#define VIDENC_STP_DAC_POWER_DISABLE     0x0000
#define VIDENC_STP_DAC_OUTPUT_ENABLE     0x0020
#define VIDENC_STP_DAC_OUTPUT_DISABLE    0x0000
#define VIDENC_STP_ANALOG_ENABLE         0x0004
#define VIDENC_STP_ANALOG_DISABLE        0x0000
#define VIDENC_STP_ENABLE                0x0001
#define VIDENC_STP_DISABLE               0x0000


#define VIDENC_SET_PARAMETERS(mode,interlace) {                                \
    switch(mode){                                                              \
        case VIDENC_MODE_LCD:                                                  \
            outw(VIDENC_STP_INTERLACE_DISABLE|VIDENC_STP_ENABLE,VIDENC_SETUP); \
            break;                                                             \
        case VIDENC_MODE_PAL:                                                  \
            outw(((interlace)?VIDENC_STP_INTERLACE_ENABLE:VIDENC_STP_INTERLACE_DISABLE)| \
                   VIDENC_STP_MODE_PAL|                                        \
                   VIDENC_STP_DAC_POWER_ENABLE|                                \
                   VIDENC_STP_DAC_OUTPUT_ENABLE|                               \
                   VIDENC_STP_ANALOG_ENABLE|                                   \
                   VIDENC_STP_ENABLE                                           \
                ,VIDENC_SETUP);                                                \
            break;                                                             \
        case VIDENC_MODE_NTSC:                                                 \
            outw(((interlace)?VIDENC_STP_INTERLACE_ENABLE:VIDENC_STP_INTERLACE_DISABLE)| \
                   VIDENC_STP_MODE_NTSC|                                       \
                   VIDENC_STP_DAC_POWER_ENABLE|                                \
                   VIDENC_STP_DAC_OUTPUT_ENABLE|                               \
                   VIDENC_STP_ANALOG_ENABLE|                                   \
                   VIDENC_STP_ENABLE                                           \
                ,VIDENC_SETUP);                                                \
            break;                                                             \
    }                                                                          \
}

#endif
