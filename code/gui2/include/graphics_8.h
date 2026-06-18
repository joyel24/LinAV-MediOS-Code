/*
* graphics_8.h
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

#ifndef __GRAPHICS_8_H
#define __GRAPHICS_8_H

void         graphics8_DrawPixel         (unsigned int color, int x, int y, struct graphicsBuffer * buff);
unsigned int graphics8_ReadPixel         (int x, int y, struct graphicsBuffer * buff);
void         graphics8_DrawRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics8_FillRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics8_DrawChar          (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char c, struct graphicsBuffer * buff);
void         graphics8_DrawSprite        (PALETTE * palette, SPRITE * sprite, unsigned int trsp,int x, int y, struct graphicsBuffer * buff);
void         graphics8_DrawBITMAP        (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void         graphics8_ScrollWindowVert  (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff);
void         graphics8_ScrollWindowHoriz (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff);
void         graphics8_DrawHLine         (unsigned int color, int x, int y, int width, struct graphicsBuffer * buff);
void         graphics8_DrawVLine         (unsigned int color, int x, int y, int height, struct graphicsBuffer * buff);

void         graphics8_DrawHorizLine (unsigned int color, int width,unsigned char * offset);
void         graphics8_DrawString    (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char * s, struct graphicsBuffer * buff);

#endif
