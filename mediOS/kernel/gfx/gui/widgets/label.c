/*
* kernel/gfx/gui/widget/label.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/label.h>

#include <kernel/malloc.h>

LABEL label_create(){
    LABEL l;

    // allocate widget memory
    l=malloc(sizeof(*l));

    // init members
    label_init(l);

    return l;
}

void label_init(LABEL l){
    widget_init((WIDGET)l);

    // methods
    l->paint=(WIDGET_PAINTHANDLER)label_paint;

    // properties
    l->canFocus=false;
    l->caption="Label";
    l->alignment=LA_LEFT;
}

void label_paint(LABEL l){
    int x,y;
    int w,h;
    int of;

    widget_paint((WIDGET)l);

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(l->font);

    gfx_getStringSize(l->caption,&w,&h);
    y=l->y+(l->height-h)/2;
    x=0;

    switch (l->alignment){
        case LA_LEFT:
            x=l->x+l->margin;
            break;
        case LA_RIGHT:
            x=l->x+l->width-l->margin-w;
            break;
        case LA_CENTER:
            x=l->x+(l->width-w)/2;
            break;
    }

    gfx_putS(l->foreColor,l->backColor,x,y,l->caption);

    gfx_fontSet(of); // restore previous font
}
