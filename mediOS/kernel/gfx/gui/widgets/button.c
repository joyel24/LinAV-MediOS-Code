/*
* kernel/gfx/gui/widget/button.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/button.h>

#include <sys_def/stddef.h>

#include <kernel/malloc.h>
#include <kernel/kfont.h>

#include <evt.h>

BUTTON button_create(){
    BUTTON b;

    // allocate widget memory
    b=malloc(sizeof(*b));

    // init members
    button_init(b);

    return b;
}

void button_destroy(BUTTON b){
    widget_destroy((WIDGET)b);
}

void button_init(BUTTON b){
    widget_init((WIDGET)b);

    // methods
    b->destroy=(WIDGET_DESTROYER)button_destroy;
    b->handleEvent=(WIDGET_EVENTHANDLER)button_handleEvent;
    b->paint=(WIDGET_PAINTHANDLER)button_paint;
    b->onClick=NULL;

    // properties
    b->caption="Button";
}

bool button_handleEvent(BUTTON b,int evt){
    bool handled=true;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)b,evt)) return true;

    switch(evt){
        case BTN_LEFT:
        case BTN_RIGHT:
        case WIDGET_ACTION_BTN:
            if (b->onClick!=NULL) b->onClick(b);
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void button_paint(BUTTON b){
    int x,y;
    int w,h;
    int color;

    widget_paint((WIDGET)b);

    color=(b->focused)?b->focusColor:b->backColor;

    // frame
    x=b->x+b->margin;
    y=b->y+b->margin;
    w=b->width-2*b->margin;
    h=b->height-2*b->margin;

    gfx_drawRect(b->foreColor,x,y,w,h);
    if (b->focused) gfx_fillRect(color,x+1,y+1,w-2,h-2);

    // caption
    int of=gfx_fontGet(); // save previous font

    gfx_fontSet(b->font);

    gfx_getStringSize(b->caption,&w,&h);
    x=b->x+(b->width-w)/2;
    y=b->y+(b->height-h)/2;

    gfx_putS(b->foreColor,color,x,y,b->caption);

    gfx_fontSet(of); // restore previous font
}
