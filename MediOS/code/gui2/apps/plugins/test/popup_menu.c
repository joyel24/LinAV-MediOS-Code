/*
* sprite.c
* by Schoki
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

#include "stdlib.h"
#include "stdio.h"

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"

struct client_operations * cops;

int mode=1;

void do_off(void * data)
{
    cops->stop_menu();
    mode=1;
}
void do_on(void * data)
{
}
void do_right(void * data)
{
}
void do_F1(void * data)
{
}
void do_F2(void * data)
{
}
void do_F3(void * data)
{
}

void mk_item_str(void * data,char * str)
{   
    sprintf(str,"%s",(char*) data);
}

struct menu_data menu_cfg = {
    useOwnDisp     : 1,
    x:20,y:20,width:45,height:100,
    dx:2,dy:2,
    txt_color      : COLOR_BLACK,
    bg_color       : COLOR_WHITE,
    select_color   : COLOR_BLUE,
    sub_color      : COLOR_RED,
    root           : NULL,
    right_action   : do_right,
    on_action      : do_on,
    off_action     : do_off,
    f1_action      : do_F1,
    f2_action      : do_F2,
    f3_action      : do_F3,
    item_str       : mk_item_str,
    submenu_str    : mk_item_str,
    getSubIcon     : NULL,
    getItemIcon    : NULL,
    isTxtMenu      : 1,
    border_color   : COLOR_BLACK,
    has_border     : 0,
    title          : NULL,
    
};

struct menu_item rootMenu;
struct menu_item menu1;
struct menu_item menu2;
struct menu_item menu3;

// Matrix entries are the indexes from the palette entries

int eventHandler(int evt)
{
    if(mode)
    {
        switch (evt) {
            case BTN_OFF:
            case EVT_QUIT:
                /* get out of here */
                RELEASE(cops)
                break;
            case BTN_F3:
                menu_cfg.root=&menu1;
                mode=0;
                cops->start_menu(&menu_cfg);
                cops->menuEvtHandler(EVT_REDRAW);
                break;
        }
    }
    else
    {
        cops->menuEvtHandler(evt);
    }
    return 0;
}

/* called function from outside */
int main(int argc,char * * argv)
{
    int i,j;    
    
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();
    cops->disableMenu();
    
    menu1.data=(void*)"Menu1";
    menu2.data=(void*)"Menu2";
    menu3.data=(void*)"Menu3";
    
    menu1.prev=NULL;
    menu1.nxt=&menu2;
    menu1.sub=NULL;
    menu1.up=NULL;
    
    menu2.prev=&menu1;
    menu2.nxt=&menu3;
    menu2.sub=NULL;
    menu2.up=NULL;
    
    menu3.prev=&menu2;
    menu3.nxt=NULL;
    menu3.sub=NULL;
    menu3.up=NULL;
    
    cops->clearScreen(COLOR_WHITE);
    
    for(i=0;i<16;i++)
    {
        for(j=0;j<16;j++)
        {
            cops->fillRect(i*16+j, j*20 , i*15, 20, 15);                  
        }
    }
        
    PACK(cops,NULL)
    return 1;
}

