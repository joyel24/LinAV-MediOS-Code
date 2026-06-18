/*
*   hw_node.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_NODE_H
#define __HW_NODE_H

#include "emu.h"
#include "HW_access.h"
#include "HW_dummy.h"

class HW_node: public HW_access {

protected:
    void add_item(HW_access * ptr);
//    void rm_item(HW_access * ptr);

    HW_dummy * dummy_hw;

public:
    HW_node(uint32_t start,uint32_t end, uint32_t granularity_shift, char * name);

    uint32_t        start;
    uint32_t        end;
    uint32_t        granularity_shift;
    char *          name;
    HW_access * *   access_array;

    virtual uint32_t read(uint32_t addr,int size);
    virtual void write(uint32_t addr,uint32_t val,int size);
    
};

#endif

