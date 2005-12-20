/*
*   kernel/gfx/graphics.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <api.h>
#include <graphics.h>

/************************************************** general functions ********/

void open_graphics(void)
{
    API_GFX(0x000,NULL,NULL);
}

void close_graphics(void)
{
    API_GFX(0x001,NULL,NULL);
}

/************************************************** plane functions ********/

void setPlane(int vplane)
{
    API_GFX(0x100,NULL,(void*)&vplane);
}

int getPlane(void)
{
    int vplane;
    API_GFX(0x101,NULL,(void*)&vplane);
    return vplane;
}

void hidePlane(int vplane)
{
    API_GFX(0x102,NULL,(void*)&vplane);
}

void showPlane(int vplane)
{
    API_GFX(0x103,NULL,(void*)&vplane);
}

int isShown(int vplane)
{
    int status=vplane;
    API_GFX(0x104,NULL,(void*)&status);
    return status;
}

void setState(int vplane,int state)
{
    PLANE_DATA data;
    data.vplane=vplane;
    data.state=state;
    API_GFX(0x105,NULL,(void*)&data);
}

int getState(int vplane)
{
    int status=vplane;
    API_GFX(0x106,NULL,(void*)&status);
    return status;
}

void setSize(int vplane,int width,int height,int bitsPerPixel)
{
    GFX_CONTEXT g_data;
    PLANE_DATA p_data;
    g_data.w=width;
    g_data.h=height;
    p_data.vplane=vplane;
    p_data.bpp=bitsPerPixel;
    API_GFX(0x107,&g_data,(void*)&p_data);
}

void getSize(int vplane,int * width,int * height,int * bitsPerPixel)
{
    GFX_CONTEXT g_data;
    PLANE_DATA p_data;
    p_data.vplane=vplane;
    API_GFX(0x108,&g_data,(void*)&p_data);
    if(width)
        *width=g_data.w;
    if(height)
        *height=g_data.h;
    if(bitsPerPixel)
        *bitsPerPixel=p_data.bpp;
}

void setPos(int vplane,int x,int y)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    API_GFX(0x109,&g_data,(void*)&vplane);
}

void getPos(int vplane,int * x,int * y)
{
    GFX_CONTEXT g_data;
    API_GFX(0x10A,&g_data,(void*)&vplane);
    if(x)
        *x=g_data.x;
    if(y)
        *y=g_data.y;
}

void* getBufferOffset(int vplane)
{
    GFX_CONTEXT g_data;
    API_GFX(0x10B,&g_data,(void*)&vplane);
    return (void *)g_data.color;
}

void setBufferOffset(int vplane, void * offset)
{
    GFX_CONTEXT g_data;
    g_data.color=(long)offset;
    API_GFX(0x10C,&g_data,(void*)&vplane);
}

/************************************************** drawing functions ********/

void clearScreen(unsigned int color)
{
    API_GFX(0x200,NULL,(void*)&color);
}

void drawPixel(unsigned int color,int x, int y)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.color=color;
    API_GFX(0x201,&g_data,NULL);
}

unsigned int readPixel(int x, int y)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    API_GFX(0x202,&g_data,NULL);
    return g_data.color;
}

void drawRect(unsigned int color, int x, int y, int width, int height)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.w=width;
    g_data.h=height;
    g_data.color=color;
    API_GFX(0x203,&g_data,NULL);
}

void fillRect(unsigned int color, int x, int y, int width, int height)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.w=width;
    g_data.h=height;
    g_data.color=color;
    API_GFX(0x204,&g_data,NULL);
}

void drawLine(unsigned int color, int x1, int y1, int x2, int y2)
{
    GFX_CONTEXT g_data;
    g_data.x=x1;
    g_data.y=y1;
    g_data.w=x2;
    g_data.h=y2;
    g_data.color=color;
    API_GFX(0x205,&g_data,NULL);
}

void putS(unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.color=color;
    g_data.bg_color=bg_color;
    API_GFX(0x206,&g_data,(void*)s);
}

void getStringS(unsigned char *str, int *w, int *h)
{
    GFX_CONTEXT g_data;
    API_GFX(0x207,&g_data,(void*)str);
    *w=g_data.w;
    *h=g_data.h;
}

void putC(unsigned int color, unsigned int bg_color, int x, int y, unsigned char s)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.color=color;
    g_data.bg_color=bg_color;
    API_GFX(0x208,&g_data,(void*)&s);
}

void drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.color=(unsigned int)palette;
    API_GFX(0x209,&g_data,(void*)sprite);
}

void drawBITMAP(BITMAP * bitmap, int x, int y)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    API_GFX(0x20A,&g_data,(void*)bitmap);
}

void scrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.w=width;
    g_data.h=height;
    g_data.bg_color=bgColor;
    g_data.direction=UP;
    g_data.delta=scroll;
    API_GFX(0x20B,&g_data,NULL);
}

void scrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
{
    GFX_CONTEXT g_data;
    g_data.x=x;
    g_data.y=y;
    g_data.w=width;
    g_data.h=height;
    g_data.bg_color=bgColor;
    g_data.direction=RIGHT;
    g_data.delta=scroll;
    API_GFX(0x20C,&g_data,NULL);
}
void setPalletteRGB(int r, int g, int b, int index)
{
    GFX_CONTEXT g_data;
    g_data.x=r;
    g_data.y=g;
    g_data.w=b;
    g_data.h=index;
    API_GFX(0x20D,&g_data,NULL);
}

/************************************************** font functions ********/

void setFont(int font_nb)
{
    API_GFX(0x300,NULL,&font_nb);
}

int getFont(void)
{
    int data;
    API_GFX(0x301,NULL,&data);
    return data;
}
