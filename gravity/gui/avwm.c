/* 
*   gui/avwm.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <graphics.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/time.h>
#include <sys_def/font.h>
#include <api.h>
#include <fs_io.h>
#include <kernel/evt.h>

#include <gui/icons.h>
#include <gui/status_line.h>

void avwm(void)
{
    unsigned int evt_buffer;
    int evt;
    
     HTASK task;
    
    
    printf("Starting AvWm\n");
    
    open_graphics();
    clearScreen(COLOR_WHITE);
    setFont(STD6X9);
    iniIcon();
    
    ini_status_bar();
    ini_file_browser();
    
    evt_buffer=register_evt();
    if(!evt_buffer)
    {
        printf("[ini_status_bar] can't register to evt\n");
        return;
    } 
    
    //browse_root();
    
    printk("browser started\n");
    
    sendEvt(EVT_REDRAW);

    while(1)
    {
        evt=waitEvt(evt_buffer);
        
        bwseventHandler(evt);       
        statusEvtHandler(evt);
    }
}
