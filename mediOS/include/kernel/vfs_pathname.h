/* 
*   include/kernel/vfs_pathname.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __VFS_PATHNAME_H
#define __VFS_PATHNAME_H

struct vfs_pathname {
    char * str;
    int length;
};

int vfs_pathnameEatSlashes(struct vfs_pathname * path,
                    struct vfs_pathname * result);

int vfs_pathnameEatNonSlashes(struct vfs_pathname * path,
                        struct vfs_pathname * result);

int vfs_pathnameSplit(struct vfs_pathname * path,
                    struct vfs_pathname * result_first_component,
                    struct vfs_pathname * result_remaining_path);

int vfs_pathnameIsEq(struct vfs_pathname * p1,
                    struct vfs_pathname * p2);

#define vfs_pathnameIsStr(str_val,path) \
  ({ struct vfs_pathname _s; _s.str = str_val; _s.length = sizeof(str_val)-1; \
     vfs_pathnameIsEq(&_s, (path)); })                                    
#endif
