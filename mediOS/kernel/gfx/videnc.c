/*
*   kernel/gfx/videnc.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/videnc.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/gio.h>
#include <kernel/osd.h>
#include <kernel/lcd.h>

void videnc_setup(int mode,bool interlaced){
    VIDENC_SET_PARAMETERS(mode,interlaced);

    GIO_DIRECTION(GIO_VID_OUT,GIO_OUT);

    switch(mode){
        case VIDENC_MODE_LCD:
            osd_setMainShift(SCREEN_LCD_SHIFT_X, SCREEN_LCD_SHIFT_Y);
            GIO_SET(GIO_VID_OUT);
            break;
        case VIDENC_MODE_PAL:
            osd_setMainShift(SCREEN_PAL_SHIFT_X, SCREEN_PAL_SHIFT_Y);
            GIO_CLEAR(GIO_VID_OUT);
            break;
        case VIDENC_MODE_NTSC:
            osd_setMainShift(SCREEN_NTSC_SHIFT_X, SCREEN_NTSC_SHIFT_Y);
            GIO_CLEAR(GIO_VID_OUT);
            break;
    }
}

