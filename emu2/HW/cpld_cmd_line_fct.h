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

int do_cmd_btn_down_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_DOWN);
}

int do_cmd_btn_left_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_LEFT);
}

int do_cmd_btn_right_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_RIGHT);
}

int do_cmd_btn_f1_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_F1);
}

int do_cmd_btn_f2_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_F2);
}

int do_cmd_btn_f3_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_F3);
}

int do_cmd_btn_joy_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_JOY);
}

int do_cmd_btn_on_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_ON);
}

int do_cmd_ch_module_s(int argc,char ** argv) 
{
    if(argc>0)
    {
        int num = my_atoi(argv[0]);
        cpld_obj->setModule(num&0xF);
    }
    return 0;
}


int do_cmd_btn_off_s(int argc,char ** argv) 
{ 
    return cpld_obj->do_cmd_btn(BTN_OFF);
}

int HW_cpld::do_cmd_btn(int btn)
{
    switch(btn)
    {
        case BTN_ON:
            ON_btn->state=ON_btn->state==1?0:1;
            break;
        case BTN_OFF:
            OFF_btn->state=OFF_btn->state==1?0:1;
            break;
        default:
            btn_var[btn]=BTN_INIT_VAL;
            break;
    }
    
    return 0;
}


void init_cpld_static_fct(HW_cpld * cpld)
{
    cpld_obj = cpld;
    add_cmd_fct("up",do_cmd_btn_up_s,"Emulate btn UP press");
    add_cmd_fct("down",do_cmd_btn_down_s,"Emulate btn DOWN press");
    add_cmd_fct("left",do_cmd_btn_left_s,"Emulate btn LEFT press");
    add_cmd_fct("right",do_cmd_btn_right_s,"Emulate btn RIGHT press");
    add_cmd_fct("f1",do_cmd_btn_f1_s,"Emulate btn F1 press");
    add_cmd_fct("f2",do_cmd_btn_f2_s,"Emulate btn F2 press");
    add_cmd_fct("f3",do_cmd_btn_f3_s,"Emulate btn F3 press");
    add_cmd_fct("joy",do_cmd_btn_joy_s,"Emulate btn JOY press");
    add_cmd_fct("on",do_cmd_btn_on_s,"Emulate btn ON press");
    add_cmd_fct("off",do_cmd_btn_off_s,"Emulate btn OFF press");  
    add_cmd_fct("ch_module",do_cmd_ch_module_s,"Change the module type (0xF) => no module");  
}
