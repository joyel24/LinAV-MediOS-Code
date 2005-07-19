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

HW_OSD::HW_OSD(HW_mem * mem):HW_access(0x30680,0x30700,"OSD")
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
        
        OSD_offset_regs[i]=0;
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
    
    lcd = new HW_lcd(mem);
}

#include <osd_cmd_line_fct.h>

uint32_t HW_OSD::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
    switch(addr)
    {
        case 0x30680:
            ret_val=OSD_config_regs[0];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Main config",ret_val,size);            
            break;
        case 0x30682:
            ret_val=OSD_config_regs[1];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 & Vid1 config",ret_val,size);            
            break;
        case 0x30684:
            ret_val=OSD_config_regs[2];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 config",ret_val,size);            
            break;
        case 0x30686:
            ret_val=OSD_config_regs[3];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 config",ret_val,size);            
            break;
        case 0x30688:
            ret_val=OSD_config_regs[4];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 config",ret_val,size);            
            break;
        case 0x3068A:
            ret_val=OSD_config_regs[5];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor1 config",ret_val,size);            
            break;
            
        case 0x3068C:
            ret_val=OSD_width_regs[0];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 buffer width",ret_val,size);            
            break;
        case 0x3068E:
            ret_val=OSD_width_regs[1];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 buffer width",ret_val,size);            
            break;
        case 0x30690:
            ret_val=OSD_width_regs[2];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 buffer width",ret_val,size);            
            break;
        case 0x30692:
            ret_val=OSD_width_regs[3];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 buffer width",ret_val,size);            
            break;
            
        case 0x30694:
            ret_val=(((OSD_offset_regs[1]>>21)&0x07)<<8) | ((OSD_offset_regs[0]>>21)&0x07);  // removing the SDRAM base addr. 0x03000000
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid0=%x Vid1=%x)\n",name,"Vid0 & Vid1 buffer HI offset",
                ret_val,size,OSD_offset_regs[0],OSD_offset_regs[1]);            
            break;        
        case 0x30696:
            ret_val=(OSD_offset_regs[0]>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid0=%x)\n",name,"Vid0 buffer LO offset",
                ret_val,size,OSD_offset_regs[0]);            
            break;        
        case 0x30698:
            ret_val=(OSD_offset_regs[1]>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid1=%x)\n",name,"Vid1 buffer LO offset",
                ret_val,size,OSD_offset_regs[1]);   
            break;
        case 0x3069A:
            ret_val=(((OSD_offset_regs[3]>>21)&0x07)<<8) | ((OSD_offset_regs[2]>>21)&0x07);  // removing the SDRAM base addr. 0x03000000
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Bmap0=%x Bmap1=%x)\n",name,"Bmap0 & Bmap1 buffer HI offset",
                   ret_val,size,OSD_offset_regs[2],OSD_offset_regs[3]);            
            break;        
        case 0x3069C:
            ret_val=(OSD_offset_regs[2]>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) Bmap0=%x)\n",name,"Bmap0 buffer LO offset",
                ret_val,size,OSD_offset_regs[2]);            
            break;        
        case 0x3069E:
            ret_val=(OSD_offset_regs[3]>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) Bmap1=%x)\n",name,"Bmap1 buffer LO offset",
                ret_val,size,OSD_offset_regs[3]);            
            break;
            
        case 0x306A0:
            ret_val=OSD_main_shift_horiz;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"main shift HORIZ",ret_val,size);            
            break;
        case 0x306A2:
            ret_val=OSD_main_shift_vert;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"main shift VERT",ret_val,size);            
            break;
            
        case 0x306A4:
            ret_val=OSD_info_regs[0].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 X",ret_val,size);            
            break;
        case 0x306A6:
            ret_val=OSD_info_regs[0].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 Y",ret_val,size);            
            break;
        case 0x306A8:
            ret_val=OSD_info_regs[0].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 width",ret_val,size);            
            break;
        case 0x306AA:
            ret_val=OSD_info_regs[0].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 height",ret_val,size);            
            break;
        case 0x306AC:
            ret_val=OSD_info_regs[1].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 X",ret_val,size);            
            break;
        case 0x306AE:
            ret_val=OSD_info_regs[1].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 Y",ret_val,size);            
            break;
        case 0x306B0:
            ret_val=OSD_info_regs[1].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 width",ret_val,size);            
            break;
        case 0x306B2:
            ret_val=OSD_info_regs[1].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid1 height",ret_val,size);            
            break;
        case 0x306B4:
            ret_val=OSD_info_regs[2].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 X",ret_val,size);            
            break;
        case 0x306B6:
            ret_val=OSD_info_regs[2].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 Y",ret_val,size);            
            break;
        case 0x306B8:
            ret_val=OSD_info_regs[2].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 width",ret_val,size);            
            break;
        case 0x306BA:
            ret_val=OSD_info_regs[2].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 height",ret_val,size);            
            break;
        case 0x306BC:
            ret_val=OSD_info_regs[3].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 X",ret_val,size);            
            break;
        case 0x306BE:
            ret_val=OSD_info_regs[3].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 Y",ret_val,size);            
            break;
        case 0x306C0:
            ret_val=OSD_info_regs[3].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 width",ret_val,size);            
            break;
        case 0x306C2:
            ret_val=OSD_info_regs[3].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 height",ret_val,size);            
            break;
        case 0x306C4:
            ret_val=OSD_info_regs[4].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 X",ret_val,size);            
            break;
        case 0x306C6:
            ret_val=OSD_info_regs[4].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 Y",ret_val,size);            
            break;
        case 0x306C8:
            ret_val=OSD_info_regs[4].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 width",ret_val,size);            
            break;
        case 0x306CA:
            ret_val=OSD_info_regs[4].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 height",ret_val,size);            
            break;
        case 0x306CC:
            ret_val=OSD_info_regs[5].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor1 X",ret_val,size);            
            break;
        case 0x306CE:
            ret_val=OSD_info_regs[5].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor1 Y",ret_val,size);            
            break;
            
        case 0x306D0:
        case 0x306D2:
        case 0x306D4:
        case 0x306D6:
        case 0x306D8:
        case 0x306DA:
        case 0x306DC:
        case 0x306DE:
        {
            int index=(addr-0x306D0)/2;
            ret_val=OSD_pallette_bank1[index];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d read => %x (size %x)\n",name,"Palette Bank1",index,ret_val,size);            
            break;
        }
        case 0x306E0:
        case 0x306E2:
        case 0x306E4:
        case 0x306E6:
        case 0x306E8:
        case 0x306EA:
        case 0x306EC:
        case 0x306EE:
        {
            int index=(addr-0x306D0)/2;
            ret_val=OSD_pallette_bank2[index];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d read => %x (size %x)\n",name,"Palette Bank2",index,ret_val,size);            
            break;
        }
        
        case 0x306F2:
            ret_val=OSD_cursor_data;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor data",ret_val,size);            
            break;
        case 0x306F4:
            ret_val=OSD_pallette_status;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Pallette status & cursor data",ret_val,size);            
            break;
        case 0x306F6:
            ret_val=OSD_pallette_data_wr;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Pallette data write",ret_val,size);            
            break;
        case 0x306F8:
            ret_val=OSD_pallette_index;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Pallette data & index",ret_val,size);            
            break;
        case 0x306FC:
            ret_val=(OSD_alt_vid_offset>>16)&0x3F;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (OSD ALT=%x)\n",name,"OSD ALT buffer HI",
                ret_val,size,OSD_alt_vid_offset);            
            break;
        case 0x306FE:
            ret_val=OSD_alt_vid_offset&0xFFFF;
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
        case 0x30680:
            OSD_config_regs[0]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Main config",val,size);            
            break;
        case 0x30682:
            OSD_config_regs[1]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 & Vid1 config",val,size);            
            break;
        case 0x30684:
            OSD_config_regs[2]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 config",val,size);            
            break;
        case 0x30686:
            OSD_config_regs[3]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 config",val,size);            
            break;
        case 0x30688:
            OSD_config_regs[4]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 config",val,size);            
            break;
        case 0x3068A:
            OSD_config_regs[5]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor1 config",val,size);            
            break;
            
        case 0x3068C:
            OSD_width_regs[0]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 buffer width",val,size);            
            break;
        case 0x3068E:
            OSD_width_regs[1]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 buffer width",val,size);            
            break;
        case 0x30690:
            OSD_width_regs[2]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 buffer width",val,size);            
            break;
        case 0x30692:
            OSD_width_regs[3]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 buffer width",val,size);            
            break;
            
        case 0x30694:
            OSD_offset_regs[0] = 0x03000000 | ((val&0x07)<<21) | (OSD_offset_regs[0] & 0x1FFFFF);
            OSD_offset_regs[1] = 0x03000000 | (((val>>8)&0x07)<<21) | (OSD_offset_regs[1] & 0x1FFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid0=%x Vid1=%x)\n",name,"Vid0 & Vid1 buffer HI offset",
                val,size,OSD_offset_regs[0],OSD_offset_regs[1]);            
            break;        
        case 0x30696:
            OSD_offset_regs[0] = 0x03000000 | (OSD_offset_regs[0]&0x0E00000) | ((val<<5)&0x1FFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid0=%x)\n",name,"Vid0 buffer LO offset",
                val,size,OSD_offset_regs[0]);            
            break;        
        case 0x30698:
            OSD_offset_regs[1] = 0x03000000 | (OSD_offset_regs[1]&0x0E00000) | ((val<<5)&0x1FFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid1=%x)\n",name,"Vid1 buffer LO offset",
                val,size,OSD_offset_regs[1]);            
            break;
        case 0x3069A:
            OSD_offset_regs[2] = 0x03000000 | ((val&0x07)<<21) | (OSD_offset_regs[2] & 0x1FFFFF);
            OSD_offset_regs[3] = 0x03000000 | (((val>>8)&0x07)<<21) | (OSD_offset_regs[3] & 0x1FFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Bmap0=%x Bmap1=%x)\n",name,"Bmap0 & Bmap1 buffer HI offset",
                val,size,OSD_offset_regs[2],OSD_offset_regs[3]);            
            break;        
        case 0x3069C:
            OSD_offset_regs[2] = 0x03000000 | (OSD_offset_regs[2]&0x0E00000) | ((val<<5)&0x1FFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) Bmap0=%x)\n",name,"Bmap0 buffer LO offset",
                val,size,OSD_offset_regs[2]);            
            break;        
        case 0x3069E:
            OSD_offset_regs[3] = 0x03000000 | (OSD_offset_regs[3]&0x0E00000) | ((val<<5)&0x1FFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) Bmap1=%x)\n",name,"Bmap1 buffer LO offset",
                val,size,OSD_offset_regs[3]);            
            break;
            
        case 0x306A0:
            OSD_main_shift_horiz=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"main shift HORIZ",val,size);            
            break;
        case 0x306A2:
            OSD_main_shift_vert=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"main shift VERT",val,size);            
            break;
            
        case 0x306A4:
            OSD_info_regs[0].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 X",val,size);            
            break;
        case 0x306A6:
            OSD_info_regs[0].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 Y",val,size);            
            break;
        case 0x306A8:
            OSD_info_regs[0].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 width",val,size);            
            break;
        case 0x306AA:
            OSD_info_regs[0].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 height",val,size);            
            break;
        case 0x306AC:
            OSD_info_regs[1].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 X",val,size);            
            break;
        case 0x306AE:
            OSD_info_regs[1].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 Y",val,size);            
            break;
        case 0x306B0:
            OSD_info_regs[1].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 width",val,size);            
            break;
        case 0x306B2:
            OSD_info_regs[1].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid1 height",val,size);            
            break;
        case 0x306B4:
            OSD_info_regs[2].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 X",val,size);            
            break;
        case 0x306B6:
            OSD_info_regs[2].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 Y",val,size);            
            break;
        case 0x306B8:
            OSD_info_regs[2].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 width",val,size);            
            break;
        case 0x306BA:
            OSD_info_regs[2].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 height",val,size);            
            break;
        case 0x306BC:
            OSD_info_regs[3].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 X",val,size);            
            break;
        case 0x306BE:
            OSD_info_regs[3].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 Y",val,size);            
            break;
        case 0x306C0:
            OSD_info_regs[3].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 width",val,size);            
            break;
        case 0x306C2:
            OSD_info_regs[3].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 height",val,size);            
            break;
        case 0x306C4:
            OSD_info_regs[4].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 X",val,size);            
            break;
        case 0x306C6:
            OSD_info_regs[4].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 Y",val,size);            
            break;
        case 0x306C8:
            OSD_info_regs[4].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 width",val,size);            
            break;
        case 0x306CA:
            OSD_info_regs[4].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 height",val,size);            
            break;
        case 0x306CC:
            OSD_info_regs[5].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor1 width",val,size);            
            break;
        case 0x306CE:
            OSD_info_regs[5].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor1 height",val,size);            
            break;
        
        case 0x306D0:
        case 0x306D2:
        case 0x306D4:
        case 0x306D6:
        case 0x306D8:
        case 0x306DA:
        case 0x306DC:
        case 0x306DE:
        {
            int index=(addr-0x306D0)/2;
            OSD_pallette_bank1[index]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d write %x (size %x)\n",name,"Palette Bank1",index,val,size);            
            break;
        }
        case 0x306E0:
        case 0x306E2:
        case 0x306E4:
        case 0x306E6:
        case 0x306E8:
        case 0x306EA:
        case 0x306EC:
        case 0x306EE:
        {
            int index=(addr-0x306D0)/2;
            OSD_pallette_bank2[index]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d write %x (size %x)\n",name,"Palette Bank2",index,val,size);            
            break;
        }
            
        case 0x306F2:
            OSD_cursor_data=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor data",val,size);            
            break;
        case 0x306F4:
            OSD_pallette_status=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Pallette status & cursor data",val,size);            
            break;
        case 0x306F6:
            OSD_pallette_data_wr=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Pallette data write",val,size);            
            break;
        case 0x306F8:
            OSD_pallette_index=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Pallette data & index",val,size);            
            break;
        case 0x306FC:
            OSD_alt_vid_offset = (OSD_alt_vid_offset & 0xFFFF) | ((val & 0x3F)<<16);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (OSD ALT=%x)\n",name,"OSD ALT buffer HI",
                val,size,OSD_alt_vid_offset);            
            break;
        case 0x306FE:
            OSD_alt_vid_offset = (OSD_alt_vid_offset & 0x3F0000) | (val & 0xFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (OSD ALT=%x)\n",name,"Pallette data & index",
                val,size,OSD_alt_vid_offset);            
            break;
    }
    
}

void HW_OSD::chk_access(uint32_t addr,uint32_t val)
{
    if(OSD_offset_regs[2] >= 0x03000000 && addr>=OSD_offset_regs[2] 
            && addr <= (OSD_offset_regs[2]+lcd->SCREEN_WIDTH*lcd->SCREEN_HEIGHT*2))
            lcd->drawPix(addr-OSD_offset_regs[2],val);
        lcd->nxtEvent(OSD_offset_regs[2]);
}

