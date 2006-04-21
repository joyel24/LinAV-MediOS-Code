/*
* kernel/gui/gfx/gui.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <kernel/kernel.h>
#include <sys_def/stddef.h>

#include <kernel/evt.h>
#include <kernel/graphics.h>

#include <gui/gui.h>

#include <gui/menu.h>
#include <gui/main_menu.h>
#include <gui/icons.h>
#include <gui/file_browser.h>

void gui_start(void)
{
    int evt_hand,evt;
    
    iniIcon();
    iniBrowser();
    gfx_openGraphics();
    
    if(mainMenu_ini()<0)
        while(1) /*nothing*/;
    mainMenu_start();
    
    statusBar_ini();
    
    mainMenu_loop();
    
    
    
    /* no way to get here atm ... 
    but should we  ? */
    
    mainMenu_dispose();
    
    gfx_closeGraphics();
}


void gui_sendEvt(int evt_num)
{
    struct evt_t evt;
    evt.evt=evt_num;
    evt.evt_class=GUI_CLASS;
    evt.data=NULL;
    evt_send(&evt);
}
