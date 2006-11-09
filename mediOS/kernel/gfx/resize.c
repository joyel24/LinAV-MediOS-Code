/*
*   kernel/gfx/resize.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/resize.h>
#include <kernel/ccdc.h>
#include <kernel/clkc.h>
#include <kernel/preview.h>
#include <kernel/kernel.h>
#include <kernel/hardware.h>
#include <kernel/io.h>

void resize_simulateVD(){
    RESIZE_INVERTVD();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTVD();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTCCDCLK();
    RESIZE_INVERTCCDCLK();
}

void resize_setup(int inAddr,int inBufWidth,int inWidth,int inHeight,int outAddr,int outWidth,int outHeight){
    int hrsz,vrsz;
    int realWidth,realHeight;
    int inOffset,outOffset;

    inOffset=(inAddr-SDRAM_START)/32;
    outOffset=(outAddr-SDRAM_START)/32;

    hrsz=(int)(0.5f+(1.0f*inWidth*PREVIEW_RESIZE_RATIO)/outWidth);
    vrsz=(int)(0.5f+(1.0f*inHeight*PREVIEW_RESIZE_RATIO)/outHeight);
    
    realWidth=(outWidth*hrsz)/PREVIEW_RESIZE_RATIO;
    realHeight=(outHeight*vrsz)/PREVIEW_RESIZE_RATIO;
    
    if(outWidth>inWidth){
        realWidth++;
    }

    // setup CCD controller
    outw(0,CCDC_ENABLE);
    outw(CCDC_STP_DATAIN_YCBCR8BIT|CCDC_STP_DATAPACK_PACKED,CCDC_SETUP);
    outw(0,CCDC_VDWIDTH);
    outw(0,CCDC_HDWIDTH);

    // setup preview engine
    outw(0,PREVIEW_ENABLE);
    outw(RESIZE_PREVIEW_SETUP,PREVIEW_SETUP);

    outw(inOffset>>16,PREVIEW_SDRAM_READ_HI);
    outw(inOffset&0xffff,PREVIEW_SDRAM_READ_LO);
    outw(outOffset>>16,PREVIEW_SDRAM_WRITE_HI);
    outw(outOffset&0xffff,PREVIEW_SDRAM_WRITE_LO);

    outw(inBufWidth,PREVIEW_HORZ_START);
    outw(realWidth,PREVIEW_HORZ_SIZE);
    outw(0,PREVIEW_VERT_START);
    outw(realHeight,PREVIEW_VERT_SIZE);

    outw(hrsz,PREVIEW_HORZ_RESIZE);
    outw(vrsz,PREVIEW_VERT_RESIZE);

    outw(0xff00,PREVIEW_LUMA_RANGE);
    outw(0xff00,PREVIEW_CHROMA_RANGE);

    resize_simulateVD();
}



void resize_execute(){
    // enable preview engine
    outw(PREVIEW_ENB_ENABLED,PREVIEW_ENABLE);
    resize_simulateVD();
}
