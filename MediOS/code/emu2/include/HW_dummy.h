/*
*   hw_dummy.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_DUMMY_H
#define __HW_DUMMY_H

#include "emu.h"
#include "HW_access.h"

class HW_dummy: public HW_access {

public:
    HW_dummy(char * name);

    virtual uint32_t read(uint32_t addr,int size);
    virtual void write(uint32_t addr,uint32_t val,int size);
};

#endif

