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


#include "HW_mem.h"


HW_mem::HW_mem(char * fname,uint32_t start,uint32_t end,char * name):HW_access(start,end,name)
{
    mem = new char[end - start + 1 ];
    memset(mem,0,end - start + 1 );
    
    FILE * fd;
    int fsize;
  
    if(fname)
    {
        fd = fopen(fname, "rb");
        if (!fd)
            printf("Error: Loading %s init: %s\n",name,fname);
        else
        {        
            fseek (fd , 0 , SEEK_END);
            fsize = ftell (fd);
            rewind (fd);
            fread(mem, 1, fsize,fd);
            fclose(fd);
            printf("Loading file %s in %s done (%d bytes read)\n",name,fname,fsize);
        }
    }
    else
        printf("INFO: no file to load in %s\n",name);
}

HW_mem::~HW_mem()
{
    delete(mem);
}

uint32_t HW_mem::read(uint32_t addr,int size)
{
    switch(size)
    {
        case 4:
            return ( ((uint32_t) (mem[addr-start] & 0xFF))
                    +(((uint32_t) (mem[addr-start+1] & 0xFF)) << 8)
                    +(((uint32_t) (mem[addr-start+2] & 0xFF)) << 16)
                    +(((uint32_t) (mem[addr-start+3] & 0xFF)) << 24)
                    );
        case 2:
            return ( ((uint32_t) (mem[addr-start  ] & 0xFF))
                    +(((uint32_t) (mem[addr-start+1] & 0xFF)) << 8)
                    );
        case 1:
            return (uint32_t) mem[addr-start] & 0xFF;
            
        default:
            printf("Error: trying bad size (%d) reading memory %s at %08x\n",size,name,addr);
            exit(0);
    }
    exit(0);
}

void HW_mem::write(uint32_t addr,uint32_t val,int size)
{
    switch(size)
    {
        case 4:
            mem[addr-start  ] = val       & 0xFF;
            mem[addr-start+1] = (val>>8)  & 0xFF;
            mem[addr-start+2] = (val>>16) & 0xFF;
            mem[addr-start+3] = (val>>24) & 0xFF;
            return;
        case 2:
            mem[addr-start  ] = val       & 0xFF;
            mem[addr-start+1] = (val>>8)  & 0xFF;
            return;
        case 1:
            mem[addr-start] = val & 0xFF;
            return;
            
        default:
            printf("Error: trying bad size (%d) writing memory %s at %08x\n",size,name,addr);
            exit(0);
    }
    exit(0);        
}

/*
void Memory::reset_init(void)
{
    mem_zone[RESET_ZONE].mem[0]=0xFE;
    mem_zone[RESET_ZONE].mem[1]=0xFF;
    mem_zone[RESET_ZONE].mem[2]=0x03;
    mem_zone[RESET_ZONE].mem[3]=0xEA;
}
*/
