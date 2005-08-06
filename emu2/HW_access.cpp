/*
*   hw_access.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdio.h>
#include <stdlib.h>

#include <emu.h>

#include <HW_access.h>

HW_access::HW_access(uint32_t start,uint32_t end, char * name)
{
    this->start = start;
    this->end = end;
    this->name=name;

    exit_on_not_match = true;
    data_abt_on_not_match = false;
    full_size = false;
}

extern bool data_abort;

