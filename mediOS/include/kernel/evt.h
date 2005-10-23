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

struct evt_pipes_s {
    PIPE evt_pipe;
    struct evt_pipes_s * nxt;
};

struct evt_pipes_s * get_evt_handling(void);
void rm_evt_handling(struct evt_pipes_s * evt_pipes);

#ifdef HAVE_EVT
#define send_evt(EVT) do_send_evt(EVT)
#else
#define send_evt(EVT)
#endif

void do_send_evt(int evt);

void init_evt(void);


#endif
