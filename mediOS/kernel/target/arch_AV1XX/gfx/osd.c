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
	osd_setMainConfig(0x2000);
	osd_setMainShift(0x79, 0x00);

	outw(0x4071,VIDEO_BASE);
	outw(0x0f40,VIDEO_BASE+0x2);
	outw(0x8100,VIDEO_BASE+0x4);
	outw(0x4c0f,VIDEO_BASE+0x6);
	outw(0xaaaa,VIDEO_BASE+0x8);
	outw(0x0000,VIDEO_BASE+0xa);
	outw(0x0000,VIDEO_BASE+0xc);
	outw(0x0000,VIDEO_BASE+0xe);
	outw(0x0000,VIDEO_BASE+0xa);
	outw(0x0000,VIDEO_BASE+0x10);
	outw(0x0000,VIDEO_BASE+0x12);
	outw(0x0000,VIDEO_BASE+0x14);
	outw(0x0000,VIDEO_BASE+0x16);
	outw(0x0000,VIDEO_BASE+0x18);
	outw(0x02e0,VIDEO_BASE+0x1a);
	outw(0x0085,VIDEO_BASE+0x1c); // X offset for display
	outw(0x0280,VIDEO_BASE+0x1e); // WIDTH in pixels (Number processed)
	outw(0x0103,VIDEO_BASE+0x20);
	outw(0x0013,VIDEO_BASE+0x22); // Y offset for display
	outw(0x00f0,VIDEO_BASE+0x24); // HEIGHT in pixels (number processed)
	outw(0x0000,VIDEO_BASE+0x26);
	outw(0x0000,VIDEO_BASE+0x28);
	outw(0x0000,VIDEO_BASE+0x2a);
	outw(0x8001,VIDEO_BASE+0x2c);
	outw(0x0002,VIDEO_BASE+0x2e);
	outw(0xff00,VIDEO_BASE+0x30); // Brightness adder (lo)
	outw(0x0000,VIDEO_BASE+0x32);
	outw(0x0000,VIDEO_BASE+0x34);
	outw(0x0000,VIDEO_BASE+0x36);
	outw(0x0022,VIDEO_BASE+0x38);

	outw(0x0004,LCD_BACK_LIGHT);

	outw(0x8100,VIDEO_BASE+0x4);

	outw(0x0004,LCD_BACK_LIGHT);
}
