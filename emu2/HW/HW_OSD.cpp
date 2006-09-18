/*
*   HW_OSD.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <HW_OSD.h>

#include <cmd_line.h>

HW_OSD * hw_OSD;

void init_mem_static_fct(HW_OSD * osd);

#define SDRAM2OSD(VAR)    ((VAR&(~SDRAM_START))


HW_OSD::HW_OSD(HW_mem * mem2):HW_access(OSD_START,OSD_END,"OSD")
{
    init_mem_static_fct(this);
    
    for(int i=0;i<6;i++)
    {
        OSD_config_regs[i]=0;
        
        OSD_info_regs[i].x=0;
        OSD_info_regs[i].y=0;
        OSD_info_regs[i].width=0;
        OSD_info_regs[i].height=0;
    }
    
    for(int i=0;i<4;i++)
    {  
        OSD_width_regs[i]=0;
        
        OSD_offset_regs[i]=SDRAM_START;
    }
    
    for(int i=0;i<8;i++)
    {
        OSD_pallette_bank1[i] = 0;
        OSD_pallette_bank2[i] = 0;
    }
    
    OSD_main_shift_horiz=0;
    OSD_main_shift_vert=0;
    OSD_cursor_data=0;
    OSD_cursor_offset=0;
    OSD_pallette_status=0;
    OSD_pallette_data_wr=0;
    OSD_pallette_index=0;
    OSD_alt_vid_offset=0;
    
    lcd = new HW_lcd(mem2,this);
}

#include <osd_cmd_line_fct.h>

uint32_t HW_OSD::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
    switch(addr)
    {
        case OSD_START+0x0:
            ret_val=OSD_config_regs[0];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Main config",ret_val,size);            
            break;
        case OSD_START+0x2:
            ret_val=OSD_config_regs[1];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 & Vid1 config",ret_val,size);            
            break;
        case OSD_START+0x4:
            ret_val=OSD_config_regs[2];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 config",ret_val,size);            
            break;
        case OSD_START+0x6:
            ret_val=OSD_config_regs[3];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 config",ret_val,size);            
            break;
        case OSD_START+0x8:
            ret_val=OSD_config_regs[4];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 config",ret_val,size);            
            break;
        case OSD_START+0xA:
            ret_val=OSD_config_regs[5];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor1 config",ret_val,size);            
            break;
            
        case OSD_START+0xC:
            ret_val=OSD_width_regs[0];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 buffer width",ret_val,size);            
            break;
        case OSD_START+0xE:
            ret_val=OSD_width_regs[1];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 buffer width",ret_val,size);            
            break;
        case OSD_START+0x10:
            ret_val=OSD_width_regs[2];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 buffer width",ret_val,size);            
            break;
        case OSD_START+0x12:
            ret_val=OSD_width_regs[3];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 buffer width",ret_val,size);            
            break;
            
        case OSD_START+0x14:
            ret_val=((((OSD_offset_regs[1]-SDRAM_START)>>21)&0x3F)<<8) | (((OSD_offset_regs[0]-SDRAM_START)>>21)&0x3F);
// removing the SDRAM base addr. 0x03000000
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid0=%x Vid1=%x)\n",name,"Vid0 & Vid1 buffer HI offset",
                ret_val,size,OSD_offset_regs[0],OSD_offset_regs[1]);            
            break;        
        case OSD_START+0x16:
            ret_val=((OSD_offset_regs[0]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid0=%x)\n",name,"Vid0 buffer LO offset",
                ret_val,size,OSD_offset_regs[0]);            
            break;        
        case OSD_START+0x18:
            ret_val=((OSD_offset_regs[1]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid1=%x)\n",name,"Vid1 buffer LO offset",
                ret_val,size,OSD_offset_regs[1]);   
            break;
        case OSD_START+0x1A:
            ret_val=((((OSD_offset_regs[3]-SDRAM_START)>>21)&0x3F)<<8) | (((OSD_offset_regs[2]-SDRAM_START)>>21)&0x3F);
     // removing the SDRAM base addr. 0x03000000
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Bmap0=%x Bmap1=%x)\n",name,
                "Bmap0 & Bmap1 buffer HI offset",
                   ret_val,size,OSD_offset_regs[2],OSD_offset_regs[3]);            
            break;        
        case OSD_START+0x1C:
            ret_val=((OSD_offset_regs[2]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) Bmap0=%x)\n",name,"Bmap0 buffer LO offset",
                ret_val,size,OSD_offset_regs[2]);            
            break;        
        case OSD_START+0x1E:
            ret_val=((OSD_offset_regs[3]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) Bmap1=%x)\n",name,"Bmap1 buffer LO offset",
                ret_val,size,OSD_offset_regs[3]);            
            break;
            
        case OSD_START+0x20:
            ret_val=OSD_main_shift_horiz;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"main shift HORIZ",ret_val,size);            
            break;
        case OSD_START+0x22:
            ret_val=OSD_main_shift_vert;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"main shift VERT",ret_val,size);            
            break;
            
        case OSD_START+0x24:
            ret_val=OSD_info_regs[0].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 X",ret_val,size);            
            break;
        case OSD_START+0x26:
            ret_val=OSD_info_regs[0].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 Y",ret_val,size);            
            break;
        case OSD_START+0x28:
            ret_val=OSD_info_regs[0].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 width",ret_val,size);            
            break;
        case OSD_START+0x2A:
            ret_val=OSD_info_regs[0].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 height",ret_val,size);            
            break;
        case OSD_START+0x2C:
            ret_val=OSD_info_regs[1].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 X",ret_val,size);            
            break;
        case OSD_START+0x2E:
            ret_val=OSD_info_regs[1].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 Y",ret_val,size);            
            break;
        case OSD_START+0x30:
            ret_val=OSD_info_regs[1].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 width",ret_val,size);            
            break;
        case OSD_START+0x32:
            ret_val=OSD_info_regs[1].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 height",ret_val,size);            
            break;
        case OSD_START+0x34:
            ret_val=OSD_info_regs[2].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 X",ret_val,size);            
            break;
        case OSD_START+0x36:
            ret_val=OSD_info_regs[2].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 Y",ret_val,size);            
            break;
        case OSD_START+0x38:
            ret_val=OSD_info_regs[2].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 width",ret_val,size);            
            break;
        case OSD_START+0x3A:
            ret_val=OSD_info_regs[2].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 height",ret_val,size);            
            break;
        case OSD_START+0x3C:
            ret_val=OSD_info_regs[3].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 X",ret_val,size);            
            break;
        case OSD_START+0x3E:
            ret_val=OSD_info_regs[3].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 Y",ret_val,size);            
            break;
        case OSD_START+0x40:
            ret_val=OSD_info_regs[3].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 width",ret_val,size);            
            break;
        case OSD_START+0x42:
            ret_val=OSD_info_regs[3].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 height",ret_val,size);            
            break;
        case OSD_START+0x44:
            ret_val=OSD_info_regs[4].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 X",ret_val,size);            
            break;
        case OSD_START+0x46:
            ret_val=OSD_info_regs[4].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 Y",ret_val,size);            
            break;
        case OSD_START+0x48:
            ret_val=OSD_info_regs[4].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 width",ret_val,size);            
            break;
        case OSD_START+0x4A:
            ret_val=OSD_info_regs[4].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 height",ret_val,size);            
            break;
        case OSD_START+0x4C:
            ret_val=OSD_info_regs[5].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor1 X",ret_val,size);            
            break;
        case OSD_START+0x4E:
            ret_val=OSD_info_regs[5].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor1 Y",ret_val,size);            
            break;
            
        case OSD_START+0x50:
        case OSD_START+0x52:
        case OSD_START+0x54:
        case OSD_START+0x56:
        case OSD_START+0x58:
        case OSD_START+0x5A:
        case OSD_START+0x5C:
        case OSD_START+0x5E:
        {
            int index=(addr-OSD_START+0x50)/2;
            ret_val=OSD_pallette_bank1[index];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d read => %x (size %x)\n",name,"Palette Bank1",index,ret_val,size);            
            break;
        }
        case OSD_START+0x60:
        case OSD_START+0x62:
        case OSD_START+0x64:
        case OSD_START+0x66:
        case OSD_START+0x68:
        case OSD_START+0x6A:
        case OSD_START+0x6C:
        case OSD_START+0x6E:
        {
            int index=(addr-OSD_START+0x60)/2;
            ret_val=OSD_pallette_bank2[index];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d read => %x (size %x)\n",name,"Palette Bank2",index,ret_val,size);            
            break;
        }
        
        case OSD_START+0x72:
            ret_val=OSD_cursor_data;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor data",ret_val,size);            
            break;
        case OSD_START+0x74:
            ret_val=OSD_pallette_status;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Pallette status & cursor data",ret_val,size);
            break;
        case OSD_START+0x76:
            ret_val=OSD_pallette_data_wr;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Pallette data write",ret_val,size);            
            break;
        case OSD_START+0x78:
            ret_val=OSD_pallette_index;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Pallette data & index",ret_val,size);            
            break;
        case OSD_START+0x7C:
            ret_val=((OSD_alt_vid_offset-SDRAM_START)>>21)&0x3F;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (OSD ALT=%x)\n",name,"OSD ALT buffer HI",
                ret_val,size,OSD_alt_vid_offset);            
            break;
        case OSD_START+0x7E:
            ret_val=((OSD_alt_vid_offset-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (OSD ALT=%x)\n",name,"Pallette data & index",
                ret_val,size,OSD_alt_vid_offset);            
            break;
        
    }
    return ret_val;
}

void HW_OSD::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr)
    {
        case OSD_START+0x0:
            OSD_config_regs[0]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Main config",val,size);            
            break;
        case OSD_START+0x2:
            OSD_config_regs[1]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 & Vid1 config",val,size);            
            break;
        case OSD_START+0x4:
            OSD_config_regs[2]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 config",val,size);            
            break;
        case OSD_START+0x6:
            OSD_config_regs[3]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 config",val,size);            
            break;
        case OSD_START+0x8:
            OSD_config_regs[4]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 config",val,size);            
            break;
        case OSD_START+0xA:
            OSD_config_regs[5]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor1 config",val,size);            
            break;
            
        case OSD_START+0xC:
            OSD_width_regs[0]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 buffer width",val,size);            
            break;
        case OSD_START+0xE:
            OSD_width_regs[1]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 buffer width",val,size);            
            break;
        case OSD_START+0x10:
            OSD_width_regs[2]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 buffer width",val,size);            
            break;
        case OSD_START+0x12:
            OSD_width_regs[3]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 buffer width",val,size);            
            break;
            
        case OSD_START+0x14:
            //OSD_offset_regs[0] = SDRAM_START | ((val&0x07)<<21) | (OSD_offset_regs[0] & 0x1FFFFF);
            //OSD_offset_regs[1] = SDRAM_START | (((val>>8)&0x07)<<21) | (OSD_offset_regs[1] & 0x1FFFFF);
            OSD_offset_regs[0] = (((val&0x3F)<<21) | ((OSD_offset_regs[0]-SDRAM_START) & 0x1FFFFF))+SDRAM_START;
            OSD_offset_regs[1] = ((((val>>8)&0x3F)<<21) | ((OSD_offset_regs[1]-SDRAM_START) & 0x1FFFFF))+SDRAM_START;
            
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid0=%x Vid1=%x)\n",name,"Vid0 & Vid1 buffer HI offset",
                val,size,OSD_offset_regs[0],OSD_offset_regs[1]);            
            break;        
        case OSD_START+0x16:
            //OSD_offset_regs[0] = SDRAM_START | (OSD_offset_regs[0]&0x0E00000) | ((val<<5)&0x1FFFFF);
            OSD_offset_regs[0] = (((OSD_offset_regs[0]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid0=%x)\n",name,"Vid0 buffer LO offset",
                val,size,OSD_offset_regs[0]);            
            break;        
        case OSD_START+0x18:
            //OSD_offset_regs[1] = SDRAM_START | (OSD_offset_regs[1]&0x0E00000) | ((val<<5)&0x1FFFFF);
            OSD_offset_regs[1] = (((OSD_offset_regs[1]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid1=%x)\n",name,"Vid1 buffer LO offset",
                val,size,OSD_offset_regs[1]);            
            break;
        case OSD_START+0x1A:
            //OSD_offset_regs[2] = SDRAM_START | ((val&0x07)<<21) | (OSD_offset_regs[2] & 0x1FFFFF);
            //OSD_offset_regs[3] = SDRAM_START | (((val>>8)&0x07)<<21) | (OSD_offset_regs[3] & 0x1FFFFF);
            OSD_offset_regs[2] = (((val&0x3F)<<21) | ((OSD_offset_regs[2]-SDRAM_START) & 0x1FFFFF)) + SDRAM_START;
            OSD_offset_regs[3] = ((((val>>8)&0x3F)<<21) | ((OSD_offset_regs[3]-SDRAM_START) & 0x1FFFFF)) + SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Bmap0=%x Bmap1=%x)\n",name,"Bmap0 & Bmap1 buffer HI offset",
                val,size,OSD_offset_regs[2],OSD_offset_regs[3]);            
            break;        
        case OSD_START+0x1C:
            //OSD_offset_regs[2] = SDRAM_START | (OSD_offset_regs[2]&0x0E00000) | ((val<<5)&0x1FFFFF);
            OSD_offset_regs[2] = (((OSD_offset_regs[2]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) Bmap0=%x)\n",name,"Bmap0 buffer LO offset",
                val,size,OSD_offset_regs[2]);            
            break;        
        case OSD_START+0x1E:
            //OSD_offset_regs[3] = SDRAM_START | (OSD_offset_regs[3]&0x0E00000) | ((val<<5)&0x1FFFFF);
            OSD_offset_regs[3] = (((OSD_offset_regs[3]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) Bmap1=%x)\n",name,"Bmap1 buffer LO offset",
                val,size,OSD_offset_regs[3]);            
            break;
            
        case OSD_START+0x20:
            OSD_main_shift_horiz=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"main shift HORIZ",val,size);            
            break;
        case OSD_START+0x22:
            OSD_main_shift_vert=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"main shift VERT",val,size);            
            break;
            
        case OSD_START+0x24:
            OSD_info_regs[0].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 X",val,size);            
            break;
        case OSD_START+0x26:
            OSD_info_regs[0].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 Y",val,size);            
            break;
        case OSD_START+0x28:
            OSD_info_regs[0].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 width",val,size);            
            break;
        case OSD_START+0x2A:
            OSD_info_regs[0].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 height",val,size);            
            break;
        case OSD_START+0x2C:
            OSD_info_regs[1].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 X",val,size);            
            break;
        case OSD_START+0x2E:
            OSD_info_regs[1].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 Y",val,size);            
            break;
        case OSD_START+0x30:
            OSD_info_regs[1].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 width",val,size);            
            break;
        case OSD_START+0x32:
            OSD_info_regs[1].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 height",val,size);            
            break;
        case OSD_START+0x34:
            OSD_info_regs[2].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 X",val,size);            
            break;
        case OSD_START+0x36:
            OSD_info_regs[2].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 Y",val,size);            
            break;
        case OSD_START+0x38:
            OSD_info_regs[2].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 width",val,size);            
            break;
        case OSD_START+0x3A:
            OSD_info_regs[2].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 height",val,size);            
            break;
        case OSD_START+0x3C:
            OSD_info_regs[3].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 X",val,size);            
            break;
        case OSD_START+0x3E:
            OSD_info_regs[3].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 Y",val,size);            
            break;
        case OSD_START+0x40:
            OSD_info_regs[3].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 width",val,size);            
            break;
        case OSD_START+0x42:
            OSD_info_regs[3].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 height",val,size);
            break;
        case OSD_START+0x44:
            OSD_info_regs[4].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 X",val,size);            
            break;
        case OSD_START+0x46:
            OSD_info_regs[4].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 Y",val,size);            
            break;
        case OSD_START+0x48:
            OSD_info_regs[4].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 width",val,size);            
            break;
        case OSD_START+0x4A:
            OSD_info_regs[4].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 height",val,size);            
            break;
        case OSD_START+0x4C:
            OSD_info_regs[5].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor1 width",val,size);            
            break;
        case OSD_START+0x4E:
            OSD_info_regs[5].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor1 height",val,size);            
            break;
        
        case OSD_START+0x50:
        case OSD_START+0x52:
        case OSD_START+0x54:
        case OSD_START+0x56:
        case OSD_START+0x58:
        case OSD_START+0x5A:
        case OSD_START+0x5C:
        case OSD_START+0x5E:
        {
            int index=(addr-OSD_START+0x50)/2;
            OSD_pallette_bank1[index]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d write %x (size %x)\n",name,"Palette Bank1",index,val,size);            
            break;
        }
        case OSD_START+0x60:
        case OSD_START+0x62:
        case OSD_START+0x64:
        case OSD_START+0x66:
        case OSD_START+0x68:
        case OSD_START+0x6A:
        case OSD_START+0x6C:
        case OSD_START+0x6E:
        {
            int index=(addr-OSD_START+0x60)/2;
            OSD_pallette_bank2[index]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d write %x (size %x)\n",name,"Palette Bank2",index,val,size);            
            break;
        }
            
        case OSD_START+0x72:
            OSD_cursor_data=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor data",val,size);            
            break;
        case OSD_START+0x74:
            OSD_pallette_status=val&0xFFFE; // keeping bit 0 at 0
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,
                "Pallette status & cursor data",val,size);            
            break;
        case OSD_START+0x76:
            OSD_pallette_data_wr=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Pallette data write",val,size);            
            break;
        case OSD_START+0x78:
            {
                int Y = (OSD_pallette_data_wr >> 8) & 0xFF;
                int Cb = OSD_pallette_data_wr & 0xFF;
                int Cr = (val >> 8) & 0xFF;
                int index = val & 0xFF;
                int ret = lcd->setPaletteYCbCr(Y,Cb,Cr,index);
                /*lcd->updte_lcd(OSD_offset_regs[2],LCD_BMAP);*/
                /*lcd->updte_lcd(OSD_offset_regs[0],LCD_VID);*/
                OSD_pallette_index=0;
                OSD_pallette_data_wr=0;
                DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Pallette data & index",val,size);
                DEBUG_HW(OSD_HW_DEBUG,"Palette update: Y=%x,Cr=%x,Cb=%x index=%x => Xindex=%x\n",
                    Y,Cb,Cr,index,ret);
            }
            break;
        case OSD_START+0x7C:
            //OSD_alt_vid_offset = (OSD_alt_vid_offset & 0xFFFF) | ((val & 0x3F)<<21);
            OSD_alt_vid_offset = (((val&0x3F)<<21) | ((OSD_alt_vid_offset-SDRAM_START) & 0x1FFFFF));
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (OSD ALT=%x)\n",name,"OSD ALT buffer HI",
                val,size,OSD_alt_vid_offset);
            break;
        case OSD_START+0x7E:
            //OSD_alt_vid_offset = (OSD_alt_vid_offset & 0x3F0000) | (val & 0xFFFF);
            OSD_alt_vid_offset = (((OSD_alt_vid_offset-SDRAM_START)&0x03E0000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (OSD ALT=%x)\n",name,"Pallette data & index",
                val,size,OSD_alt_vid_offset);
            break;
    }

}

int HW_OSD::nxtEvent(void)
{
#ifdef HAS_LCD
    return lcd->nxtEvent(OSD_config_regs,OSD_offset_regs);
#endif
}

void HW_OSD::chk_access(uint32_t addr,uint32_t val,int size)
{
#ifdef HAS_LCD
    if((OSD_config_regs[2]&0x1) && addr>=OSD_offset_regs[2]
            && addr <= (OSD_offset_regs[2]+OSD_width_regs[2]*32*OSD_info_regs[2].height)){
        uint32_t v=val;
        uint32_t a=addr-OSD_offset_regs[2];
        switch(size){ /* no break intended */
            case 4:
                lcd->drawPix(a,v&0xff);++a;v>>=8;
                lcd->drawPix(a,v&0xff);++a;v>>=8;
            case 2:
                lcd->drawPix(a,v&0xff);++a;v>>=8;
            case 1:
                lcd->drawPix(a,v&0xff);++a;v>>=8;
        }
    }

#ifdef HAS_VID0
    else if ((OSD_config_regs[1]&0x1) && addr>=OSD_offset_regs[0]
            && addr <= (OSD_offset_regs[0]+OSD_width_regs[0]*8*OSD_info_regs[0].height))
            lcd->drawVidPix(addr-OSD_offset_regs[0],val);
#endif
#endif
}

