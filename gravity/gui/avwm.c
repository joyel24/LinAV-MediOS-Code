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
#include <sys_def/colordef.h>
#include <sys_def/time.h>
#include <sys_def/font.h>
#include <api.h>
#include <fs_io.h>

#include <gui/icons.h>


extern void ini_status_bar(void);
extern void drawStatusLine(void);

void avwm(void)
{
    DIR * dir;
    struct dirent* entry;
    printf("Starting AvWm\n");
    
    open_graphics();
    clearScreen(COLOR_WHITE);
    setFont(STD6X9);
    iniIcon();
    
    ini_status_bar();
    drawStatusLine(); /* should be done via EVT_REDRAW */
    
    dir=opendir("/");
    while((entry=readdir(dir)))
    {
        printf("%s\n",entry->d_name);
    }
    
    while(1) /*printk(" a ")*/;
}
