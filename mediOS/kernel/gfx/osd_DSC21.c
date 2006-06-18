/*
*   lib/osd.c
*
*   AvLo - linav project
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

int osd_comp_conf_tab[] = { OSD_VID0_1_CONF, 0,
                            OSD_BITMAP0_CONF, OSD_BITMAP1_CONF,
                            OSD_CURSOR0_CONF, 0
                          };
#define OSD_COMP_CONF(component)          (osd_comp_conf_tab[component])

int osd_comp_buff_w_tab[] = { OSD_VID0_BUFF_W, 0,
                              0, 0,
                              0, 0
                            };
#define OSD_COMP_BUFF_W(component)          (osd_comp_buff_w_tab[component])

int osd_comp_w_tab[] = { OSD_VID_W, 0,
                              OSD_BITMAP0_RIGHT, OSD_BITMAP1_RIGHT,
                              OSD_CURSOR_RIGHT, 0
                            };                            
#define OSD_COMP_W(component)          (osd_comp_w_tab[component])

int osd_comp_h_tab[] = { OSD_VID_H, 0,
                              OSD_BITMAP0_BOTTOM, OSD_BITMAP1_BOTTOM,
                              OSD_CURSOR_BOTTOM, 0
                            };                            
#define OSD_COMP_H(component)          (osd_comp_h_tab[component])

int osd_comp_x_tab[] = { 0, 0,
                              OSD_BITMAP0_LEFT, OSD_BITMAP1_LEFT,
                              OSD_CURSOR_LEFT, 0
                            };                            
#define OSD_COMP_X(component)          (osd_comp_x_tab[component])

int osd_comp_y_tab[] = { 0, 0,
                              OSD_BITMAP0_TOP, OSD_BITMAP1_TOP,
                              OSD_CURSOR_TOP, 0
                            };                            
#define OSD_COMP_Y(component)          (osd_comp_y_tab[component])

#define OSD_SDRAM_OFF_HI_VID0              (OSD_BASE+0x06)   // SDRAM offset HI Video0 (no Video1)
#define OSD_SDRAM_OFF_LO_VID0              (OSD_BASE+0x08)   // SDRAM offset video0
//#define OSD_SDRAM_OFF_VID1                (0)               // NO SDRAM offset video1
#define OSD_SDRAM_OFF_HI_CURSOR           (OSD_BASE+0x0a)   // SDRAM offset HI CURSOR 
#define OSD_SDRAM_OFF_LO_CURSOR           (OSD_BASE+0x0c)   // SDRAM offset LO CURSOR 
#define OSD_SDRAM_OFF_HI_BITMAP0          (OSD_BASE+0x0e)   // SDRAM offset HI Bitmap0
#define OSD_SDRAM_OFF_LO_BITMAP0          (OSD_BASE+0x10)   // SDRAM offset LO Bitmap0
#define OSD_SDRAM_OFF_HI_BITMAP1          (OSD_BASE+0x12)   // SDRAM offset HI Bitmap1
#define OSD_SDRAM_OFF_LO_BITMAP1          (OSD_BASE+0x14)   // SDRAM offset LO Bitmap1


int osdLookupOffsetHI[6] = {    OSD_SDRAM_OFF_HI_VID0   , 0 ,
                                OSD_SDRAM_OFF_HI_BITMAP0, OSD_SDRAM_OFF_HI_BITMAP1 ,
                                OSD_SDRAM_OFF_HI_CURSOR , 0 };
int osdLookupOffsetLO[6] = {    OSD_SDRAM_OFF_LO_VID0   , 0 ,
                                OSD_SDRAM_OFF_LO_BITMAP0, OSD_SDRAM_OFF_LO_BITMAP1 ,
                                OSD_SDRAM_OFF_LO_CURSOR , 0 };

int osd_RGB2Packed(int r, int g, int b)
{
	return  ((RGB2Cr(r,g,b) << 16) | (RGB2Y(r,g,b) << 8) | RGB2Cb(r,g,b));
}

void osd_setCursor2Bitmap (int index, int data)
{
    /* no palette in DSC21*/
}

void osd_setBorderColor (int color)
{
	//outw((inw(OSD_CONF) & 0xFF00) | color,OSD_CONF);
}

void osd_setMainConfig (int config)
{
	outw(config,OSD_CONF);
}

void osd_setMainShift (int horizontal,int vertical)
{
	outw(horizontal,OSD_MAIN_SHIFT_HORIZ);
	outw(vertical,OSD_MAIN_SHIFT_VERT);
}

void osd_setEntirePalette(int palette[256][3],int size)
{
    /* no palette in DSC21*/
}

void osd_setPaletteRGB(int r,int g, int b, int index)
{
    /* no palette in DSC21*/
}

void osd_setPallette (int Y, int Cr, int Cb, int index)
{
    /* no palette in DSC21*/
}

void osd_set16CPallette (int bankN, int index, int value)
{
    int val=inw(GET_BANK_ADDR(bankN,index));
    val &= 0xFF00 >> GET_BANK_SHIFT(index);
    val |= value << GET_BANK_SHIFT(index);
    outw(val,GET_BANK_ADDR(bankN,index));
}


void osd_setAltOffset (int address)
{
    /* No Alt offset for DSC21 or UKN */
}

void osd_setComponentOffset (int component, int address)
{
    address -= SDRAM_START;
    
    address >>= 5;
    outw(address,osdLookupOffsetLO[component]);

    address >>= 16;
    outw(address,osdLookupOffsetHI[component]);
}

void osd_setComponentSize (int component, int width, int height)
{
    if(component == OSD_VIDEO)
    {
        outw(width,OSD_COMP_W(component));
        outw(height,OSD_COMP_H(component));
    }
    else
    {
        int x = inw(OSD_COMP_X(component));
        int y = inw(OSD_COMP_Y(component));
        outw(width+x,OSD_COMP_W(component));
        outw(height+y,OSD_COMP_H(component));
    }
}

void osd_setComponentPosition (int component, int x, int y)
{
    if(component >= OSD_BITMAP1 && component <= OSD_CURSOR)
    {
        outw(x,OSD_COMP_X(component));
        outw(y,OSD_COMP_Y(component));
    }
}

void osd_setComponentSourceWidth (int component, int width)
{
#warning need to see what to do here !
    /*if(component == OSD_VIDEO)
    {
        outw(width,OSD_VID0_BUFF_W);
    }*/
}

void osd_setComponentConfig (int component, int config)
{
    if(component==OSD_VIDEO || (component >= OSD_BITMAP1 && component <= OSD_CURSOR))
        outw(config,OSD_COMP_CONF(component));
}

void osd_restorePlane(int component, unsigned int address, int x, int y, int w, int h, int bpp, int state,int enable)
{
    osd_setComponentOffset(component,address);
    osd_setComponentSize (component, 2*w, h);
    osd_setComponentPosition(component,x,y);
    osd_setComponentSourceWidth(component,(((w*bpp)/32)/8));
    
    if(enable)
    {
        osd_setComponentConfig(component,state|OSD_COMPONENT_ENABLE(component));
    }
}

void osd_init()
{
  arch_osd_init();
}
