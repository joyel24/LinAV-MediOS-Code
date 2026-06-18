/*
*   lib/target/arch_AV3XX/osd.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stddef.h>

#include <io.h>
#include <hardware.h>
#include <kernel.h>
#include <osd.h>

void arch_init_osd(void)
{
    osdSetMainConfig(0x1);
    osdSetMainShift(0x8F, 0x15);
    
    outw(0x2182,PREVIEW_BASE); /* preview engine enable/diasble */
    outw(0x0400,PREVIEW_BASE+0x2);    
    
    //outw(0x14,0x30612);
}
