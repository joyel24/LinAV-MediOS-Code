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

#define BKPT_CPU        0
#define BKPT_MEM        1
#define BKPT_STEPOVER   2

#define BKPT_MEM_READ   0
#define BKPT_MEM_WRITE  1

typedef struct bkpt_s {
    uint32_t address;
    uint32_t size;
    struct bkpt_s * nxt;
    char * cause;
} BKPT;

typedef struct bkpt_list {
    BKPT * head;
    int type;
    int bk_count;
    void (*fct)(struct bkpt_list * ptr_list,uint32_t address,int mode);
} BKPT_LIST;

BKPT_LIST * new_bkpt_list(int type);
void add(BKPT_LIST * ptr,uint32_t address,char * cause);
void add(BKPT_LIST * ptr,uint32_t address);
void add(BKPT_LIST * ptr,uint32_t address,uint32_t size);
void add(BKPT_LIST * ptr_list,uint32_t address,uint32_t size,char * cause);
void del(BKPT_LIST * ptr_list,uint32_t address);
void delN(BKPT_LIST * ptr_list,int num);
void updateFctPointer(BKPT_LIST * ptr_list);
void fct_void(BKPT_LIST * ptr_list,uint32_t address,int mode);
void has_bkpt(BKPT_LIST * ptr_list,uint32_t address,int mode);
void print_bkpt_list(bkpt_list * ptr_list);

#endif

