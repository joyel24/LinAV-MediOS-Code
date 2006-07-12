/*
* kernel/gfx/gui/widget/widget.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/widget.h>

#include <kernel/malloc.h>

#include <evt.h>

WIDGET widget_create(){
    WIDGET w;

    // allocate widget memory
    w=malloc(sizeof(*w));

    // init members
    widget_init(w);

    return w;
}

void widget_destroy(WIDGET w){
    // free widget memory
    free(w);
}

void widget_init(WIDGET w){
    // methods
    w->destroy=(WIDGET_DESTROYER)widget_destroy;
    w->handleEvent=(WIDGET_EVENTHANDLER)widget_handleEvent;
    w->paint=(WIDGET_PAINTHANDLER)widget_paint;

    // properties
    w->x=0;
    w->y=0;
    w->width=0;
    w->height=0;
    w->margin=WIDGET_DEFAULT_MARGIN;
    w->clearBackground=true;
    w->canFocus=true;
    w->focusPosition=-1;
    w->focused=false;
    w->foreColor=WIDGET_DEFAULT_FORECOLOR;
    w->backColor=WIDGET_DEFAULT_BACKCOLOR;
    w->focusColor=WIDGET_DEFAULT_FOCUSCOLOR;
    w->font=WIDGET_DEFAULT_FONT;
}

bool widget_handleEvent(WIDGET w,int evt){
    bool handled=true;

    switch(evt){
        case EVT_REDRAW:
            w->paint(w);
            break;
        default:
            handled=false;
            break;
    }
    
    return handled;
}

void widget_paint(WIDGET w){

    if(w->clearBackground){
        gfx_fillRect(w->backColor,w->x,w->y,w->width,w->height);
    }
}
