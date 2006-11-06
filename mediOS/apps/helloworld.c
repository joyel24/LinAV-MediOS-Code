/*
*   helloworld.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <api.h>

void app_main(int argc,char * * argv)
{
    gfx_clearScreen(COLOR_BLACK);
    
    gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Hello world");
    
    mdelay(1000);
    
}
