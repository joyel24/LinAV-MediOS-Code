/*
* menu.c
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

#include <stdlib.h>

#include "graphics.h"
#include "osd.h"
#include "events.h"
#include "cops.h"
#include "menu.h"
#include "colordef.h"
#include "avevents.h"
#include "parse_cfg.h"

#define MAXPOS       10
#define TITLE_OFFSET  2

#define CHG_PLANE     {if(current_menu->useOwnDisp) setPlane(BMAP2);}
#define RESTORE_PLANE  {if(current_menu->useOwnDisp) setPlane(BMAP1);}

int nselect;
struct menu_item *pos;
struct menu_item *pselect;

struct menu_data * current_menu;

char tmp[MAX_TOKEN+5];

int printName(struct menu_item * item,int x,int y,int clear,int selected)
{
    int color;
    int w = 0;
    int h = 0;
    
    getStringS("M", &w, &h);

    if(item->sub)
    {
        current_menu->submenu_str(item->data,tmp);
        color=COLOR_RED; /* => submenu */
    }
    else
    {
        current_menu->item_str(item->data,tmp);
        color=COLOR_BLACK; /* => item */
    }
    
    CHG_PLANE
       
    if(clear)
        fillRect(COLOR_WHITE,x, y , current_menu->width-x, h+1);

    if(selected)
        putS(color, COLOR_BLUE,x, y, tmp);
    else
        putS(color, COLOR_WHITE,x, y, tmp);
    RESTORE_PLANE
}

void printAllName(struct menu_item * pos,int nselect)
{
    struct menu_item * i;
    int nbAff=0,w=0,h=0;
    getStringS("M", &w, &h);

    for (i = pos; i !=NULL && nbAff < MAXPOS; i=i->nxt) {
        printName(i,5,TITLE_OFFSET + nbAff*(h+1) + h+6+MENU_SHADOW,0,nbAff==nselect);
        nbAff++;
    }
}

void printAName(struct menu_item * pos, int posY, int clear, int selected)
{
    int w=0,h=0;
    getStringS("M", &w, &h);
    printName(pos,5,TITLE_OFFSET + posY*(h+1)+ h+6+MENU_SHADOW,clear,selected);
}

void start_menu(struct menu_data * client_menu)
{    
    current_menu=client_menu;
    if(current_menu->width==0)
        current_menu->width=320;
    if(current_menu->height==0)
        current_menu->width=240;
    if(current_menu->useOwnDisp)
    {
        setSize(BMAP2,current_menu->width,current_menu->height,8);
        showPlane(BMAP2);
             
        setPlane(BMAP2);
        clearScreen(COLOR_WHITE);
        setPos(BMAP2,0x14 +2*current_menu->x,0x13+current_menu->y);
        setPlane(BMAP1);
    }
}

void stop_menu(void)
{
    if(current_menu->useOwnDisp)
        hidePlane(BMAP2);
}

void menuEvtHandler(int evt)
{
   int w = 0;
   int h = 0;

   getStringS("M", &w, &h);

   switch(evt) {
        case BTN_UP:
            if(nselect==0) // moving out of current window
            {
                if(!pos->prev) // we are at the beg => nothing to change
                    break; // to do rolling menu code to change is here

                pos=pos->prev;
                pselect=pos;

                CHG_PLANE
                scrollWindowVert(COLOR_WHITE, 5, h+6+MENU_SHADOW, current_menu->width-5, (h+1)*MAXPOS, h+1,0);
                RESTORE_PLANE
            }
            else // just going up
            {
                nselect--;
                pselect=pselect->prev;
            }
            printAName(pselect->nxt,nselect+1,1,0);
            printAName(pselect,nselect,1,1);
            break;
        case BTN_DOWN:
            if(!pselect->nxt) // we are at the end => can't go down anymore
                break;

            if(nselect==MAXPOS-1) // moving out of the window
            {
                if(!pos->nxt) // we are at the end => can't go down anymore
                    break;
                
                pos=pos->nxt;
                pselect=pos;
                CHG_PLANE
                scrollWindowVert(COLOR_WHITE, 5, h+6+MENU_SHADOW, current_menu->width-5, (h+1)*MAXPOS, h+1,1);
                RESTORE_PLANE
            }
            else
            {
                nselect++;
                pselect=pselect->nxt;
            }
            printAName(pselect->prev,nselect-1,1,0);
            printAName(pselect,nselect,1,1);
            break;
        case BTN_RIGHT:
            if(pselect->sub) // submenu
            {
                pos=pselect->sub;
                nselect=0;
                pselect=pos;
                CHG_PLANE
                fillRect(COLOR_WHITE,5, h+6+MENU_SHADOW , current_menu->width-5,(h+1)*MAXPOS);
                RESTORE_PLANE
                printAllName(pos,nselect);
                clearEventQueue();
            }
            else // launch plugin
            {
                current_menu->right_action(pselect->data);
            }
            break;
        case BTN_LEFT:
            if(pselect->up)
            {
                if(pos->up)
                {
                    if(pos->up->up)
                        pos=pos->up->up;    
                    else
                        pos=current_menu->root;
                    nselect=0;
                    pselect=pos;
                    CHG_PLANE
                    fillRect(COLOR_WHITE,5, h+6+MENU_SHADOW , current_menu->width-5,(h+1)*MAXPOS);
                    RESTORE_PLANE
                    printAllName(pos,nselect);
                    clearEventQueue();
                }
            }
            break;
        case EVT_REDRAW:
            CHG_PLANE
            fillRect(COLOR_WHITE,0 , h+6+MENU_SHADOW, current_menu->width-5, current_menu->height-h-6-MENU_SHADOW);
            RESTORE_PLANE
            pos=current_menu->root;
            pselect=current_menu->root;
            nselect=0;
            printAllName(pos,nselect);
            break;
        case BTN_OFF:
            current_menu->off_action(pselect->data);            
            break;
        case BTN_ON:
            current_menu->on_action(pselect->data);
            break;
        case BTN_F1:
            current_menu->f1_action(pselect->data);
            break;
        case BTN_F2:
            current_menu->f2_action(pselect->data);
            break;
        case BTN_F3:
            current_menu->f3_action(pselect->data);
            break;
    }
}

/* txt function used for debug */

void doPrint(struct menu_item * ptr,int level)
{
    int i;
    while(ptr!=NULL)
    {
        for(i=0;i<level;i++)
            printf("  ");
        if(ptr->sub)
        {
            current_menu->submenu_str(ptr->data,tmp);
            printf("%s\n",tmp);
            doPrint(ptr->sub,level+1);
        }
        else
        {
            current_menu->item_str(ptr->data,tmp);
            printf("%s\n",tmp);
        }
        ptr=ptr->nxt;
    }
}


