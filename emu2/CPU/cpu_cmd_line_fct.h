
int do_cmd_help_s(int argc,char ** argv)
{ 
    return cur_cpu->do_cmd_help(argc,argv);
}

int Cpu::do_cmd_help(int argc,char ** argv)
{
    printf("Av3XX emulator by OxyGen\n");
    /* print all cmd */
    print_cmd_list();
    return 0;
}

int do_cmd_quit_s(int argc,char ** argv)
{ 
    return cur_cpu->do_cmd_quit(argc,argv);
}


int Cpu::do_cmd_quit(int argc,char ** argv)
{
#warning we can do a better stop
    printf("Bye\n\n");
    exit(0);
    return 0;
}

int do_cmd_step_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_step(argc,argv);
}


int Cpu::do_cmd_step(int argc,char ** argv)
{
    return 1;
}

int do_cmd_run_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_run(argc,argv);
}


int Cpu::do_cmd_run(int argc,char ** argv)
{
    run_mode=RUN;
    return 1;
}

int do_cmd_add_bkpt_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_add_bkpt(argc,argv);
}


int Cpu::do_cmd_add_bkpt(int argc,char ** argv)
{
    /*get first arg*/
    uint32_t i=0;
    if(argc>0)
    {
        i = my_atoi(argv[0]);          
        bkpt->add(i);
    }
    else
        bkpt->print_bkpt_list();
    return 0;
}

int do_cmd_print_stack_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_print_stack(argc,argv);
}


int Cpu::do_cmd_print_stack(int argc,char ** argv)
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

int do_cmd_chg_disp_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_chg_disp(argc,argv);
}

int Cpu::do_cmd_chg_disp(int argc,char ** argv)
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

int do_cmd_chg_hw_disp_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_chg_hw_disp(argc,argv);
}

int Cpu::do_cmd_chg_hw_disp(int argc,char ** argv)
{
    int i;
    if(argc>0)
    {
        i = my_atoi(argv[0]);
        if(i)
            HW_mode=1;
        else
            HW_mode=0;
    }
    else
    {
        if(HW_mode)            
            printf("displaying HW output\n");
        else
            printf("Not displaying HW output\n");
    }
    return 0;
}

int do_cmd_print_state_s(int argc,char ** argv) 
{ 
    return cur_cpu->do_cmd_print_state(argc,argv);
}


int Cpu::do_cmd_print_state(int argc,char ** argv)
{
    printState();    
    return 0;
}


void init_static_fct(Cpu * cpu)
{
    cur_cpu = cpu;
    add_cmd_fct("help",do_cmd_help_s,"Print this help");
    add_cmd_fct("?",do_cmd_help_s,"Print this help");
    add_cmd_fct("q",do_cmd_quit_s,"Exit");
    add_cmd_fct("quit",do_cmd_quit_s,"Exit");
    add_cmd_fct("exit",do_cmd_quit_s,"Exit");
    add_cmd_fct("step",do_cmd_step_s,"Execute next instruction");
    add_cmd_fct("run",do_cmd_run_s,"Execute code");
    add_cmd_fct("bkpt",do_cmd_add_bkpt_s,"Add/display a breakpoint");
    add_cmd_fct("stack",do_cmd_print_stack_s,"Show stack");
    add_cmd_fct("sp",do_cmd_print_stack_s,"Show stack");
    add_cmd_fct("SP",do_cmd_print_stack_s,"Show stack");
    add_cmd_fct("state",do_cmd_print_state_s,"Show reg value");
    add_cmd_fct("reg",do_cmd_print_state_s,"Show reg value");
    add_cmd_fct("disp",do_cmd_chg_disp_s,"Change/display display mode");
    add_cmd_fct("hw",do_cmd_chg_hw_disp_s,"Change/display HW display mode");
}
