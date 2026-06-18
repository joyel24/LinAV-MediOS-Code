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

#include "osd.h"

int osdLookupOffsetLO[4] = { AV3XX_OSD_SDRAM_OFF_VID0 ,
                             AV3XX_OSD_SDRAM_OFF_VID1 ,
							 AV3XX_OSD_SDRAM_OFF_BITMAP0 ,
							 AV3XX_OSD_SDRAM_OFF_BITMAP1 };
int osdLookupOffsetHI[4] = { AV3XX_OSD_SDRAM_OFF_HI_VID0_1 ,
                             AV3XX_OSD_SDRAM_OFF_HI_VID0_1 ,
							 AV3XX_OSD_SDRAM_OFF_HI_BITMAP0_1,
							 AV3XX_OSD_SDRAM_OFF_HI_BITMAP0_1 };
							 
int AV3XX_OSD_OFF_HI_SHIFT[4] = {0,8,0,8};

int osdRGB2Packed(int r, int g, int b)
{
	int y = (306*r + 601*g + 117*b) >> 10 ; 
	int cb = ((-173*r -339*g + 512*b) >> 10) + 128;
	int cr = ((512*r - 429*g - 83*b) >> 10) + 128;

	return  (cr << 16) | (y << 8) | cb;
}

void osdSetCursor2Bitmap (int index, int data)
{
    int val;
    OUTW(data,AV3XX_OSD_CURSOR2_DATA);           /* Setup data reg */
    val=INW(AV3XX_OSD_CURSOR2_ADD_LATCH) & 0xFF;
    index=index<<8;
    OUTW(val | index | 0x80,AV3XX_OSD_CURSOR2_ADD_LATCH);       /* Set the data... */
}

void osdSetBorderColor (int color)
{
	OUTW((INW(AV3XX_OSD_CONF) & 0xFF00) | color,AV3XX_OSD_CONF);
}

void osdSetMainConfig (int config)
{
	OUTW(((INW(AV3XX_OSD_CONF) & 0xFF) << 8) | config,AV3XX_OSD_CONF);
}

void osdSetMainShift (int horizontal,int vertical)
{
	OUTW(horizontal,AV3XX_OSD_BITMAP0_SHIFT_HORIZ);
	OUTW(vertical,AV3XX_OSD_BITMAP0_SHIFT_VERT);
}

void osdSetPallette (int Y, int Cr, int Cb, int index)
{
	Y&=0xFF;
        Cr&=0xFF;
        Cb&=0xFF;
	while((INW(AV3XX_OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;
        OUTW((Y << 8) | Cb,AV3XX_OSD_PAL_DATA_WRITE);
	//OUTW((Cr << 8) | index,AV3XX_OSD_PAL_INDEX_WRITE);

	while((INW(AV3XX_OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;

	//OUTW((Y << 8) | Cb,AV3XX_OSD_PAL_DATA_WRITE);
        OUTW((Cr << 8) | index,AV3XX_OSD_PAL_INDEX_WRITE);
        
        while((INW(AV3XX_OSD_PAL_ACCESS_STATUS)&0x1) != 0)
        /* nothing */ ;
}

void osdSet16CPallette (int bankN, int index, int value)
{
	int val=INW(AV3XX_GET_BANK_ADDR(bankN,index));
	val &= 0xFF00 >> AV3XX_GET_BANK_SHIFT(index);
	val |= value << AV3XX_GET_BANK_SHIFT(index);
	OUTW(val,AV3XX_GET_BANK_ADDR(bankN,index));
}

void osdSetAltOffset (int address)
{
	int offset = address - 0x03000000;
	offset = offset >> 5;

	OUTW(offset,AV3XX_OSD_ALT_VID_OFF_HI);
	OUTW(offset>>16,AV3XX_OSD_ALT_VID_OFF_LO);
}

void osdSetComponentOffset (int component, int address)
{
	int val;
	int offset = address - 0x03000000;

	offset = offset >> 5;
	OUTW(offset,osdLookupOffsetLO[component]);

	offset = offset >> 16;
	val=INW(osdLookupOffsetHI[component]);
	val &= 0xFF00 >> AV3XX_OSD_OFF_HI_SHIFT[component];
	val |= offset << AV3XX_OSD_OFF_HI_SHIFT[component];

	OUTW(val,osdLookupOffsetHI[component]);
}

void osdSetComponentSize (int component, int width, int height)
{
	OUTW(width,AV3XX_OSD_COMP_W(component));
	OUTW(height,AV3XX_OSD_COMP_H(component));
}

void osdSetComponentPosition (int component, int x, int y)
{
	OUTW(x,AV3XX_OSD_COMP_X(component));
	OUTW(y,AV3XX_OSD_COMP_Y(component));
}

void osdSetComponentSourceWidth (int component, int width)
{
	OUTW(width,AV3XX_OSD_COMP_BUFF_W(component));
}

void osdSetComponentConfig (int component, int config)
{
	if(component == AV3XX_OSD_VIDEO1)
	{
		OUTW((INL(AV3XX_OSD_VID0_1_CONF) & 0xFF00) | config,AV3XX_OSD_VID0_1_CONF);
	}
	else if(component == AV3XX_OSD_VIDEO2)
	{
		OUTW((INL(AV3XX_OSD_VID0_1_CONF) & 0xFF) | (config<<8),AV3XX_OSD_VID0_1_CONF);
	}
	else
	{
		OUTW(config,AV3XX_OSD_COMP_CONF(component));
	}
}

void osdSetBacklight(int val) /* val = 0/1 */
{
	if(val)
		OUTW(4,AV3XX_LCD_BACK_LIGHT);
	else
		OUTW(0,AV3XX_LCD_BACK_LIGHT);
}

void osdInit()
{
	osdSetMainConfig(0x2000);
	osdSetMainShift(0x79, 0x00);

	OUTW(0x4071,AV3XX_VIDEO_BASE);
	OUTW(0x0f40,AV3XX_VIDEO_BASE+0x2);
	OUTW(0x8100,AV3XX_VIDEO_BASE+0x4);
	OUTW(0x4c0f,AV3XX_VIDEO_BASE+0x6);
	OUTW(0xaaaa,AV3XX_VIDEO_BASE+0x8);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0xa);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0xc);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0xe);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0xa);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x10);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x12);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x14);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x16);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x18);
	OUTW(0x02e0,AV3XX_VIDEO_BASE+0x1a);
	OUTW(0x0085,AV3XX_VIDEO_BASE+0x1c); // X offset for display
	OUTW(0x0280,AV3XX_VIDEO_BASE+0x1e); // WIDTH in pixels (Number processed)
	OUTW(0x0103,AV3XX_VIDEO_BASE+0x20);
	OUTW(0x0013,AV3XX_VIDEO_BASE+0x22); // Y offset for display
	OUTW(0x00f0,AV3XX_VIDEO_BASE+0x24); // HEIGHT in pixels (number processed)
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x26);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x28);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x2a);
	OUTW(0x8001,AV3XX_VIDEO_BASE+0x2c);
	OUTW(0x0002,AV3XX_VIDEO_BASE+0x2e);
	OUTW(0xff00,AV3XX_VIDEO_BASE+0x30); // Brightness adder (lo)
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x32);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x34);
	OUTW(0x0000,AV3XX_VIDEO_BASE+0x36);
	OUTW(0x0022,AV3XX_VIDEO_BASE+0x38);

	OUTW(0x0004,AV3XX_LCD_BACK_LIGHT);

	OUTW(0x8100,AV3XX_VIDEO_BASE+0x4);

	OUTW(0x0004,AV3XX_LCD_BACK_LIGHT);
}
