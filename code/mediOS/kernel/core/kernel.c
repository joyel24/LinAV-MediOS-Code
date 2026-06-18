/* 
*   kernel/core/kernel.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/hardware.h>
#include <kernel/target/arch/lcd.h>
#include <kernel/kernel.h>

int getArch(void)
{
    return CURRENT_ARCH;
}

void getResolution(int * width,int * height)
{
    if(width) *width=SCREEN_REAL_WIDTH;
    if(height) *height=SCREEN_HEIGHT;
}
