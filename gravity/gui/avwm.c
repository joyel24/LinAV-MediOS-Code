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
#include <api.h>


void avwm(void)
{
    printf("Starting AvWm\n");
    
    open_graphics();
    clearScreen(COLOR_WHITE);
    
    iniIcon();
    
    ini_status_bar();
    drawStatusLine(); /* should be done via EVT_REDRAW */
    
    while(1) /*NOTHING*/;
}
