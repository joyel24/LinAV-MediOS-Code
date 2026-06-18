/* 
*   include/memmgr.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __MEMMGR_H
#define __MEMMGR_H

extern void* kmalloc (int nBytes);
extern void kfree (void* ptr);
extern unsigned long kmemavail ();
extern void init_malloc (void *beg, long size);
#endif
