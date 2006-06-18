/*
*   include/kernel/osd.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __OSD_H
#define __OSD_H

#ifdef DSC25
    #include <kernel/osd_DSC25.h>
#elif defined(DSC21)
    #include <kernel/osd_DSC21.h>
#else
    #error NO CHIP defined
#endif


int  osd_RGB2Packed(int r, int g, int b);
void osd_setCursor2Bitmap (int index, int data);
void osd_setMainConfig (int config);
void osd_setMainShift (int horizontal,int vertical);
void osd_setEntirePalette (int palette[256][3],int size);
void osd_setPaletteRGB(int r,int g,int b,int index);
void osd_setPalette (int Y, int Cr, int Cb, int index);
void osd_set16CPalette (int bankN, int index, int value);
void osd_setAltOffset (int address);
void osd_setComponentOffset (int component, int address);
void osd_setComponentSize (int component, int width, int height);
void osd_setComponentPosition (int component, int x, int y);
void osd_setComponentSourceWidth (int component, int width);
void osd_setComponentConfig (int component, int config);
void osd_restorePlane(int component, unsigned int address, int x, int y, int w, int h, int bpp, int state,int enable);
void osd_init();
void arch_osd_init(void);

#define RGB2Y(r,g,b)  ((306*r + 601*g + 117*b) >> 10)
#define RGB2Cb(r,g,b)  (((-173*r -339*g + 512*b) >> 10) + 128)
#define RGB2Cr(r,g,b)  (((512*r - 429*g - 83*b) >> 10) + 128)

#endif

