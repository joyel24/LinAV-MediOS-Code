/*
*   include/kernel/resize.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __RESIZE_H
#define __RESIZE_H

#include <sys_def/stddef.h>
#include <sys_def/types.h>

#if defined(DM270)
    #define RESIZE_INVERTVD() {outw(inw(CCDC_SETUP)^CCDC_STP_VDSYNC_NEGATIVE,CCDC_SETUP);}
    #define RESIZE_INVERTCCDCLK() {outw(inw(CLKC_SOURCE_SELECT)^CLKC_SRC_CCD_INVERT,CLKC_SOURCE_SELECT);}

    #define RESIZE_PREVIEW_SETUP PREVIEW_STP_BURST_ALIGNED|PREVIEW_STP_INPUT_SDRAM|PREVIEW_STP_MODE_RESIZEONLY|PREVIEW_STP_CONTINUOUS

#elif defined(DM320)
    #define RESIZE_INVERTVD() {}
    #define RESIZE_INVERTCCDCLK() {}

    #define RESIZE_PREVIEW_SETUP PREVIEW_STP_BURST_ALIGNED|PREVIEW_STP_INPUT_SDRAM|PREVIEW_STP_MODE_RESIZEONLY|PREVIEW_STP_ONESHOT|PREVIEW_STP_VD_INTERNAL

#else
    #define RESIZE_INVERTVD() {outw(inw(CCDC_SETUP)^CCDC_STP_VDSYNC_NEGATIVE,CCDC_SETUP);}
    #warning no idea on how to simulate CCD clock on DSC21/25
    #define RESIZE_INVERTCCDCLK() {}

    #define RESIZE_PREVIEW_SETUP PREVIEW_STP_BURST_ALIGNED|PREVIEW_STP_INPUT_SDRAM|PREVIEW_STP_MODE_RESIZEONLY|PREVIEW_STP_ONESHOT

#endif

void resize_setup(int inAddr,int inBufWidth,int inWidth,int inHeight,int outAddr,int outWidth,int outHeight);
void resize_execute();

#endif
