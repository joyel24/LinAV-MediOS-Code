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

#include <sys_def/malloc.h>

void  mem_addPool  (void *buffer, unsigned int len);

void* do_malloc    (unsigned int size,int user_flag);

void* do_realloc   (void *buffer, unsigned int newsize,int user_flag);

void  free         (void *buf);
void free_user     (void);

void mem_stat      (unsigned int *curalloc_user, unsigned int *curalloc_kernel,
            unsigned int *totfree,unsigned int *maxfree);
void mem_printStat(void);
void mem_freeList  (void);
#endif

