/*
* kernel/gfx/gui/widget/trackbar.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/trackbar.h>

#include <sys_def/stddef.h>
#include <sys_def/string.h>

#include <kernel/malloc.h>
#include <kernel/kfont.h>

#include <evt.h>

TRACKBAR trackbar_create(){
    TRACKBAR t;

    // allocate widget memory
    t=malloc(sizeof(*t));

    // init members
    trackbar_init(t);

    return t;
}

void trackbar_destroy(TRACKBAR t){
    widget_destroy((WIDGET)t);
}

void trackbar_init(TRACKBAR t){
    widget_init((WIDGET)t);

    // methods
    t->destroy=(WIDGET_DESTROYER)trackbar_destroy;
    t->handleEvent=(WIDGET_EVENTHANDLER)trackbar_handleEvent;
    t->paint=(WIDGET_PAINTHANDLER)trackbar_paint;
    t->onChange=NULL;

    // properties
    t->value=0;
    t->minimum=0;
    t->maximum=100;
    t->increment=1;
    t->numTicks=3;
    t->numValueDigits=3;
}

bool trackbar_handleEvent(TRACKBAR t,int evt){
    bool handled=true;
    int ov=t->value;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)t,evt)) return true;

    switch(evt){
        case BTN_LEFT:
            t->value=MAX(t->value-t->increment,t->minimum);
            t->paint(t);
            break;
        case BTN_RIGHT:
            t->value=MIN(t->value+t->increment,t->maximum);
            t->paint(t);
            break;
        default:
            handled=false;
            break;
    }

    // onChange event
    if (t->onChange!=NULL && ov!=t->value) t->onChange(t);

    return handled;
}

void trackbar_paint(TRACKBAR t){
    int i;
    int x,y,w,h;
    int valw;
    int curw;
    int color;
    FONT f;

    widget_paint((WIDGET)t);

    f=fnt_fontFromId(t->font);
    valw=MAX(0,t->numValueDigits*f->width);

    curw=t->height*TRACKBAR_CURSOR_RATIO;

    color=(t->focused)?t->focusColor:t->backColor;

    // bar
    x=t->x+t->margin+curw/2;
    y=t->y+t->height/3;
    w=t->width-valw-TRACKBAR_SPACING-2*t->margin-curw;
    h=t->height/3;

    gfx_drawRect(t->foreColor,x,y,w,h);

    // ticks
    for(i=1;i<=t->numTicks;i++){
        int tx=x+i*w/(t->numTicks+1);

        gfx_drawLine(t->foreColor,tx,y,tx,y+h-1);
    };

    // cursor
    if(t->value>=t->minimum && t->value<=t->maximum){
      x=t->x+(t->value-t->minimum)*w/(t->maximum-t->minimum)+t->margin;
      y=t->y+t->margin;
      h=t->height-2*t->margin;
  
      gfx_drawRect(t->foreColor,x,y,curw,h);
      gfx_fillRect(color,x+1,y+1,curw-2,h-2);
    }

    // value
    if (valw){
        int of=gfx_fontGet(); // save previous font
        char vs[10];

        x=t->x+t->width-valw-t->margin;
        y=t->y+(t->height-f->height)/2;

        sprintf(vs,"%d",t->value);

        gfx_fontSet(t->font);

        gfx_putS(t->foreColor,t->backColor,x,y,vs);

        gfx_fontSet(of); // restore previous font
    }
}

