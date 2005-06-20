
int do_cmd_help_s(char * arg) 
{ 
    return cur_cpu->do_cmd_help(arg);
}

int Cpu::do_cmd_help(char * arg)
{
    printf("Av3XX emulator by OxyGen\n");
    /* print all cmd */
    print_cmd_list();
    return 0;
}

int do_cmd_quit_s(char * arg) 
{ 
    return cur_cpu->do_cmd_quit(arg);
}


int Cpu::do_cmd_quit(char * arg)
{
#warning we can do a better stop
    printf("Bye\n\n");
    exit(0);
    return 0;
}

int do_cmd_step_s(char * arg) 
{ 
    return cur_cpu->do_cmd_step(arg);
}


int Cpu::do_cmd_step(char * arg)
{
    return 1;
}

int do_cmd_run_s(char * arg) 
{ 
    return cur_cpu->do_cmd_run(arg);
}


int Cpu::do_cmd_run(char * arg)
{
    run_mode=RUN;
    return 1;
}

int do_cmd_add_bkpt_s(char * arg) 
{ 
    return cur_cpu->do_cmd_add_bkpt(arg);
}


int Cpu::do_cmd_add_bkpt(char * arg)
{
    /*get first arg*/
    
    int i = my_atoi(arg);
    bkpt->add(i);
    return 0;
}

int do_cmd_print_stack_s(char * arg) 
{ 
    return cur_cpu->do_cmd_print_stack(arg);
}


int Cpu::do_cmd_print_stack(char * arg)
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

int do_cmd_print_state_s(char * arg) 
{ 
    return cur_cpu->do_cmd_print_state(arg);
}


int Cpu::do_cmd_print_state(char * arg)
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
    add_cmd_fct("bkpt",do_cmd_add_bkpt_s,"Add a breakpoint");
    add_cmd_fct("stack",do_cmd_print_stack_s,"Show stack");
    add_cmd_fct("sp",do_cmd_print_stack_s,"Show stack");
    add_cmd_fct("SP",do_cmd_print_stack_s,"Show stack");
    add_cmd_fct("state",do_cmd_print_state_s,"Show reg value");
    add_cmd_fct("reg",do_cmd_print_state_s,"Show reg value");
    
    
}
