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

void graphics8_DrawPixel         (int color, int x, int y, struct graphicsBuffer * buff);
int  graphics8_ReadPixel         (int x, int y, struct graphicsBuffer * buff);
void graphics8_DrawRect          (int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void graphics8_FillRect          (int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void graphics8_DrawChar          (struct graphicsFont * font, int color,int bg_color, int x, int y, char c, struct graphicsBuffer * buff);
void graphics8_DrawSprite        (PALETTE * palette, SPRITE * sprite, unsigned int trsp,int x, int y, struct graphicsBuffer * buff);
void graphics8_DrawBITMAP        (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void graphics8_ScrollWindowVert  (int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff);
void graphics8_ScrollWindowHoriz (int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff);

void graphics8_1bit_Sprite (char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void graphics8_2bit_Sprite (char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void graphics8_4bit_Sprite (char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);

void graphics8_DrawHorizLine (int color, int width,char * offset);
void graphics8_DrawString    (struct graphicsFont * font, int color,int bg_color, int x, int y, char * s, struct graphicsBuffer * buff);
int graphics8_GetStringSize(struct graphicsFont * font, const unsigned char *str, int *w, int *h);

struct graphics_operations g8ops =  {
	drawPixel         : graphics8_DrawPixel,
	readPixel         : graphics8_ReadPixel,
	drawRect          : graphics8_DrawRect,
	fillRect          : graphics8_FillRect,
	drawSprite        : graphics8_DrawSprite,
	drawChar          : graphics8_DrawChar,
	drawBITMAP        : graphics8_DrawBITMAP,
	drawString        : graphics8_DrawString,
	scrollWindowVert  : graphics8_ScrollWindowVert,
	scrollWindowHoriz : graphics8_ScrollWindowHoriz,
	getStringSize     : graphics8_GetStringSize
};

#endif
