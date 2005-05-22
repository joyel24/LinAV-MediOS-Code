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
#include "cpu.h"

Memory * mem;
Cpu    * cpu;

int main(int argc, char* argv[])
{
    mem = new Memory(NULL,"gravity.bin");
    
    cpu = new Cpu(mem);
    
    cpu->go(0x03000000,0x000080000-0x4);
    
    delete(mem);
    delete(cpu);
}
