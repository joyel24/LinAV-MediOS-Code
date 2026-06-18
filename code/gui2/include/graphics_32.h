/*
* graphics_32.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __GRAPHICS_32_H
#define __GRAPHICS_32_H

void         graphics32_DrawPixel         (unsigned int color, int x, int y, struct graphicsBuffer * buff);
unsigned int graphics32_ReadPixel         (int x, int y, struct graphicsBuffer * buff);
void         graphics32_DrawRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics32_FillRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics32_DrawChar          (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char c, struct graphicsBuffer * buff);
void         graphics32_DrawSprite        (PALETTE * palette, SPRITE * sprite, unsigned int trsp,int x, int y, struct graphicsBuffer * buff);
void         graphics32_DrawBITMAP        (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void         graphics32_ScrollWindowVert  (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff);
void         graphics32_ScrollWindowHoriz (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff);
void         graphics32_DrawHLine         (unsigned int color, int x, int y, int width, struct graphicsBuffer * buff);
void         graphics32_DrawVLine         (unsigned int color, int x, int y, int height, struct graphicsBuffer * buff);

void         graphics32_DrawHorizLine (unsigned int color, int width,unsigned int * offset);
void         graphics32_DrawString    (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char * s, struct graphicsBuffer * buff);

#endif
