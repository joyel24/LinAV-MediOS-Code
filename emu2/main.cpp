/* 
*   main.cpp
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

#include <emu.h>
#include <mem_space.h>
#include <cpu.h>
#include <cmd_line.h>

mem_space * mem;
Cpu    * cpu;

enum cmd_type {NO_CMD=0x0,F_SDRM='s',F_FLASH='f',C_HELP='h'};

void usage(char * name)
{
    printf("Usage: %s -f name|-s name|-h\n-f : file to be loaded in flash\n-s : file to be loaded in sdram\n-h: help\n",name);
    exit(0);
}

void cmd_line(void);

int main(int argc, char* argv[])
{
    int i;
    char * sdram_file = NULL;
    char * flash_file = NULL;
    
    int cur_cmd = NO_CMD;
    
    for(i=1;i<argc;i++)
    {
        printf("cur_cmd= %c\n",cur_cmd==0?' ':cur_cmd);
        if(cur_cmd!=NO_CMD)
        {            
            switch(cur_cmd)
            {
                case F_SDRM:
                    sdram_file = argv[i];
                    break;
                case F_FLASH:
                    flash_file = argv[i];
                    break;
            }
            cur_cmd = NO_CMD;
        }
        else
        {        
            if(argv[i][0] == '-')
            {
                switch(argv[i][1])
                {
                    case F_SDRM:
                    case F_FLASH:
                        cur_cmd = argv[i][1];
                        break;
                    case C_HELP:
                        usage(argv[0]);
                    default:
                        cur_cmd = NO_CMD;                
                }
            }
        }
    }
    
    if(sdram_file == NULL)
    {
        usage(argv[0]);
    }
    
    init_cmd_line();
    
    //sdram_file = "CJBM_v2.depack";
    
    mem = new mem_space(flash_file,sdram_file);
    
    cpu = new Cpu(mem);
    
    cpu->go(0x03000000,0x000080000-0x4);
    
    delete(mem);
    delete(cpu);
}
