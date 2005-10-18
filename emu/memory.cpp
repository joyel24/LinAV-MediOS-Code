/* 
*   memory.cpp
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
#include <string.h>
//#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>

#include "memory.h"

#define NB_MEM_ZONE 6

#define RESET_ZONE 0
#define FLASH_ZONE 3
#define SDRAM_ZONE 5

struct MemZone mem_zone_init[] = {
    {
        start:0x00000000,
        end:  0x00000003,
        type: MEMORY,
        name: "Reset"
    },
    {
        start:0x00000004,
        end:  0x00007fff,
        type: MEMORY,
        name: "ARM RAM"
    },
    {
        start:0x00030000,
        end:  0x0003FFFF,
        type: HARDWARE,
        name: "ARM HW"
    },
    {
        start:0x00100000,
        end:  0x0017ffff,
        type: MEMORY,
        name: "FLASH"
    },
    {
        start:0x02000000,
        end:  0x02FFFFFF,
        type: HARDWARE,
        name: "CPLD"
    },
    {
        start:0x03000000,
        end:  0x03FFFFFF,
        type: MEMORY,
        name: "SDRAM"
    }
};

Memory::Memory(char * flash,char * sdram)
{
    flash_file = flash;
    sdram_file = sdram;
    
    mem_zone = mem_zone_init;

    for(int i=0;i<NB_MEM_ZONE;i++)
    {
        if(mem_zone[i].type == MEMORY)
        {
            mem_zone[i].mem=new char[mem_zone[i].end-mem_zone[i].start+1];
            memset(mem_zone[i].mem, 0, mem_zone[i].end-mem_zone[i].start+1);            
        }
        else
        {
            /*NOTHING*/;
        }
    }
    
    /* init some mem zone */
    
    reset_init();
    flash_init();
    sdram_init();
}

Memory::~Memory()
{
    for(int i=0;i<NB_MEM_ZONE;i++)
    {
        if(mem_zone[i].type == MEMORY)
        {
            delete(mem_zone[i].mem);            
        }
        else
        {
            /*NOTHING*/;
        }
    }
}

uint32_t Memory::read_m(uint32_t addr,int size)
{
    for(int i=0;i<NB_MEM_ZONE;i++)
    {
        if(addr>=mem_zone[i].start && addr<=(mem_zone[i].end-size+1))
        {
            if(mem_zone[i].type == MEMORY)
            {
                switch(size)
                {
                    case 4:
                        //printf("size 4\n");
                        return ( ((uint32_t) (mem_zone[i].mem[addr-mem_zone[i].start] & 0xFF))
                                +(((uint32_t) (mem_zone[i].mem[addr-mem_zone[i].start+1] & 0xFF)) << 8)
                                +(((uint32_t) (mem_zone[i].mem[addr-mem_zone[i].start+2] & 0xFF)) << 16)
                                +(((uint32_t) (mem_zone[i].mem[addr-mem_zone[i].start+3] & 0xFF)) << 24)
                               );
                    case 2:
                        return ( ((uint32_t) (mem_zone[i].mem[addr-mem_zone[i].start  ] & 0xFF))
                                +(((uint32_t) (mem_zone[i].mem[addr-mem_zone[i].start+1] & 0xFF)) << 8)
                               );
                    case 1:
                        return (uint32_t) mem_zone[i].mem[addr-mem_zone[i].start] & 0xFF;
                        
                    default:
                        printf("Error: trying bad size (%d) reading memory %s at %08x\n",size,mem_zone[i].name,addr);
                        return 0;
                }
            }
            else            
            {
                printf("HW access: %s, at %08x (size: %d)\n",mem_zone[i].name,addr,size);
                return 0;
            }
            printf("Memory::read(%08x,%d) ERROR should not come here\n",addr,size);            
            return 0;
        }
    }
    
    printf("Memory::read(%08x,%d) ERROR addr out of defined mem zone\n",addr,size);            
    return 0;
}

void Memory::write_m(uint32_t addr,uint32_t val,int size)
{
    for(int i=0;i<NB_MEM_ZONE;i++)
    {
        if(addr>=mem_zone[i].start && addr<=(mem_zone[i].end-size+1))
        {
            if(mem_zone[i].type == MEMORY)
            {
                switch(size)
                {
                    case 4:
                        mem_zone[i].mem[addr-mem_zone[i].start  ] = val       & 0xFF;
                        mem_zone[i].mem[addr-mem_zone[i].start+1] = (val>>8)  & 0xFF;
                        mem_zone[i].mem[addr-mem_zone[i].start+2] = (val>>16) & 0xFF;
                        mem_zone[i].mem[addr-mem_zone[i].start+3] = (val>>24) & 0xFF;
                        return;
                    case 2:
                        mem_zone[i].mem[addr-mem_zone[i].start  ] = val       & 0xFF;
                        mem_zone[i].mem[addr-mem_zone[i].start+1] = (val>>8)  & 0xFF;
                        return;
                    case 1:
                        mem_zone[i].mem[addr-mem_zone[i].start] = val & 0xFF;
                        return;
                        
                    default:
                        printf("Error: trying bad size (%d) reading memory %s at %08x\n",size,mem_zone[i].name,addr);
                        return;
                }
            }
            else            
            {
                printf("HW access: %s, at %08x (size: %d)\n",mem_zone[i].name,addr,size);
                return;
            }
            printf("Memory::read(%08x,%d) ERROR should not come here\n",addr,size);            
            return;
        }
    }
    
    printf("Memory::read(%08x,%d) ERROR addr out of defined mem zone\n",addr,size);            
    return;
}


void Memory::reset_init(void)
{
    mem_zone[RESET_ZONE].mem[0]=0xFE;
    mem_zone[RESET_ZONE].mem[1]=0xFF;
    mem_zone[RESET_ZONE].mem[2]=0x03;
    mem_zone[RESET_ZONE].mem[3]=0xEA;
}

void Memory::flash_init(void)
{
    int fd;
    struct stat s;
  
    if(flash_file)
    {
        fd = open(flash_file, O_RDONLY);
        if (fd == -1)
            printf("ERROR: Loading flash init: %s\n",flash_file);
        else
        {        
            fstat(fd, &s);
            read(fd, mem_zone[FLASH_ZONE].mem, s.st_size);
            close(fd);
            printf("Loading file %s in flash done (%d bytes read)\n",flash_file,s.st_size);
        }
    }
    else
        printf("INFO: no file to load in flash\n");
}

void Memory::sdram_init(void)
{
    int fd;
    struct stat s;
  
    if(sdram_file)
    {
        fd = open(sdram_file, O_RDONLY);
        if (fd == -1)
            printf("Error: Loading sdram init: %s\n",sdram_file);
        else
        {        
            fstat(fd, &s);
            read(fd, mem_zone[SDRAM_ZONE].mem, s.st_size);
            close(fd);
            printf("Loading file %s in SDRAM done (%d bytes read)\n",sdram_file,s.st_size);
        }
    }
    else
        printf("INFO: no file to load in SDRAM\n");
}
