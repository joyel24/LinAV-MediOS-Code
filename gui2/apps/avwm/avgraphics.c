/*
* avgraphics.c
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
#include "graphics.h"
#include "colordef.h"
#include "osd.h"

#define CHK_PLANE     int savPlane=getPlane(); if(savPlane!=plugPlane) setPlane(plugPlane);
#define RESTORE_PLANE if(savPlane!=plugPlane) setPlane(savPlane);

int plugPlane;

void wmDrawPixel(unsigned int color, int x, int y)
{
    CHK_PLANE
    drawPixel(color,x, y);
    RESTORE_PLANE
}

unsigned int wmReadPixel(int x, int y)
{
    int ret;
    CHK_PLANE
    ret=readPixel(x,y);
    RESTORE_PLANE
}

void wmDrawRect(unsigned int color, int x, int y, int width, int height)
{
    CHK_PLANE
    drawRect(color,x,y,width,height);
    RESTORE_PLANE
}

void wmFillRect(unsigned int color, int x, int y, int width, int height)
{
    CHK_PLANE
    fillRect(color,x,y,width,height);
    RESTORE_PLANE
}

void wmDrawLine(unsigned int color, int x1, int y1, int x2, int y2)
{
    CHK_PLANE
    drawLine(color,x1,y1,x2,y2);
    RESTORE_PLANE
}

void wmPutS(unsigned int color, unsigned int bg_color,int x, int y, unsigned char *s)
{
    CHK_PLANE
    wmPutSFont(color,bg_color,x,y,s);
    RESTORE_PLANE
}

void wmPutC(unsigned int color, unsigned int bg_color,int x, int y, unsigned char s)
{
    CHK_PLANE
    wmPutCFont(color,bg_color,x,y,s);
    RESTORE_PLANE
}

void wmDrawSprite(PALETTE * palette, SPRITE * sprite, int x, int y)
{
    CHK_PLANE
    drawSprite(palette,sprite,x,y);
    RESTORE_PLANE
}

void wmDrawBITMAP(BITMAP * bitmap, int x, int y)
{
    CHK_PLANE
    drawBITMAP(bitmap,x,y);
    RESTORE_PLANE
}

void wmScrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    CHK_PLANE
    scrollWindowVert(bgColor,x,y,width,height,scroll,UP);
    RESTORE_PLANE
}

void wmScrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
{
    CHK_PLANE
    scrollWindowHoriz(bgColor,x,y,width,height,scroll,RIGHT);
    RESTORE_PLANE
}

void wmClearScreen(unsigned int color)
{
    CHK_PLANE
    clearScreen(color);
    RESTORE_PLANE
}

void wmSetPlane(int vplane)
{
    plugPlane=vplane;
}

int wmGetPlane(void)
{
    return plugPlane;
}

void defaultPlane(void)
{
    plugPlane=getPlane();
}

void cfgPlane(int vplane,int state)
{
    if(vplane == VID1 || vplane == VID2)
        return;
    setState(vplane,state|AV3XX_OSD_BITMAP_8BIT|AV3XX_OSD_BITMAP_ZX1|AV3XX_OSD_BITMAP_RAMCLUT| COLOR_TRSP << AV3XX_OSD_BITMAP_A_SHIFT);
    if(isShown(vplane))
        showPlane(vplane);

}


