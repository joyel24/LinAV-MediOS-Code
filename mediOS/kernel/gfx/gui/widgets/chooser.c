/*
* kernel/gfx/gui/widget/chooser.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/chooser.h>

#include <sys_def/stddef.h>

#include <kernel/malloc.h>
#include <kernel/kfont.h>

#include <evt.h>

CHOOSER chooser_create(){
    CHOOSER c;

    // allocate widget memory
    c=malloc(sizeof(*c));

    // init members
    chooser_init(c);

    return c;
}

void chooser_destroy(CHOOSER c){
    widget_destroy((WIDGET)c);
}

void chooser_init(CHOOSER c){
    widget_init((WIDGET)c);

    // methods
    c->destroy=(WIDGET_DESTROYER)chooser_destroy;
    c->handleEvent=(WIDGET_EVENTHANDLER)chooser_handleEvent;
    c->paint=(WIDGET_PAINTHANDLER)chooser_paint;

    // properties
    c->items=NULL;
    c->itemCount=0;
    c->index=-1;
}

bool chooser_handleEvent(CHOOSER c,int evt){
    bool handled=true;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)c,evt)) return true;

    switch(evt){
        case BTN_LEFT:
            if (c->itemCount){
                c->index=MAX(0,c->index-1);
            }else{
                c->index=-1;
            }
            c->paint(c);
            break;
        case BTN_RIGHT:
            if (c->itemCount){
                c->index=MIN(c->itemCount-1,c->index+1);
            }else{
                c->index=-1;
            }
            c->paint(c);
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void chooser_paint(CHOOSER c){
    int x,y,bs;
    int ax1,ax2,ay1,ay2,ay3;
    int color;

    widget_paint((WIDGET)c);

    color=(c->focused)?c->focusColor:c->backColor;

    // text
    if(c->index>=0 && c->index<c->itemCount && c->items && c->items[c->index]){ // make sure everything is ok
        char * txt = c->items[c->index];
        int of=gfx_fontGet(); // save previous font
        int tw,th;

        gfx_fontSet(c->font);

        gfx_getStringSize(txt,&tw,&th);
        x=c->x+(c->width-tw)/2;
        y=c->y+(c->height-th)/2;

        gfx_putS(c->foreColor,c->backColor,x,y,txt);

        gfx_fontSet(of); // restore previous font
    }

    // arrows
    y=c->y+c->margin;
    bs=c->height-2*c->margin;
    ay1=y+bs*1/4;
    ay2=y+bs*1/2;
    ay3=y+bs*3/4;

    // left arrow
    x=c->x+c->margin;
    ax1=x+bs*1/4;
    ax2=x+bs*3/4;

    gfx_drawRect(c->foreColor,x,y,bs,bs);
    gfx_fillRect(color,x+1,y+1,bs-2,bs-2);

    gfx_drawLine(c->foreColor,ax1,ay2,ax2,ay1);
    gfx_drawLine(c->foreColor,ax1,ay2,ax2,ay3);
    gfx_drawLine(c->foreColor,ax2,ay1,ax2,ay3);

    // right arrow
    x=c->x+c->width-c->margin-bs;
    ax1=x+bs*1/4;
    ax2=x+bs*3/4;

    gfx_drawRect(c->foreColor,x,y,bs,bs);
    gfx_fillRect(color,x+1,y+1,bs-2,bs-2);

    gfx_drawLine(c->foreColor,ax2,ay2,ax1,ay1);
    gfx_drawLine(c->foreColor,ax2,ay2,ax1,ay3);
    gfx_drawLine(c->foreColor,ax1,ay1,ax1,ay3);
}
