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


#include <HW_mem.h>

HW_mem::HW_mem(char * fname,uint32_t start,uint32_t end,char * name, uint32_t load_offset):HW_access(start,end,name)
{
  init(fname,start,end,load_offset);
}

HW_mem::HW_mem(char * fname,uint32_t start,uint32_t end,char * name):HW_access(start,end,name)
{
  init(fname,start,end,0);
}

HW_mem::~HW_mem()
{
    delete(mem);
}

void HW_mem::init(char * fname,uint32_t start,uint32_t end,uint32_t load_offset){
    mem = new char[ end - start + 1 ];
    memset(mem,0,end - start + 1 );

    FILE * fd;
    int fsize;

    full_size = true;

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

            fread(&mem[load_offset], 1, fsize,fd);

            fclose(fd);
            printf("Loading file %s in %s done (%d bytes read)\n",fname,name,fsize);
        }
    }
    else
        printf("INFO: no file to load in %s\n",name);
}

uint32_t HW_mem::read(uint32_t addr,int size)
{
    return  (*((unsigned long *) (mem+addr-start))) & (0xffffffff>>((4-size)<<3));
}

void HW_mem::write(uint32_t addr,uint32_t val,int size)
{
    unsigned long tmp = *((unsigned long *) (mem+addr-start));
    tmp = (size>=4) ? val : (tmp & (0xffffffff<<(size<<3))) | (val & (0xffffffff>>((4-size)<<3)));
    *((unsigned long *) (mem+addr-start))=tmp;
}

