/*
*   include/kernel/videnc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __VIDENC_H
#define __VIDENC_H

#include <sys_def/stddef.h>
#include <sys_def/types.h>

#if defined(DM270)
    #include <kernel/videnc_DM270.h>
#elif defined(DM320)
    #include <kernel/videnc_DM320.h>
#elif defined(DSC25)
    #include <kernel/videnc_DSC25.h>
#elif defined(DSC21)
    #include <kernel/videnc_DSC21.h>
#else
    #error NO CHIP defined
#endif

#define VIDENC_MODE_LCD 0
#define VIDENC_MODE_PAL 1
#define VIDENC_MODE_NTSC 2

void videnc_setup(int mode,bool interlaced);

#endif
