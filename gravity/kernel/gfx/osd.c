/* 
*   kernel/gfx/osd.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/osd.h>

int osdLookupOffsetLO[4] = { OSD_SDRAM_OFF_VID0 ,
                             OSD_SDRAM_OFF_VID1 ,
							 OSD_SDRAM_OFF_BITMAP0 ,
							 OSD_SDRAM_OFF_BITMAP1 };
int osdLookupOffsetHI[4] = { OSD_SDRAM_OFF_HI_VID0_1 ,
                             OSD_SDRAM_OFF_HI_VID0_1 ,
							 OSD_SDRAM_OFF_HI_BITMAP0_1,
							 OSD_SDRAM_OFF_HI_BITMAP0_1 };
							 
int OSD_OFF_HI_SHIFT[4] = {0,8,0,8};

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
    outw(data,OSD_CURSOR2_DATA);           /* Setup data reg */
    val=inw(OSD_CURSOR2_ADD_LATCH) & 0xFF;
    index=index<<8;
    outw(val | index | 0x80,OSD_CURSOR2_ADD_LATCH);       /* Set the data... */
}

void osdSetBorderColor (int color)
{
	outw((inw(OSD_CONF) & 0xFF00) | color,OSD_CONF);
}

void osdSetMainConfig (int config)
{
	outw(((inw(OSD_CONF) & 0xFF) << 8) | config,OSD_CONF);
}

void osdSetMainShift (int horizontal,int vertical)
{
	outw(horizontal,OSD_BITMAP0_SHIFT_HORIZ);
	outw(vertical,OSD_BITMAP0_SHIFT_VERT);
}

void osdSetPallette (int Y, int Cr, int Cb, int index)
{
	Y&=0xFF;
        Cr&=0xFF;
        Cb&=0xFF;
	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;
        outw((Y << 8) | Cb,OSD_PAL_DATA_WRITE);
	//outw((Cr << 8) | index,OSD_PAL_INDEX_WRITE);

	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0)
		/* nothing */ ;

	//outw((Y << 8) | Cb,OSD_PAL_DATA_WRITE);
        outw((Cr << 8) | index,OSD_PAL_INDEX_WRITE);
        
        while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0)
        /* nothing */ ;
}

void osdSet16CPallette (int bankN, int index, int value)
{
	int val=inw(GET_BANK_ADDR(bankN,index));
	val &= 0xFF00 >> GET_BANK_SHIFT(index);
	val |= value << GET_BANK_SHIFT(index);
	outw(val,GET_BANK_ADDR(bankN,index));
}

void osdSetAltOffset (int address)
{
	int offset = address - 0x03000000;
	offset = offset >> 5;

	outw(offset,OSD_ALT_VID_OFF_HI);
	outw(offset>>16,OSD_ALT_VID_OFF_LO);
}

void osdSetComponentOffset (int component, int address)
{
	int val;
	int offset = address - 0x03000000;

	offset = offset >> 5;
	outw(offset,osdLookupOffsetLO[component]);

	offset = offset >> 16;
	val=inw(osdLookupOffsetHI[component]);
	val &= 0xFF00 >> OSD_OFF_HI_SHIFT[component];
	val |= offset << OSD_OFF_HI_SHIFT[component];

	outw(val,osdLookupOffsetHI[component]);
}

void osdSetComponentSize (int component, int width, int height)
{
	outw(width,OSD_COMP_W(component));
	outw(height,OSD_COMP_H(component));
}

void osdSetComponentPosition (int component, int x, int y)
{
	outw(x,OSD_COMP_X(component));
	outw(y,OSD_COMP_Y(component));
}

void osdSetComponentSourceWidth (int component, int width)
{
	outw(width,OSD_COMP_BUFF_W(component));
}

void osdSetComponentConfig (int component, int config)
{
	if(component == OSD_VIDEO1)
	{
		outw((inl(OSD_VID0_1_CONF) & 0xFF00) | config,OSD_VID0_1_CONF);
	}
	else if(component == OSD_VIDEO2)
	{
		outw((inl(OSD_VID0_1_CONF) & 0xFF) | (config<<8),OSD_VID0_1_CONF);
	}
	else
	{
		outw(config,OSD_COMP_CONF(component));
	}
}

void osdSetBacklight(int val) /* val = 0/1 */
{
	if(val)
		outw(4,LCD_BACK_LIGHT);
	else
		outw(0,LCD_BACK_LIGHT);
}

void osdInit()
{
	osdSetMainConfig(0x2000);
	osdSetMainShift(0x79, 0x00);

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
