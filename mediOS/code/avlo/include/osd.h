/*
*   include/osd.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __OSD_H
#define __OSD_H

#ifdef AV3XX
#include <osd_DSC25.h>
#endif

#ifdef AV1XX
#include <osd_DSC25.h>
#endif  
  
#ifdef JBMM
#include <osd_DSC21.h>
#endif
  
void setPalette      (int palette[256][3],int size);
int  osdRGB2Packed(int r, int g, int b);
void osdSetCursor2Bitmap (int index, int data);
void osdSetMainConfig (int config);
void osdSetMainShift (int horizontal,int vertical);
void osdSetPallette (int Y, int Cr, int Cb, int index);
void osdSet16CPallette (int bankN, int index, int value);
void osdSetAltOffset (int address);
void osdSetComponentOffset (int component, int address);
void osdSetComponentSize (int component, int width, int height);
void osdSetComponentPosition (int component, int x, int y);
void osdSetComponentSourceWidth (int component, int width);
void osdSetComponentConfig (int component, int config);
void osdInit();
void arch_init_osd(void);
void setPaletteRGB(int r,int g, int b, int index);

#define RGB2Y(r,g,b)  ((306*r + 601*g + 117*b) >> 10)
#define RGB2Cb(r,g,b)  (((-173*r -339*g + 512*b) >> 10) + 128)
#define RGB2Cr(r,g,b)  (((512*r - 429*g - 83*b) >> 10) + 128)

#define osdSetPaletteRGB(r,g,b,index)  {osdSetPallette(RGB2Y(r,g,b), RGB2Cr(r,g,b),RGB2Cb(r,g,b),index);}

void osdRestorePlane(int component, unsigned int address, int x, int y, int w, int h, int bpp, int state,int enable);

#endif

