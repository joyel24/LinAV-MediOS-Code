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

#define BKPT_CPU   0
#define BKPT_MEM   1

typedef struct bkpt_s {
    uint32_t address;
    uint32_t size;
    struct bkpt_s * nxt;
    char * cause;
} BKPT;



class bkpt_list {
    public:
        bkpt_list();
        
        void del(uint32_t address,int type);
        void add(uint32_t address,int type);
        void add(uint32_t address,int type,char * cause);
        void add(uint32_t address,uint32_t size,int type);
        void add(uint32_t address,uint32_t size,int type,char * cause);
        bool has_bkpt(uint32_t address,int type);
        void print_bkpt_list(int type);
        
    private:
        BKPT * head[2];
        char * bkpt_str[2];      
};

#endif

