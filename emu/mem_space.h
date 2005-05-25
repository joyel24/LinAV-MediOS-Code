/* 
*   mem_space.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MEM_SPACE_H
#define __MEM_SPACE_H

#include "emu.h"
#include "HW_access.h"

class mem_space:public HW_access {
    public:
        mem_space(char * flash,char * sdram);
        ~mem_space();        
};

#endif
