/* 
*   cpu_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

int do_cmd_help(int argc,char ** argv)
{
    printf("Av3XX emulator by OxyGen\n");
    /* print all cmd */
    print_cmd_list();
    return 0;
}

int do_cmd_quit(int argc,char ** argv)
{
#warning we can do a better stop
    printf("Bye\n\n");
    exit(0);
    return 0;
}

int do_cmd_step(int argc,char ** argv)
{
    return 1;
}

int do_cmd_run(int argc,char ** argv)
{
    CHG_RUN_MODE(RUN);
    return 1;
}

int do_cmd_add_bkpt(int argc,char ** argv)
{
    /*get first arg*/
    uint32_t i=0;
    bool do_del = false;
    if(argc>0)
    {
        if(argv[0][0] == '-')
        {
            do_del=true;
            argv[0]++;
        }
        i = my_atoi(argv[0]);
        if(do_del)
            del(bkpt,i);
        else
            add(bkpt,i);
    }
    else
        print_bkpt_list(bkpt);
    return 0;
}

int do_cmd_print_stack(int argc,char ** argv)
{
    uint32_t SP_val = GET_REG(R_SP);
    printf("Stack content:\n");
    for(int i=0;i<0x10;i++)
    {        
        printf("%x:@%08x = %x\n",i,SP_val,mem->read(SP_val,4));
        SP_val+=0x4;
    }
    
    return 0;
}

int do_cmd_chg_disp(int argc,char ** argv)
{
    int i;
    if(argc>0)
    {
        i = my_atoi(argv[0]);
        if(i)
            disp_mode=1;
        else
            disp_mode=0;
    }
    else
    {
        if(disp_mode)            
            printf("displaying instruction\n");
        else
            printf("Not displaying instruction\n");
    }
    return 0;
}

int do_cmd_chg_hw_disp(int argc,char ** argv)
{
    int i;
    if(argc>0)
    {
        i = my_atoi(argv[0]);
        HW_mode=i;
    }
    else
    {
        if(HW_mode)            
            printf("displaying HW output level: %x\n",HW_mode);
        else
            printf("Not displaying HW output\n");
    }
    return 0;
}

int do_cmd_print_state(int argc,char ** argv)
{
    int reg_num;
    uint32_t val;
    if(argc>0)
    {
        reg_num = my_atoi(argv[0]);
        if(argc>1)
        {
            val = my_atoi(argv[1]);
            REG(reg_num) = val;
            printf("%s set to %x\n",RR(reg_num),GET_REG(reg_num));
        }
        else
            printf("%s=%x\n",RR(reg_num),GET_REG(reg_num));
    }
    else    
        printState();    
    return 0;
}

void init_cpu_static_fct(void)
{
    add_cmd_fct("help",do_cmd_help,"Print this help");
    add_cmd_fct("?",do_cmd_help,"Print this help");
    add_cmd_fct("q",do_cmd_quit,"Exit");
    add_cmd_fct("quit",do_cmd_quit,"Exit");
    add_cmd_fct("exit",do_cmd_quit,"Exit");
    add_cmd_fct("step",do_cmd_step,"Execute next instruction");
    add_cmd_fct("run",do_cmd_run,"Execute code");
    add_cmd_fct("bkpt",do_cmd_add_bkpt,"Add/display a breakpoint");
    add_cmd_fct("stack",do_cmd_print_stack,"Show stack");
    add_cmd_fct("sp",do_cmd_print_stack,"Show stack");
    add_cmd_fct("SP",do_cmd_print_stack,"Show stack");
    add_cmd_fct("state",do_cmd_print_state,"Show reg value");
    add_cmd_fct("reg",do_cmd_print_state,"Show reg value");
    add_cmd_fct("disp",do_cmd_chg_disp,"Change/display display mode");
    add_cmd_fct("hw",do_cmd_chg_hw_disp,"Change/display HW display mode");
}
