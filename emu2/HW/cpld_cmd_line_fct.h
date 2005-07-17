/* 
*   cpld_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


int do_cmd_btn_up_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_UP);
}

int HW_cpld::do_cmd_btn(int btn)
{
    btn_var[btn]=BTN_INIT_VAL;
    return 0;
}

void init_cpld_static_fct(HW_cpld * cpld)
{
    cpld_obj = cpld;
    add_cmd_fct("btn_up",do_cmd_btn_up_s,"Emulate btn UP press");    
}
