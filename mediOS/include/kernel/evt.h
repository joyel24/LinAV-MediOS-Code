/* 
*   include/kernel/evt.h
*
*   AMOS project
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

#define EVT_DELAY HZ
#define NB_EVT_PIPES 10

struct evt_pipes {
    struct pipe evt_pipe;
    unsigned int mask;
    int used;
};

struct evt_t {
    int evt;
    unsigned int evt_class;
    void * data;
};

int evt_getHandler(unsigned int mask,int * result);
int evt_freeHandler(int num_evt_pipe);



void evt_send(struct evt_t * evt);                      /* kernel drivers use it to send evt */
int  evt_getStatus(int num_evt_pipe, int * result);         /* apps use it to read evt on their pipe */

void evt_init(void);


#endif
