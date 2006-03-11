/*
*   include/sys_def/malloc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#ifndef __SYS_DEF_MALLOC_H
#define __SYS_DEF_MALLOC_H

void* malloc       (unsigned int size);

void* realloc      (void *buffer, unsigned int newsize);

/* defined in kernel/malloc.h because already defined in api.h
for apps
 void  free         (void *buf);
 */


#endif
