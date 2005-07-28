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



int do_cmd_updt_lcd_s(int argc,char ** argv) 
{ 
    return hw_OSD->do_cmd_updt_lcd(argc,argv);
}


int HW_OSD::do_cmd_updt_lcd(int argc,char ** argv)
{
    uint32_t start_addr=0;
    if(argc>0)
    {
        start_addr = my_atoi(argv[0]);
        lcd->updte_lcd(start_addr);
    }
    else
    {
        if(OSD_offset_regs[2]>=SDRAM_START && OSD_offset_regs[2] < SDRAM_END)
            lcd->updte_lcd(OSD_offset_regs[2]);
    }
    return 0;
}

void init_mem_static_fct(HW_OSD * osd)
{
    hw_OSD = osd;
    add_cmd_fct("updt_lcd",do_cmd_updt_lcd_s,"Update the LCD");
}
