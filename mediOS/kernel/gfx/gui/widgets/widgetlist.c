/*
* kernel/gfx/gui/widget/widgetlist.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/widgetlist.h>

#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <evt.h>

//*****************************************************************************
// WIDGETLIST
//*****************************************************************************

WIDGETLIST widgetList_create(){
    WIDGETLIST l;

    // allocate widget memory
    l=malloc(sizeof(*l));

    // init members
    widgetList_init(l);

    return l;
}

void widgetList_destroy(WIDGETLIST l){
    // destroy items
    l->clearWidgets(l);

    // free items list
    free(l->widgets);

    widget_destroy((WIDGET)l);
}

void widgetList_init(WIDGETLIST l){
    widget_init((WIDGET)l);

    // methods
    l->destroy=(WIDGET_DESTROYER)widgetList_destroy;
    l->handleEvent=(WIDGET_EVENTHANDLER)widgetList_handleEvent;
    l->paint=(WIDGET_PAINTHANDLER)widgetList_paint;
    l->addWidget=(WIDGETLIST_WIDGETADDER)widgetList_addWidget;
    l->clearWidgets=(WIDGETLIST_WIDGETSCLEARER)widgetList_clearWidgets;
    l->indexOf=(WIDGETLIST_INDEXGETTER)widgetList_indexOf;
    l->setFocusedWidget=(WIDGETLIST_FOCUSEDWIDGETSETTER)widgetList_setFocusedWidget;
    l->changeFocus=(WIDGETLIST_FOCUSCHANGER)widgetList_changeFocus;

    // properties
    l->widgets=malloc(0); // will be realloced as items are added
    l->widgetCount=0;
    l->ownWidgets=true;
    l->previousWidget=NULL;
    l->focusedWidget=NULL;
    l->fastRepaint=false;
}

bool widgetList_handleEvent(WIDGETLIST l,int evt){
    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)l,evt)) return true;

    if (l->focusedWidget==NULL) return false;

    // pass events to the focused widget
    return l->focusedWidget->handleEvent(l->focusedWidget,evt);
}

void widgetList_paint(WIDGETLIST l){
    int i;

    if(l->fastRepaint){

        if(l->focusedWidget==NULL){
            printk("[widgetlist] widgetList_paint sanity error!\n");
            return;
        }

        // repaint only previous and current widget
        if(l->previousWidget!=NULL){
            l->previousWidget->paint(l->previousWidget);
        }
        l->focusedWidget->paint(l->focusedWidget);
    }else{

        widget_paint((WIDGET)l);

        // repaint all widgets
        for(i=0;i<l->widgetCount;++i){
            l->widgets[i]->paint(l->widgets[i]);
        }
    }
}

void widgetList_addWidget(WIDGETLIST l,WIDGET w){
    l->widgetCount++;
    l->widgets=realloc(l->widgets,l->widgetCount*sizeof(w));
    l->widgets[l->widgetCount-1]=w;

    w->focused=false;
}

void widgetList_clearWidgets(WIDGETLIST l){
    // destroy widgets
    if (l->ownWidgets){
        int i;
        for(i=0;i<l->widgetCount;++i) l->widgets[i]->destroy(l->widgets[i]);
    }

    l->widgets=realloc(l->widgets,0);
    l->previousWidget=NULL;
    l->focusedWidget=NULL;
    l->fastRepaint=false;
}

int widgetList_indexOf(WIDGETLIST l,WIDGET w){
    int i;
    for(i=0;i<l->widgetCount;++i){
        if(l->widgets[i]==w) return i;
    }
    // widget not found
    return -1;
}

void widgetList_setFocusedWidget(WIDGETLIST l,WIDGET w){

    if(l->indexOf(l,w)<0){
        printk("[widgetList] error: widget is not in the list!\n");
        return;
    }

    // update previousWidget
    if (l->focusedWidget!=NULL){
        l->previousWidget=l->focusedWidget;
        l->previousWidget->focused=false;
    }

    // update focusedWidget
    l->focusedWidget=w;
    l->focusedWidget->focused=true;

    // repaint widgets
    l->fastRepaint=true;
    l->paint(l);
    l->fastRepaint=false;
}


void widgetList_changeFocus(WIDGETLIST l,WL_DIRECTION dir){
    int i;
    int pos;
    int idx;
    WIDGET w;
    WIDGET tw;

    w=NULL;

    if (l->focusedWidget!=NULL){

        pos=l->focusedWidget->focusPosition;
        idx=l->indexOf(l,l->focusedWidget);

        if(dir==WLD_PREVIOUS){

            // find focusable widget with highest position that is smaller than the current one
            for(i=1;i<l->widgetCount;++i){

                tw=l->widgets[(l->widgetCount+idx-i)%l->widgetCount];

                if(tw->focusPosition<=pos && tw->canFocus && (w==NULL || tw->focusPosition>w->focusPosition)){
                    w=tw;
                }
            }

            if(w==NULL){ // no next widget ?
                // find focusable widget with highest position
                pos=INT_MIN;
                for(i=0;i<l->widgetCount;++i){
                    if(l->widgets[i]->focusPosition>pos && l->widgets[i]->canFocus){
                        w=l->widgets[i];
                        pos=w->focusPosition;
                    }
                }
            }

        }else{

            // find focusable widget with smallest position that is higher than the current one
            for(i=1;i<l->widgetCount;++i){

                tw=l->widgets[(idx+i)%l->widgetCount];

                if(tw->focusPosition>=pos && tw->canFocus && (w==NULL || tw->focusPosition<w->focusPosition)){
                    w=tw;
                }
            }
        }
    }

    if(w==NULL){ // no focused widget or no next widget ?

        // find focusable widget with smallest position
        pos=INT_MAX;
        for(i=0;i<l->widgetCount;++i){
            if(l->widgets[i]->focusPosition<pos && l->widgets[i]->canFocus){
                w=l->widgets[i];
                pos=w->focusPosition;
            }
        }
    }

    l->setFocusedWidget(l,w);
}
