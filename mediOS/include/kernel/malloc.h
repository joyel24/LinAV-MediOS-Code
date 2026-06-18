/*
*   include/api.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MALLOC_H
#define __MALLOC_H

#include <kernel/thread.h>

void  mem_init     (void * start,unsigned int len);

void  mem_addPool  (void *buffer, unsigned int len);

void* malloc       (unsigned int size);
void* kmalloc      (unsigned int size);

void* realloc      (void *buffer, unsigned int newsize);
void* krealloc     (void *buffer, unsigned int newsize);

void  free         (void *buf);
void  kfree        (void * buf);

void mem_stat      (unsigned int *curalloc,unsigned int *totfree,unsigned int *maxfree);
void mem_printShortStat(void);
void mem_printStat (void);
void mem_freeList  (void);

/* internal struct */

/* Queue links */
struct qlinks {
    struct bhead *nxt;        /* Forward link */
    struct bhead *prev;       /* Backward link */
};

/* Header in allocated and free buffers */
struct bhead {
    int size; 
    int magic_val;   
    struct qlinks blist;
    struct qlinks free_list;
    THREAD_LINKS
};

void mem_printItem (struct bhead * ptr);

void internalFree(struct bhead * b);
void * internalMalloc(unsigned int  requested_size,int isKernel);
void * internalRealloc(void *buf,unsigned int size,int isKernel);

#endif

