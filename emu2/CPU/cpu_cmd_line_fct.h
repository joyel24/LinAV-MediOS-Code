
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
    
    int i = atoi(arg);
    printf("%s: %x\n",arg,i);
    printf("not working atm\n");
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
    
}
