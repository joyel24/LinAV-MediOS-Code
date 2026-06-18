/* 
*   HW_null.cpp
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

#include "HW_uart.h"

HW_uart::HW_uart(uint32_t start,uint32_t end,char * name):HW_access(start,end,name)
{
    char fname[]="uartN";
    switch(start)
    {
        case 0x30300:
            uartNum=0;
            break;
        case 0x30380:
            uartNum=1;
            break;
        default:
            printf("UART base address is wrong : 0x%08x\n",start);
            return;
    }
    
    fname[4]=0x30+uartNum;
    fd = fopen(fname, "w");
    if (!fd)
        printf("Error: opening: %s for %s\n",fname,name);
    else
    {
        printf("%s init: output send to %s\n",name,fname);
        fprintf(fd,"%s ouptput\n",name);
        fflush(fd);
    }    
    
}

HW_uart::~HW_uart()
{
    fclose(fd);
}

uint32_t HW_uart::read(uint32_t addr,int size)
{
    switch(addr&0xF)
    {
        case 0xc:
            return 0xffff;
        default:        
            printf("%s read @0x%08x, size %x\n",name,addr,size);
            break;
    }
    return 0;
}

void HW_uart::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr&0xF)
    {
        case 0x0:
            if(fd)
                fputc(val&0xFF,fd);
                fflush(fd);
            break;
        default:
            printf("%s write %x @0x%08x, size %x\n",name,val,addr,size);
            break;
    }
}
