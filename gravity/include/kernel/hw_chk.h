/* 
*   include/hw_chk.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __HW_CHK_H
#define __HW_CHK_H

#define HW_FREQ                     HZ/25

struct hw_chk_s {
    void (*action)(void);
    struct hw_chk_s * nxt;
    struct hw_chk_s * prev;
    char * name;
};

void init_HW_chk(void);
void add_hw_chker(struct hw_chk_s * hw_chk_data);
void del_hw_chker(struct hw_chk_s * hw_chk_data);


#endif
