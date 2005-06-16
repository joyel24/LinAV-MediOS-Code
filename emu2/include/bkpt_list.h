/* 
*   bkpt_list.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BKPT_LIST_H
#define __BKPT_LIST_H

#include "emu.h"

typedef struct bkpt_s {
    uint32_t address;
    struct bkpt_s * nxt;
} BKPT;

class bkpt_list {
    public:
        bkpt_list();
        
        void del(uint32_t address);
        void add(uint32_t address);
        bool has_bkpt(uint32_t address);
        void print_bkpt_list(void);
        
    private:
        BKPT * head;        
};

#endif

