/*
* arch/armnommu/mach-av3xx/av3xx_osd.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_osd.h>

u32 osdLookupOffsetLO[4] = { AV3XX_OSD_SDRAM_OFF_VID0 ,
                             AV3XX_OSD_SDRAM_OFF_VID1 ,
							 AV3XX_OSD_SDRAM_OFF_BITMAP0 ,
							 AV3XX_OSD_SDRAM_OFF_BITMAP1 };
u32 osdLookupOffsetHI[4] = { AV3XX_OSD_SDRAM_OFF_HI_VID0_1 ,
                             AV3XX_OSD_SDRAM_OFF_HI_VID0_1 ,
							 AV3XX_OSD_SDRAM_OFF_HI_BITMAP0_1,
							 AV3XX_OSD_SDRAM_OFF_HI_BITMAP0_1 };


void osdSetCursor2Bitmap (u16 index, u16 data)
{
	outw(data,AV3XX_OSD_CURSOR2_DATA);           /* Setup data reg */

	outw((((inw(AV3XX_OSD_CURSOR2_ADD_LATCH) & 0xFF) << 8) | index) | 0x80,
			AV3XX_OSD_CURSOR2_ADD_LATCH);       /* Set the data... */
}

void osdSetBorderColor (u16 color)
{
	outw((inw(AV3XX_OSD_CONF) & 0xFF00) | color,AV3XX_OSD_CONF);
}

void osdSetMainConfig (u16 config)
{
	outw(((inw(AV3XX_OSD_CONF) & 0xFF) << 8) | config,AV3XX_OSD_CONF);
}

void osdSetMainShift (u16 horizontal,u16 vertical)
{
	outw(horizontal,AV3XX_OSD_BITMAP0_SHIFT_HORIZ);
	outw(vertical,AV3XX_OSD_BITMAP0_SHIFT_VERT);
}

void osdSetPallette (u16 Y, u16 Cr, u16 Cb, u16 index)
{
	int val;
	while((inw(AV3XX_OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;

	outw((Cr << 8) | index,AV3XX_OSD_PAL_INDEX_WRITE);

	while((inw(AV3XX_OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;

	outw((Y << 8) | Cb,AV3XX_OSD_PAL_DATA_WRITE);
}

void osdSet16CPallete (int bankN, u16 index, u16 value)
{
	u32 val=inw(AV3XX_GET_BANK_ADDR(bankN,index));
	val &= 0xFF00 >> AV3XX_GET_BANK_SHIFT(index);
	val |= value << AV3XX_GET_BANK_SHIFT(index);
	outw(val,AV3XX_GET_BANK_ADDR(bankN,index));
}

void osdSetAltOffset (u32 address)
{
	u32 offset = address - 0x03000000;
	offset = offset >> 5;

	outw(offset,AV3XX_OSD_ALT_VID_OFF_HI);
	outw(offset>>16,AV3XX_OSD_ALT_VID_OFF_LO);
}

void osdSetComponentOffset (int component, u32 address)
{
	u32 val;
	u32 offset = address - 0x03000000;

	offset = offset >> 5;
	outw(offset,osdLookupOffsetLO[component]);

	offset = offset >> 16;
	val=inw(osdLookupOffsetHI[component]);
	val &= 0xFF00 >> AV3XX_OSD_OFF_HI_SHIFT(component);
	val |= offset << AV3XX_OSD_OFF_HI_SHIFT(component);

	outw(val,osdLookupOffsetHI[component]);
}

void osdSetComponentSize (int component, u16 width, u16 height)
{
	outw(width,AV3XX_OSD_COMP_W(component));
	outw(height,AV3XX_OSD_COMP_H(component));
}

void osdSetComponentPosition (int component, u16 x, u16 y)
{
	outw(x,AV3XX_OSD_COMP_X(component));
	outw(y,AV3XX_OSD_COMP_Y(component));
}

void osdSetComponentSourceWidth (int component, u16 width)
{
	outw(width,AV3XX_OSD_COMP_BUFF_W(component));
}

void osdSetComponentConfig (int component, u16 config)
{
	if(component == AV3XX_OSD_VIDEO1)
	{
		outw((inl(AV3XX_OSD_VID0_1_CONF) & 0xFF00) | config,AV3XX_OSD_VID0_1_CONF);
	}
	else if(component == AV3XX_OSD_VIDEO2)
	{
		outw((inl(AV3XX_OSD_VID0_1_CONF) & 0xFF) | (config<<8),AV3XX_OSD_VID0_1_CONF);
	}
	else
	{
		outw(config,AV3XX_OSD_COMP_CONF(component));
	}
}

void osdSetBacklight(int val) /* val = 0/1 */
{
	if(val)
		outw(4,AV3XX_LCD_BACK_LIGHT);
	else
		outw(0,AV3XX_LCD_BACK_LIGHT);
}

void osdInit()
{
	osdSetMainConfig(0x2000);
	osdSetMainShift(0x79, 0x00);

	outw(0x4071,AV3XX_VIDEO_BASE);
	outw(0x0f40,AV3XX_VIDEO_BASE+0x2);
	outw(0x8100,AV3XX_VIDEO_BASE+0x4);
	outw(0x4c0f,AV3XX_VIDEO_BASE+0x6);
	outw(0xaaaa,AV3XX_VIDEO_BASE+0x8);
	outw(0x0000,AV3XX_VIDEO_BASE+0xa);
	outw(0x0000,AV3XX_VIDEO_BASE+0xc);
	outw(0x0000,AV3XX_VIDEO_BASE+0xe);
	outw(0x0000,AV3XX_VIDEO_BASE+0xa);
	outw(0x0000,AV3XX_VIDEO_BASE+0x10);
	outw(0x0000,AV3XX_VIDEO_BASE+0x12);
	outw(0x0000,AV3XX_VIDEO_BASE+0x14);
	outw(0x0000,AV3XX_VIDEO_BASE+0x16);
	outw(0x0000,AV3XX_VIDEO_BASE+0x18);
	outw(0x02e0,AV3XX_VIDEO_BASE+0x1a);
	outw(0x0085,AV3XX_VIDEO_BASE+0x1c); // X offset for display
	outw(0x0280,AV3XX_VIDEO_BASE+0x1e); // WIDTH in pixels (Number processed)
	outw(0x0103,AV3XX_VIDEO_BASE+0x20);
	outw(0x0013,AV3XX_VIDEO_BASE+0x22); // Y offset for display
	outw(0x00f0,AV3XX_VIDEO_BASE+0x24); // HEIGHT in pixels (number processed)
	outw(0x0000,AV3XX_VIDEO_BASE+0x26);
	outw(0x0000,AV3XX_VIDEO_BASE+0x28);
	outw(0x0000,AV3XX_VIDEO_BASE+0x2a);
	outw(0x8001,AV3XX_VIDEO_BASE+0x2c);
	outw(0x0002,AV3XX_VIDEO_BASE+0x2e);
	outw(0xff00,AV3XX_VIDEO_BASE+0x30); // Brightness adder (lo)
	outw(0x0000,AV3XX_VIDEO_BASE+0x32);
	outw(0x0000,AV3XX_VIDEO_BASE+0x34);
	outw(0x0000,AV3XX_VIDEO_BASE+0x36);
	outw(0x0022,AV3XX_VIDEO_BASE+0x38);

	outw(0x0004,AV3XX_LCD_BACK_LIGHT);

	outw(0x8100,AV3XX_VIDEO_BASE+0x4);

	outw(0x0004,AV3XX_LCD_BACK_LIGHT);
}
