/*
*   include/kernel/vfs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __VFS_H
#define __VFS_H


//#define DEBUG_VFS

#ifdef DEBUG_VFS
#define VFS_PRINT(s...)   printk(s)
#else
#define VFS_PRINT(s...)
#endif

MED_RET_T vfs_init(int device,unsigned int startsector);
void vfs_rootPrint(void);

#endif
