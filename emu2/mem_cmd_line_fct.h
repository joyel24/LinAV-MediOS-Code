/* 
*   mem_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/



int do_cmd_dump_s(int argc,char ** argv) 
{ 
    return mem_obj->do_cmd_dump(argc,argv);
}


int mem_space::do_cmd_dump(int argc,char ** argv)
{
    uint32_t start_addr=0;
    int size = 0x200;
    if(argc>0)
    {
        start_addr = my_atoi(argv[0]);
        if(argc>1)
        {
            size = my_atoi(argv[1]);                   
        }
        my_print_mem(this,(char*)start_addr,size);
    } 
    return 0;
}

int do_cmd_add_bk_mem_s(int argc,char ** argv) 
{ 
    return mem_obj->do_cmd_add_bk_mem(argc,argv);
}

int mem_space::do_cmd_add_bk_mem(int argc,char ** argv)
{
    /*get first arg*/
    uint32_t i=0;
    uint32_t size=0;
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
            bkpt->del(i,BKPT_MEM);
        else
        {
            if(argc>1)
                size=my_atoi(argv[1]);
            bkpt->add(i,size,BKPT_MEM);
        }
    }
    else
        bkpt->print_bkpt_list(BKPT_MEM);
    return 0;
}

void init_mem_static_fct(mem_space * mem)
{
    mem_obj = mem;
    add_cmd_fct("dump",do_cmd_dump_s,"Dump mem to screen");
    add_cmd_fct("bkm",do_cmd_add_bk_mem_s,"Manage mem bkpt"); 
}
