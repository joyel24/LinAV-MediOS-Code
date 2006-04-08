/* 
*   include/kernel/evt.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __EVT_K_H
#define __EVT_K_H

#include <evt.h>
#include <kernel/pipes.h>
#include <kernel/errors.h>

#define EVT_DELAY HZ
#define NB_EVT_PIPES 10

struct evt_pipes {
    struct pipe evt_pipe;
    unsigned int mask;
    int used;
};

int evt_getHandler(unsigned int mask);
MED_RET_T evt_freeHandler(int num_evt_pipe);
void evt_send(struct evt_t * evt);                
int  evt_getStatus(int num_evt_pipe);        
MED_RET_T evt_getFullStatus(int num_evt_pipe, struct evt_t * evt);
int evt_purgeHandler(int num_evt_pipe);
void evt_init(void);


#endif
