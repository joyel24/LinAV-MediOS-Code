/*
*   include/sys_def/alloca.h
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

#ifndef __ALLOCA_H
#define __ALLOCA_H

void *__builtin_alloca(size_t size);

#define alloca(size) __builtin_alloca(size)

#endif
