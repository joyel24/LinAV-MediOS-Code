/*
* avstring.c
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
#include <stdlib.h>
#include <stdarg.h>
#include "font.h"
#include "graphics.h"
#include "avstring.h"
#include "colordef.h"

#define DO_DEBUG 1

needFont(std6x9);
needFont(std8x13);
needFont(std4x6);
needFont(std4x8);
needFont(std5x7);
needFont(std5x8);
needFont(std6x10);
needFont(std6x12);
needFont(std6x13);
needFont(std6x9);
needFont(std7x13);
needFont(std7x14);
needFont(std8x13);
needFont(cursive);
needFont(dagger);
needFont(inkblot);
needFont(radon);
needFont(radonWide);
needFont(shadow);
needFont(shadowBold);

FONT_ID font_table[NBFONT] ;

FONT_ID plugin_font;

void ini_font(int font)
{

    font_table[0]=std4x6;
    font_table[1]=std4x8;
    font_table[2]=std5x7;
    font_table[3]=std5x8;
    font_table[4]=std6x10;
    font_table[5]=std6x12;
    font_table[6]=std6x13;
    font_table[7]=std6x9;
    font_table[8]=std7x13;
    font_table[9]=std7x14;
    font_table[10]=std8x13;
    font_table[11]=cursive;
    font_table[12]=dagger;
    font_table[13]=inkblot;
    font_table[14]=radon;
    font_table[15]=radonWide;
    font_table[26]=shadow;
    font_table[27]=shadowBold;
    
    setFont(font_table[font]);
    wmSetFont(font);
}

void wmgetStringS(const unsigned char *str, int *w, int *h)
{
    FONT_ID font=getFont();
    if(font != plugin_font)
    {
        setFont(plugin_font);
        getStringS(str,w,h);
        setFont(font);
    }
    else
        getStringS(str,w,h);
}

void wmPutS(int color, int bg_color,int x, int y, char *s)
{
    FONT_ID font=getFont();
    if(font != plugin_font)
    {
        setFont(plugin_font);
        putS(color,bg_color,x,y,s);
        setFont(font);
    }
    else
        putS(color,bg_color,x,y,s);
}

void wmPutC(int color, int bg_color,int x, int y, char s)
{
    FONT_ID font=getFont();
    if(font != plugin_font)
    {
        setFont(plugin_font);
        putC(color,bg_color,x,y,s);
        setFont(font);
    }
    else
        putC(color,bg_color,x,y,s);
}

void wmSetFont(int font)
{
    plugin_font=font_table[font];
}

void defaultFont(void)
{
    plugin_font=getFont();
    
}


static char debugmembuf[200];

void debug(char *fmt, ...)
{
#ifdef DO_DEBUG

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    putS(COLOR_BLACK,COLOR_WHITE,0,230,debugmembuf);
    va_end(ap);

#endif
}
