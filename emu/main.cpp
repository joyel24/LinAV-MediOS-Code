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

#include "emu.h"
#include "memory.h"

Memory * mem;

int main(int argc, char* argv[])
{
    mem = new Memory(NULL,"gravity.bin");
    unsigned int data;
    for(unsigned int i = 0;i<0x10;i++)
    {
        data=mem->read_m(0x03000000+i*0x4,4);
        printf("%d|%08x\n",i,data);
    }
    delete(mem);
}
