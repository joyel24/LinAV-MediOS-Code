/*
* kernel/gfx/gui/widget/textmenu.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/textmenu.h>

#include <sys_def/stddef.h>

#include <kernel/malloc.h>

#include <evt.h>

//*****************************************************************************
// TEXTMENU_ITEM
//*****************************************************************************

TEXTMENU_ITEM textMenuItem_create(){
    TEXTMENU_ITEM mi;

    // allocate widget memory
    mi=malloc(sizeof(*mi));

    // init members
    textMenuItem_init(mi);

    return mi;
}

void textMenuItem_destroy(TEXTMENU_ITEM mi){
    menuItem_destroy((MENU_ITEM)mi);
}

void textMenuItem_init(TEXTMENU_ITEM mi){
    menuItem_init((MENU_ITEM)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)textMenuItem_destroy;
    mi->paint=(WIDGET_PAINTHANDLER)textMenuItem_paint;

    // properties
    mi->caption="TextMenuItem";
    mi->alignment=TMA_LEFT;
}

void textMenuItem_paint(TEXTMENU_ITEM mi){
    int x,y;
    int w,h;
    int color;
    int of;

    widget_paint((WIDGET)mi);

    color=(mi->focused)?mi->focusColor:mi->backColor;

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(mi->font);

    gfx_getStringSize(mi->caption,&w,&h);
    y=mi->y+(mi->height-h)/2;
    x=0;

    switch (mi->alignment){
        case TMA_LEFT:
            x=mi->x+mi->margin;
            break;
        case TMA_RIGHT:
            x=mi->x+mi->width-mi->margin-w;
            break;
        case TMA_CENTER:
            x=mi->x+(mi->width-w)/2;
            break;
    }

    gfx_putS(mi->foreColor,color,x,y,mi->caption);

    gfx_fontSet(of); // restore previous font
}


//*****************************************************************************
// TEXTMENU
//*****************************************************************************

TEXTMENU textMenu_create(){
    TEXTMENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));

    // init members
    textMenu_init(m);

    return m;
}

void textMenu_destroy(TEXTMENU m){
    menu_destroy((MENU)m);
}

void textMenu_init(TEXTMENU m){
    menu_init((MENU)m);

    // methods
    m->handleEvent=(WIDGET_EVENTHANDLER)textMenu_handleEvent;
    m->addItem=(MENU_ITEMADDER)textMenu_addItem;

    // properties
    m->itemHeight=10;
}

bool textMenu_handleEvent(TEXTMENU m,int evt){
    bool handled=true;

    // let's see if the ancestor handles the event
    if (menu_handleEvent((MENU)m,evt)) return true;

    // sanity check
    if(m->index<0 || m->index>=m->itemCount) return false;

    // save prev index
    m->previousIndex=m->index;

    switch(evt){
        case BTN_UP:
            do{ // find previous focusable item
                m->index--;
                if(m->index<0) m->index=m->itemCount-1;
            }while(!m->items[m->index]->canFocus);
            break;
        case BTN_DOWN:
            do{ // find next focusable item
                m->index++;
                if(m->index>=m->itemCount) m->index=0;
            }while(!m->items[m->index]->canFocus);
            break;
        case BTN_RIGHT:
            menu_handleEvent((MENU)m,WIDGET_ACTION_BTN);
            break;
        default:
            handled=false;
            break;
    }

    // handle moves
    if(m->index!=m->previousIndex){
        if(m->index>=m->topIndex && m->index<m->topIndex+m->visibleCount){ // no changes in items visibility?

            // only changes in focus -> fast update & repaint
            textMenu_updateItems(m,true);
            m->fastRepaint=true;
            m->paint(m);
            m->fastRepaint=false;

        }else{
            
            if(m->index<m->topIndex){ // we moved up?
                m->topIndex=m->index;
            }else{
                m->topIndex=m->index-m->visibleCount+1;
            }

            // full update & repaint
            textMenu_updateItems(m,false);
            m->fastRepaint=false;
            m->paint(m);
        }
    }

    return handled;
}

void textMenu_addItem(TEXTMENU m, TEXTMENU_ITEM item){
    int maxvc=(m->height-2*m->margin)/m->itemHeight; // count of max visible items in the menu

    menu_addItem((MENU)m,(MENU_ITEM)item);

    if(m->visibleCount<maxvc) m->visibleCount++;

    textMenu_updateItems(m,false);
}

void textMenu_updateItems(TEXTMENU m, bool fast){
    int i;
    MENU_ITEM mi;

    if (fast){

        // only update focus
        m->items[m->previousIndex]->focused=false;
        m->items[m->index]->focused=true;

    }else{

        // full repositioning of visible items
        for(i=m->topIndex;i<m->topIndex+m->visibleCount;++i){
            mi=m->items[i];

            mi->focused=(i==m->index);

            mi->x=m->x+m->margin;
            mi->y=m->y+m->margin+(i-m->topIndex)*m->itemHeight;
            mi->width=m->width-2*m->margin;
            mi->height=m->itemHeight;
        }
    }
}

