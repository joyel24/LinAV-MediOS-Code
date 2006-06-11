/*
*   kernel/target/arch_AV3XX/osd.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/osd.h>
#include <kernel/lcd.h>

void arch_osd_init(void)
{
    osd_setMainConfig(0x1400);
    osd_setMainShift(0x7d, 0x16);
    
    outw(0x0001,0x30700);
    outw(0x0000,0x30702);
    outw(0x0000,0x3071c);
    outw(0x0000,0x3071e);
    
   
    outw(0x0000,VIDEO_IO_BASE); /* VIDEO_IO engine enable/diasble */
    outw(0x0000,VIDEO_IO_BASE+0x2);    
    outw(0x0008,VIDEO_IO_BASE+0xc); /* horiz position*/
    outw(0x0280,VIDEO_IO_BASE+0xe); /* horiz size */
   
    outw(0x0008,VIDEO_IO_BASE+0x10); /* vert position*/
    outw(0x0200,VIDEO_IO_BASE+0x12); /* vert size */    
    outw(0x0081,VIDEO_IO_BASE+0x14);
    outw(0x0000,VIDEO_IO_BASE+0x16);
    outw(0x00D0,VIDEO_IO_BASE+0x18);
    outw(0x0000,VIDEO_IO_BASE+0x1a);
    outw(0x2000,VIDEO_IO_BASE+0x1c); 
    outw(0x0020,VIDEO_IO_BASE+0x1e); 
       
    outw(0x0000,VIDEO_IO_BASE+0x20);
    outw(0x2000,VIDEO_IO_BASE+0x22); 
    outw(0x0020,VIDEO_IO_BASE+0x24); 
    outw(0x0000,VIDEO_IO_BASE+0x26);
    outw(0x0000,VIDEO_IO_BASE+0x28);
    outw(0x0000,VIDEO_IO_BASE+0x2a);
    outw(0x0000,VIDEO_IO_BASE+0x2c);
    outw(0x0000,VIDEO_IO_BASE+0x2e);
    
    outw(0x0000,VIDEO_IO_BASE+0x30); 
    outw(0x0000,VIDEO_IO_BASE+0x32);
    outw(0x0000,VIDEO_IO_BASE+0x34);
    outw(0x0000,VIDEO_IO_BASE+0x36);
    outw(0x0000,VIDEO_IO_BASE+0x38);
    outw(0x0000,VIDEO_IO_BASE+0x3a);
    outw(0x0010,VIDEO_IO_BASE+0x3c);
    outw(0x0010,VIDEO_IO_BASE+0x3e);

    outw(0x0000,VIDEO_IO_BASE+0x40); 
    outw(0x0000,VIDEO_IO_BASE+0x42);
    outw(0x4000,VIDEO_IO_BASE+0x44);
    outw(0x0000,VIDEO_IO_BASE+0x46);
    outw(0x4000,VIDEO_IO_BASE+0x48);
    outw(0x0000,VIDEO_IO_BASE+0x4a);
    outw(0x4000,VIDEO_IO_BASE+0x4c);
    outw(0x0000,VIDEO_IO_BASE+0x4e);    
    
    outw(0x0000,VIDEO_IO_BASE+0x50); 
    outw(0x0000,VIDEO_IO_BASE+0x52);
    outw(0x0008,VIDEO_IO_BASE+0x54);
    outw(0xe743,VIDEO_IO_BASE+0x56);
    outw(0x0d37,VIDEO_IO_BASE+0x58);
    outw(0x8d3c,VIDEO_IO_BASE+0x5a);
    outw(0x4730,VIDEO_IO_BASE+0x5c);
    outw(0x8a00,VIDEO_IO_BASE+0x5e);
    
    outw(0x0000,VIDEO_IO_BASE+0x60); 
    outw(0x0000,VIDEO_IO_BASE+0x62);
    outw(0x1100,VIDEO_IO_BASE+0x64);
    outw(0x0000,VIDEO_IO_BASE+0x66);
    
    outw(0xeb10,VIDEO_IO_BASE+0x6a);
    outw(0xeb10,VIDEO_IO_BASE+0x6c);
    
    outw(0x4075,VIDEO_BASE+0x00);
    outw(0x6C00,VIDEO_BASE+0x02);
}
