/*
* cops.h
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

#ifndef __COPS_H
#define __COPS_H

#include "graphics.h"

#define REGISTER(ops,event,flag)    {ops=(struct client_operations*)atoi(argv[argc-1]);ops->registerPlugin(event,flag);}
#define PACK(ops)              {ops->pack();}

#define MENU_SHADOW 2 // height of small shadow under the title

struct client_operations { 
   
    void (*closeScreen)       (void);
    int  (*openScreen)        (void);
    void (*clearScreen)       (int color);

    void (*drawPixel)         (int color, int x, int y);
    int  (*readPixel)         (int x, int y);
    void (*drawRect)          (int color, int x, int y, int width, int height);
    void (*fillRect)          (int color, int x, int y, int width, int height);
    void (*drawLine)          (int color, int x1, int y1, int x2, int y2);
    
    void (*getStringS)        (const unsigned char *str, int *w, int *h);
    void (*putS)              (int color, int bg_color,int x, int y, char *s);
    void (*putC)              (int color, int bg_color,int x, int y, char s);
    void (*setFont)           (int font);
    
    void (*drawSprite)        (PALETTE * palette, SPRITE * sprite, int x, int y);
    void (*drawBITMAP)        (BITMAP * bitmap, int x, int y);
    void (*drawImage)         (char * filename);
    
    void (*scrollWindowVert)  (int bgColor, int x, int y, int width, int height, int scroll, int UP);
    void (*scrollWindowHoriz) (int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);
    
    
    int  (*clearEventQueue)   (void);    
    void (*registerPlugin)    (void (*evtHandle),int flag);    
    void (*pack)              (void);    
    
    void (*showSBar)          (void);
    void (*hideSBar)          (void);
    int  (*sBarStatus)        (void); 
    
    void (*setPlane)          (int vplane);
    void (*hidePlane)         (int vplane);
    void (*showPlane)         (int vplane);
    
};

#endif