/*
* avwm.h
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

#ifndef __AVWM_H
#define __AVWM_H

#include "graphics.h"

#define MENU_SHADOW 2 // height of small shadow under the title

#define EVT_REDRAW  0x0100
#define EVT_QUIT    0x0101
#define EVT_SUSPEND 0x0102
#define EVT_RESUME  0x0103

#define REGISTER(ops,event)    {ops=atoi(argv[argc-1]);ops->addEventHandler(event);}
#define PACK(ops)              {ops->pack();}

struct client_operations {    
    void (*drawPixel)         (int color, int x, int y);
    int  (*readPixel)         (int x, int y);
    void (*drawRect)          (int color, int x, int y, int width, int height);
    void (*fillRect)          (int color, int x, int y, int width, int height);
    void (*drawLine)          (int color, int x1, int y1, int x2, int y2);
    void (*putS)              (int color, int bg_color,int x, int y, char *s);
    void (*putC)              (int color, int bg_color,int x, int y, char s);
    void (*drawSprite)        (PALETTE * palette, SPRITE * sprite, int x, int y);
    void (*drawBITMAP)        (BITMAP * bitmap, int x, int y);
    void (*scrollWindowVert)  (int bgColor, int x, int y, int width, int height, int scroll, int UP);
    void (*scrollWindowHoriz) (int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);
    void (*drawImage)         (char * filename);
    int  (*clearEventQueue)   (void);
    int  (*nxtEvent)          (void);
    void (*processEvent)      (int evt);
    void (*addEventHandler)   (void (*evtHandle));
    void (*pack)              (void);
    int  (*closeScreen)       (void);
    void (*openScreen)        (void);
    void (*setFont)           (int font);
    int  (*getStringS)        (const unsigned char *str, int *w, int *h);
    void (*stopEvt)           ();
    void (*startEvt)          ();
    void (*stopTimout)        ();
    void (*startTimout)       ();
};

struct menu_item {
    struct cfg_menu * data;
    struct menu_item * nxt;
    struct menu_item * prev;
    struct menu_item * sub;
    struct menu_item * up;
};

void   wmNxtEvent       (int evt);
void   drawMenu         (void);
void   drawGui          (void);
void   getTime          (char * timeSt);
void   addEventHandler  (void (*evtHandle));
void   pack             (void);
void   eventLoop        (void);
void   debug            (char *fmt, ...);
void   drawTime         (void);
void   processTimeOut   (void);
int    loadPlugin       (char * path, char * param);  // prepare env to load a new plugin
int    launchPlugin     (char * path,char * param);   // launch a plugin
void   wmPutS           (int color, int bg_color,int x, int y, char *s);
void   wmPutC           (int color, int bg_color,int x, int y, char s);
void   wmSetFont        (int font);
void   wmgetStringS     (const unsigned char *str, int *w, int *h);

void   stopEvt          ();
void   startEvt         ();
void   stopTimout       ();
void   startTimout      ();

#endif
