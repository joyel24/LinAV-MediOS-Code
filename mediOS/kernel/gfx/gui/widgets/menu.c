/*
* kernel/gfx/gui/widget/menu.c
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <sys_def/string.h>
#include <kernel/kernel.h>

#include <kernel/graphics.h>
#include <evt.h>
#include <gui/gui.h>
#include <gui/menu.h>

#define MAXPOS       10
#define TITLE_OFFSET  2

#define CHG_PLANE     {if(current_menu->useOwnDisp) gfx_setPlane(BMAP2);}
#define RESTORE_PLANE  {if(current_menu->useOwnDisp) gfx_setPlane(BMAP1);}

int nselect,iselect,jselect;
struct menu_item *pos;
struct menu_item *pselect;

struct menu_data * current_menu;

char tmp[PATHLEN+5];

int dispName_norm(struct menu_item * item,int x,int y,int clear,int selected)
{
    int color;
    int w = 0;
    int h = 0;
    
    gfx_getStringSize("M", &w, &h);

    if(item->sub)
    {
        current_menu->submenu_str(item->data,tmp);
        color=current_menu->sub_color; /* => submenu */
    }
    else
    {
        current_menu->item_str(item->data,tmp);
        color=current_menu->txt_color; /* => item */
    }
    
    CHG_PLANE
       
    if(clear)
        gfx_fillRect(current_menu->bg_color,x, y , current_menu->width-x, h+1);

    if(selected)
        gfx_putS(color, current_menu->select_color,x, y, tmp);
    else
        gfx_putS(color, current_menu->bg_color,x, y, tmp);
    RESTORE_PLANE
    return 1;
}

int item_width=70;
int item_height=70;
int icon_zone_height=60;
int space_btw_items=4;

int dispName_icon(struct menu_item * item,int i,int j,int clear_txt,int clear_icon,int selected)
{
    int color;
    int x,y;
    int w,h;
    
    BITMAP * icon=NULL;
    
    x=current_menu->dx+i*(item_width+space_btw_items);
    y=current_menu->dy+TITLE_OFFSET+j*(item_height+space_btw_items);
    
    if(item->sub)
    {
        current_menu->submenu_str(item->data,tmp);
        color=current_menu->sub_color; /* => submenu */
        icon=current_menu->getSubIcon(item->data);
    }
    else
    {
        current_menu->item_str(item->data,tmp);
        color=current_menu->txt_color; /* => item */
        icon=current_menu->getItemIcon(item->data);        
    }
    
    CHG_PLANE
       
    if(clear_icon)
    {
        gfx_fillRect(current_menu->bg_color,x, y , item_width, icon->height);
        if(icon && icon->width<=item_width && icon->height<=icon_zone_height)
            gfx_drawBitmap(icon,x+(item_width-icon->width)/2,y+(icon_zone_height-icon->height)/2);
    }
    
    if(clear_txt)
    {
        gfx_fillRect(current_menu->bg_color,x, y + icon_zone_height+1, item_width,item_height-(icon_zone_height+1));
    }
    
       gfx_getStringSize(tmp, &w, &h); 
    
    if(selected)
        gfx_putS(color, current_menu->select_color,x+(item_width-w)/2, y+icon_zone_height+1, tmp);
    else
        gfx_putS(color, current_menu->bg_color,x+(item_width-w)/2, y+icon_zone_height+1, tmp);
    RESTORE_PLANE
    return 1;
}

void dispAllName_norm(struct menu_item * pos,int nselect)
{
    struct menu_item * i;
    int nbAff=0,w=0,h=0;
    gfx_getStringSize("M", &w, &h);

    for (i = pos; i !=NULL && nbAff < MAXPOS; i=i->nxt) {
        dispName_norm(i,current_menu->dx,TITLE_OFFSET + nbAff*(h+1) + current_menu->dy,0,nbAff==nselect);
        nbAff++;
    }
}

int MaxI=4;
int MaxJ=3;

void dispAllName_icon(struct menu_item * pos,int nselect)
{
    struct menu_item * ptr;
    int nbAff=0;
    int i=0,j=0;

    for (ptr = pos; ptr !=NULL && j<MaxJ; ptr=ptr->nxt) {
        dispName_icon(ptr,i,j,0,1,nbAff==nselect);
        i++;
        if(i>=MaxI)
        {
            i=0;
            j++;
        }
        nbAff++;
    }
}

void dispAName_norm(struct menu_item * pos, int posY, int clear, int selected)
{
    int w=0,h=0;
    gfx_getStringSize("M", &w, &h);
    dispName_norm(pos,current_menu->dx,TITLE_OFFSET + posY*(h+1)+ current_menu->dy,clear,selected);
}

void start_menu(struct menu_data * client_menu)
{    
    current_menu=client_menu;
    
    gfx_fontSet(current_menu->font);
    
    if(current_menu->useOwnDisp)
    {
        gfx_planeSetSize(BMAP2,current_menu->width,current_menu->height,8);
        gfx_planeShow(BMAP2);

        gfx_setPlane(BMAP2);
        gfx_clearScreen(current_menu->bg_color);
                
        gfx_planeSetPos(BMAP2,0x14 +2*current_menu->x,0x13+current_menu->y);
        gfx_setPlane(BMAP1);
    }
}

void stop_menu(void)
{
    if(current_menu->useOwnDisp)
        gfx_planeHide(BMAP2);
}

void menu_EvtHandler(int evt)
{
    if(current_menu->isTxtMenu)
        normMenu_handler(evt);
    else    
        iconMenu_handler(evt);
}

void normMenu_handler(int evt)
{
   int w = 0;
   int h = 0;

   gfx_getStringSize("M", &w, &h);

   switch(evt) {
        case BTN_UP:
            if(nselect==0) // moving out of current window
            {
                if(!pos->prev) // we are at the beg => nothing to change
                {
                    
                    break;
                }

                pos=pos->prev;
                pselect=pos;

                CHG_PLANE
                gfx_scrollWindowVert(current_menu->bg_color,current_menu->dx, current_menu->dy,
                                    current_menu->width-current_menu->dx, (h+1)*MAXPOS, h+1,0);
                RESTORE_PLANE
            }
            else // just going up
            {
                nselect--;
                pselect=pselect->prev;
            }
            dispAName_norm(pselect->nxt,nselect+1,1,0);
            dispAName_norm(pselect,nselect,1,1);
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
                gfx_scrollWindowVert(current_menu->bg_color, current_menu->dx, current_menu->dy,
                                    current_menu->width-current_menu->dx, (h+1)*MAXPOS, h+1,1);
                RESTORE_PLANE
            }
            else
            {
                nselect++;
                pselect=pselect->nxt;
            }
            dispAName_norm(pselect->prev,nselect-1,1,0);
            dispAName_norm(pselect,nselect,1,1);
            break;
        case BTN_RIGHT:
            if(pselect->sub) // submenu
            {
                pos=pselect->sub;
                nselect=0;
                pselect=pos;
                CHG_PLANE
                gfx_fillRect(current_menu->bg_color,current_menu->dx, current_menu->dy,
                             current_menu->width-current_menu->dx,(h+1)*MAXPOS);
                if(current_menu->has_border)
                {
                    gfx_drawRect(current_menu->border_color,0,0,current_menu->width,current_menu->height);
                }
                RESTORE_PLANE
                dispAllName_norm(pos,nselect);
            }
            else // launch plugin
            {
                current_menu->do_action(pselect->data);
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
                    gfx_fillRect(current_menu->bg_color,current_menu->dx, current_menu->dy,
                                current_menu->width-current_menu->dx,(h+1)*MAXPOS);
                    RESTORE_PLANE
                    dispAllName_norm(pos,nselect);
                }
            }
            break;
        case EVT_REDRAW:        
            CHG_PLANE
            gfx_fillRect(current_menu->bg_color,0 , current_menu->dy,
                        current_menu->width, current_menu->height-current_menu->dy);
            if(current_menu->has_border)
            {
                gfx_drawRect(current_menu->border_color,0,0,current_menu->width,current_menu->height);
            }
            RESTORE_PLANE
            pos=current_menu->root;
            pselect=current_menu->root;
            nselect=0;
            dispAllName_norm(pos,nselect);
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

struct menu_item * getPrevLine(struct menu_item * ptr)
{
    int var=0;
    while(ptr->prev && var<MaxI)
    {
        ptr=ptr->prev;
        var++;
    }
    if(var!=MaxI)
        return NULL;
    else
        return ptr;
}

struct menu_item * getMaxPrevLine(struct menu_item * ptr)
{
    int var=0;
    while(ptr->prev && var<MaxI)
    {
        ptr=ptr->prev;
        var++;
    }
    return ptr;
}

struct menu_item * getNxtLine(struct menu_item * ptr)
{
    int var=0;
    while(ptr->nxt && var<MaxI)
    {
        ptr=ptr->nxt;
        var++;
    }
    if(var!=MaxI)
        return NULL;
    else
        return ptr;
}

struct menu_item * getMaxNxtLine(struct menu_item * ptr)
{
    int var=0;
    while(ptr->nxt && var<MaxI)
    {
        ptr=ptr->nxt;
        var++;
    }
    return ptr;
}

void iconMenu_handler(int evt)
{
   int var;
   struct menu_item * ptr;
   switch(evt) {
        case BTN_UP:
            if(jselect==0) /* we are at the top of screen*/
            {
                if(!pos->prev) /* we already display the first line */
                    break; /* roll should be done here */
                
                /* deselect previsous */
                dispName_icon(pselect,iselect,jselect,1,0,0);            
                
                ptr=getPrevLine(pos);
                
                if(ptr) /* we are one line up */
                {
                    pselect=getPrevLine(pselect);
                    pos=ptr;
                    CHG_PLANE
                    gfx_scrollWindowVert(current_menu->bg_color, current_menu->dx, current_menu->dy,
                                    (item_width+space_btw_items)*MaxI,(item_height+space_btw_items)*MaxJ,
                                    item_height+space_btw_items,0);
                    RESTORE_PLANE
                    for(var=0;var<MaxI;var++)
                    {
                        if(var==iselect)
                        {
                            dispName_icon(ptr,var,jselect,1,1,1);
                            pselect=ptr;
                        }
                        else
                            dispName_icon(ptr,var,jselect,1,1,0);
                        ptr=ptr->nxt;
                    }
                        
                }
                else /* not a complete line => redraw everything */
                {
                    pos=pselect=getMaxPrevLine(pos);
                    CHG_PLANE
                    gfx_fillRect(current_menu->bg_color,0 , current_menu->dy,
                                current_menu->width, current_menu->height-current_menu->dy);
                    RESTORE_PLANE
                    iselect=jselect=0;
                    dispAllName_icon(pos,0);
                }
            }
            else
            {
                dispName_icon(pselect,iselect,jselect,1,0,0);
                jselect--;
                pselect=getPrevLine(pselect);
                dispName_icon(pselect,iselect,jselect,1,0,1);                
            }
            break;
        case BTN_DOWN:
            if(jselect==(MaxJ-1) || !getNxtLine(pselect)) /* we are at the bottom of screen*/
            {
                ptr=pselect;
                for(var=iselect;var>0;var--)
                    ptr=ptr->prev;
                    
                ptr=getNxtLine(ptr);
                if(!ptr || (jselect!=(MaxJ-1) && !getNxtLine(pselect))) /* we are already at the end of  */
                {
                    /*dispName_icon(pselect,iselect,jselect,1,0,0);
                    jselect=0;
                    
                    while(pos->prev!=NULL)
                        pos=pos->prev;
                    pselect=pos;
                    for(var=0;var<iselect;var++)
                        pselect=pselect->nxt;
                    dispName_icon(pselect,iselect,jselect,1,0,1);    */
                    break; 
                }
                dispName_icon(pselect,iselect,jselect,1,0,0);
                CHG_PLANE
                gfx_scrollWindowVert(current_menu->bg_color, current_menu->dx, current_menu->dy,
                                (item_width+space_btw_items)*MaxI,(item_height+space_btw_items)*MaxJ,
                                item_height+space_btw_items,1);
                RESTORE_PLANE
                pos=getNxtLine(pos);
                
                pselect=NULL;
                for(var=0;var<MaxI;var++)
                {
                    if(var==iselect)
                    {
                        dispName_icon(ptr,var,jselect,1,1,1);
                        pselect=ptr;                        
                    }
                    else
                        dispName_icon(ptr,var,jselect,1,1,0);
                    if(ptr->nxt==NULL)
                    {
                        break;
                    }
                    ptr=ptr->nxt;
                }
                
                if(pselect==NULL)
                {
                    pselect=ptr;
                    iselect=var;
                    dispName_icon(pselect,var,jselect,1,0,1);
                }
                
            }
            else
            {
                dispName_icon(pselect,iselect,jselect,1,0,0);
                jselect++;
                pselect=getNxtLine(pselect);
                dispName_icon(pselect,iselect,jselect,1,0,1);
            }
            break;
        case BTN_RIGHT:
            if(iselect==(MaxI-1) || pselect->nxt==NULL) /* roll to start of line */
            {
                dispName_icon(pselect,iselect,jselect,1,0,0);
                while(pselect->prev != NULL && iselect!=0)
                {
                    pselect=pselect->prev;
                    iselect--;
                }
                dispName_icon(pselect,iselect,jselect,1,0,1);
            }
            else
            {
                dispName_icon(pselect,iselect,jselect,1,0,0);
                pselect=pselect->nxt;
                iselect++;
                dispName_icon(pselect,iselect,jselect,1,0,1);
            }
            break;
        case BTN_LEFT:
            if(iselect==0) /* roll to start of line */
            {
                dispName_icon(pselect,iselect,jselect,1,0,0);
                while(pselect->nxt != NULL && iselect!=(MaxI-1))
                {
                    pselect=pselect->nxt;
                    iselect++;
                }
                dispName_icon(pselect,iselect,jselect,1,0,1);
            }
            else
            {
                dispName_icon(pselect,iselect,jselect,1,0,0);
                pselect=pselect->prev;
                iselect--;
                dispName_icon(pselect,iselect,jselect,1,0,1);
            }
            break;
        /*case EVT_MENU_UP_LVL:        
            if(pselect->up)
            {
                if(pos->up)
                {
                    if(pos->up->up)
                        pos=pos->up->up;    
                    else
                        pos=current_menu->root;
                    iselect=jselect=0;
                    pselect=pos;
                    CHG_PLANE
                    gfx_fillRect(current_menu->bg_color,current_menu->dx, current_menu->dy,
                                current_menu->width-current_menu->dx,current_menu->height-current_menu->dy);
                    RESTORE_PLANE
                    dispAllName_icon(pos,0);
                }
            }
            break;*/
        case EVT_REDRAW:        
            CHG_PLANE
            gfx_fillRect(current_menu->bg_color,0 , current_menu->dy,
                        current_menu->width, current_menu->height-current_menu->dy);
            RESTORE_PLANE
            pos=current_menu->root;
            pselect=current_menu->root;
            iselect=jselect=0;
            dispAllName_icon(pos,0);
            break;
        case BTN_OFF:
            current_menu->off_action(pselect->data);            
            break;
        case BTN_ON:
            if(pselect->sub) // submenu
            {
                pos=pselect->sub;
                iselect=jselect=0;
                pselect=pos;
                CHG_PLANE
                gfx_fillRect(current_menu->bg_color,current_menu->dx, current_menu->dy,
                             current_menu->width-current_menu->dx,current_menu->height-current_menu->dy);
                RESTORE_PLANE
                dispAllName_icon(pos,0);
            }
            else // launch plugin
            {
                current_menu->do_action(pselect->data);
            }            
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
            printk("  ");
        if(ptr->sub)
        {
            current_menu->submenu_str(ptr->data,tmp);
            printk("%s\n",tmp);
            doPrint(ptr->sub,level+1);
        }
        else
        {
            current_menu->item_str(ptr->data,tmp);
            printk("%s\n",tmp);
        }
        ptr=ptr->nxt;
    }
}


