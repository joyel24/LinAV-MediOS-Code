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

#include <kernel/graphics.h>
#include <kernel/colordef.h>

void avwm(void)
{
    printk("Starting AvWm\n");
    
    open_graphics();
    clearScreen(COLOR_WHITE);
    drawRect(COLOR_RED,10,10,100,100);
    putS(COLOR_BLUE,COLOR_WHITE,0,0,"Hello world");
    while(1) {
        /*NOTHING*/
    }
}
