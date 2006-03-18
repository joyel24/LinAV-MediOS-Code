/* 
*   osd_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

int do_cmd_cfg_out_s(int argc,char ** argv) 
{ 
    return hw_OSD->do_cmd_cfg_out(argc,argv);
}

char * str_zoom[] = {"normal", "x2", "x4"};
int color_map_tab[] = {2,4,16,256};

extern int lcd_update_cnt[2];

int HW_OSD::do_cmd_cfg_out(int argc,char ** argv)
{
    int val;
    printf("Current OSD config\n");
    printf("Main config (%04x)\n",OSD_config_regs[0]);
    val = OSD_config_regs[1]&0xFF;
    printf("VID0 config (%02x): %s, %s, Zoom y %s, Zoom x %s\n",val,(val&0x1)?"enable":"disable",
        (val&0x2)?"half height":"normal height",str_zoom[(val>>2)&0x3],str_zoom[(val>>4)&0x3]);
    val = (OSD_config_regs[1]>>8)&0xFF;
    printf("VID1 config (%02x): %s, %s, Zoom y %s, Zoom x %s\n",val,(val&0x1)?"enable":"disable",
        (val&0x2)?"half height":"normal height",str_zoom[(val>>2)&0x3],str_zoom[(val>>4)&0x3]);

     
    val = OSD_config_regs[2]&0xFFFF;  
    printf("BMAP0 config (%04x): %s, %s, %s, tsp color = %x, %d color map, Zoom y %s, Zoom x %s, %s palette\n",
        val,(val&0x1)?"enable":"disable",
        (val&0x2)?"half height":"normal height",(val&0x4)?"Merge with vplane":"Color 0 = tsp",
        (val>>3)&0x7,color_map_tab[(val>>6)&0x3],str_zoom[(val>>8)&0x3],str_zoom[(val>>0xa)&0x3],
        (val>>0xc)&0x1?"RAM":"ROM");
    val = OSD_config_regs[3]&0xFFFF;  
    printf("BMAP1 config (%04x): %s, %s, %s, tsp color = %x, %d color map, Zoom y %s, Zoom x %s, %s palette\n",
        val,(val&0x1)?"enable":"disable",
        (val&0x2)?"half height":"normal height",(val&0x4)?"Merge with vplane":"Color 0 = tsp",
        (val>>3)&0x7,color_map_tab[(val>>6)&0x3],str_zoom[(val>>8)&0x3],str_zoom[(val>>0xa)&0x3],
        (val>>0xc)&0x1?"RAM":"ROM");    
    printf("VID0 @:0x%x\nVID1 @:0x%x\nBMAP0 @:0x%x\nBMAP1 @:0x%x\n",
        OSD_offset_regs[0],OSD_offset_regs[1],OSD_offset_regs[2],OSD_offset_regs[3]);
    printf("LCD update stats: BMAP0 : %d, VID0 : %d\n",lcd_update_cnt[0],lcd_update_cnt[1]);
    return 0;
}

int do_cmd_updt_lcd_s(int argc,char ** argv) 
{ 
    return hw_OSD->do_cmd_updt_lcd(argc,argv);
}


int HW_OSD::do_cmd_updt_lcd(int argc,char ** argv)
{
    uint32_t start_addr=0;
    /*if(argc>0) !!!!!!!!!!!! need to be changed to take a second arg: display type = VID or BMAP
    {
        start_addr = my_atoi(argv[0]);
        lcd->updte_lcd(start_addr);
    }
    else
    {
        if(OSD_offset_regs[2]>=SDRAM_START && OSD_offset_regs[2] < SDRAM_END)
            lcd->updte_lcd(OSD_offset_regs[2]);
    }*/
    return 0;
}

void init_mem_static_fct(HW_OSD * osd)
{
    hw_OSD = osd;
    add_cmd_fct("updt_lcd",do_cmd_updt_lcd_s,"Update the LCD");
    add_cmd_fct("osd_info",do_cmd_cfg_out_s,"Current OSD config");
}
