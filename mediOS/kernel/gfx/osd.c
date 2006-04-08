/*
*   kernel/gfx/osd.c
*
*   MediOS project
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
#include <kernel/lcd.h>


int osdLookupOffsetLO[4] = { OSD_SDRAM_OFF_VID0 ,
                             OSD_SDRAM_OFF_VID1 ,
							 OSD_SDRAM_OFF_BITMAP0 ,
							 OSD_SDRAM_OFF_BITMAP1 };
int osdLookupOffsetHI[4] = { OSD_SDRAM_OFF_HI_VID0_1 ,
                             OSD_SDRAM_OFF_HI_VID0_1 ,
							 OSD_SDRAM_OFF_HI_BITMAP0_1,
							 OSD_SDRAM_OFF_HI_BITMAP0_1 };
							 
int OSD_OFF_HI_SHIFT[4] = {0,8,0,8};

int osd_RGB2Packed(int r, int g, int b)
{
	return  ((RGB2Cr(r,g,b) << 16) | (RGB2Y(r,g,b) << 8) | RGB2Cb(r,g,b));
}

void osd_setCursor2Bitmap (int index, int data)
{
    int val;
    outw(data,OSD_CURSOR2_DATA);           /* Setup data reg */
    val=inw(OSD_CURSOR2_ADD_LATCH) & 0xFF;
    index=index<<8;
    outw(val | index | 0x80,OSD_CURSOR2_ADD_LATCH);       /* Set the data... */
}

void osd_setBorderColor (int color)
{
	outw((inw(OSD_CONF) & 0xFF00) | color,OSD_CONF);
}

void osd_setMainConfig (int config)
{
	outw(((inw(OSD_CONF) & 0xFF) << 8) | config,OSD_CONF);
}

void osd_setMainShift (int horizontal,int vertical)
{
	outw(horizontal,OSD_BITMAP0_SHIFT_HORIZ);
	outw(vertical,OSD_BITMAP0_SHIFT_VERT);
}

void osd_setEntirePalette(int palette[256][3],int size)
{
    int i=0;
    int y,cr,cb;
    for(i=0;i<size;i++)
    {
        y = (306*palette[i][0] + 601*palette[i][1] + 117*palette[i][2]) >> 10 ; 
        cb = ((-173*palette[i][0] -339*palette[i][1] + 512*palette[i][2]) >> 10) + 128;
        cr = ((512*palette[i][0] - 428*palette[i][1] - 84*palette[i][2]) >> 10) + 128;
    
        /*printk("[setPalette] (%03d) RGB=(%03d,%03d,%03d) YCrCb=(%03d,%03d,%03d)\n",i,
                    palette[i][0],palette[i][1],palette[i][2],
                    y,cr,cb);*/
        
        osd_setPalette (y, cr, cb, i);
    }
}

void osd_setPaletteRGB(int r,int g,int b,int index)
{
  osd_setPalette(RGB2Y(r,g,b), RGB2Cr(r,g,b),RGB2Cb(r,g,b),index);
}

void osd_setPalette (int Y, int Cr, int Cb, int index)
{
	Y&=0xFF;
        Cr&=0xFF;
        Cb&=0xFF;

        //printk("OSD set palette (%x,%x,%x) at %d\n",Y,Cr,Cb,index);

	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0) /* nothing */ ;
        outw((Y << 8) | Cb,OSD_PAL_DATA_WRITE);
	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0) /* nothing */ ;
        outw((Cr << 8) | index,OSD_PAL_INDEX_WRITE);
        while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0) /* nothing */ ;
}

void osd_set16CPalette (int bankN, int index, int value)
{
	int val=inw(GET_BANK_ADDR(bankN,index));
	val &= 0xFF00 >> GET_BANK_SHIFT(index);
	val |= value << GET_BANK_SHIFT(index);
	outw(val,GET_BANK_ADDR(bankN,index));
}


void osd_setAltOffset (int address)
{
	int offset = address - SDRAM_START;
	offset = offset >> 5;

	outw(offset,OSD_ALT_VID_OFF_HI);
	outw(offset>>16,OSD_ALT_VID_OFF_LO);
}

void osd_setComponentOffset (int component, int address)
{
	int val;
	int offset = address - SDRAM_START;

	offset = offset >> 5;
	outw(offset,osdLookupOffsetLO[component]);

	offset = offset >> 16;
	val=inw(osdLookupOffsetHI[component]);
	val &= 0xFF00 >> OSD_OFF_HI_SHIFT[component];
	val |= offset << OSD_OFF_HI_SHIFT[component];

	outw(val,osdLookupOffsetHI[component]);
}

void osd_setComponentSize (int component, int width, int height)
{
	outw(width,OSD_COMP_W(component));
	outw(height,OSD_COMP_H(component));
}

void osd_setComponentPosition (int component, int x, int y)
{
	outw(x,OSD_COMP_X(component));
	outw(y,OSD_COMP_Y(component));
}

void osd_setComponentSourceWidth (int component, int width)
{
	outw(width,OSD_COMP_BUFF_W(component));
}

void osd_setComponentConfig (int component, int config)
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

void osd_restorePlane(int component, unsigned int address, int x, int y, int w, int h, int bpp, int state,int enable)
{
    osd_setComponentOffset (component,address);

    outw(2*w,OSD_COMP_W(component));
    outw(h,OSD_COMP_H(component));

    outw(x,OSD_COMP_X(component));
    outw(y,OSD_COMP_Y(component));

    outw((((w*bpp)/32)/8),OSD_COMP_BUFF_W(component));

    if(enable)
    {
        switch(component)
        {
            case OSD_VIDEO1:
                outw((inl(OSD_VID0_1_CONF) & 0xFF00) | (state|OSD_COMPONENT_ENABLE),OSD_VID0_1_CONF);
                break;
            case OSD_VIDEO2:
                outw((inl(OSD_VID0_1_CONF) & 0xFF) | ((state|OSD_COMPONENT_ENABLE)<<8),OSD_VID0_1_CONF);
                break;
            default:
                outw((state|OSD_COMPONENT_ENABLE),OSD_COMP_CONF(component));
        }
    }
}

void osd_init()
{
  arch_osd_init();
}
