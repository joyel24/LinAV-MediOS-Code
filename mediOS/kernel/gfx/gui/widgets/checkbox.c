/*
* kernel/gfx/gui/widget/checkbox.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/checkbox.h>

#include <sys_def/stddef.h>

#include <kernel/malloc.h>
#include <kernel/kfont.h>

#include <evt.h>

CHECKBOX checkbox_create(){
    CHECKBOX c;

    // allocate widget memory
    c=malloc(sizeof(*c));

    // init members
    checkbox_init(c);

    return c;
}

void checkbox_destroy(CHECKBOX c){
    widget_destroy((WIDGET)c);
}

void checkbox_init(CHECKBOX c){
    widget_init((WIDGET)c);

    // methods
    c->destroy=(WIDGET_DESTROYER)checkbox_destroy;
    c->handleEvent=(WIDGET_EVENTHANDLER)checkbox_handleEvent;
    c->paint=(WIDGET_PAINTHANDLER)checkbox_paint;
    c->onChange=NULL;

    // properties
    c->caption="Checkbox";
    c->checked=false;
}

bool checkbox_handleEvent(CHECKBOX c,int evt){
    bool handled=true;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)c,evt)) return true;

    switch(evt){
        case BTN_LEFT:
        case BTN_RIGHT:
        case WIDGET_ACTION_BTN:
            c->checked=!c->checked;
            c->paint(c);
            if (c->onChange!=NULL) c->onChange(c);
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void checkbox_paint(CHECKBOX c){
    int x,y,bs,of;
    int color;

    widget_paint((WIDGET)c);

    color=(c->focused)?c->focusColor:c->backColor;

    // box
    bs=c->height-2*c->margin;
    x=c->x+c->margin;
    y=c->y+c->margin;

    gfx_drawRect(c->foreColor,x,y,bs,bs);
    gfx_fillRect(color,x+1,y+1,bs-2,bs-2);

    if (c->checked){
        gfx_drawLine(c->foreColor,x,y,x+bs-1,y+bs-1);
        gfx_drawLine(c->foreColor,x+bs-1,y,x,y+bs-1);
    }

    // text
    x=c->x+c->margin+bs+CHECKBOX_SPACING;
    y=c->y+(c->height-fnt_fontFromId(c->font)->height)/2;

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(c->font);

    gfx_putS(c->foreColor,c->backColor,x,y,c->caption);

    gfx_fontSet(of); // restore previous font
}
