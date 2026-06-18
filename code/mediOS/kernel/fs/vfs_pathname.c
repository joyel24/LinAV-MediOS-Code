/*
*   kernel/fs/vfs_pathname.c
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
#include <sys_def/string.h>
#include <kernel/kernel.h>
#include <kernel/errors.h>

#include <kernel/vfs_pathname.h>

int vfs_pathnameEatSlashes(struct vfs_pathname * path,
                    struct vfs_pathname * result)
{
    int retval = 0;

    result->str = path->str;
    result->length   = path->length;

    while (result->length > 0)
    {
        if (*result->str != '/')
            break;

        result->str ++;
        result->length --;
        retval = 1;
    }

    if(result->length <= 0)
        result->str = NULL;

    return retval;
}

int vfs_pathnameEatNonSlashes(struct vfs_pathname * path,
                        struct vfs_pathname * result)
{
    int retval = 0;

    result->str = path->str;
    result->length   = path->length;

    while (result->length > 0)
    {
        if (*result->str == '/')
        break;

        result->str ++;
        result->length --;
        retval = 1;
    }

    if(result->length <= 0)
        result->str = NULL;

    return retval;
}

void vfs_pathnameSplitPath(struct vfs_pathname * path,
                    struct vfs_pathname * result_path_component,
                    struct vfs_pathname * result_file_component)
{
    int old_length = path->length;

    result_path_component->str      = path->str;
    result_path_component->length   = path->length;


    result_file_component->str = strrchr(path->str, '/')+1;

    result_path_component->length = (unsigned int)result_file_component->str - (unsigned int)result_path_component->str;

    result_file_component->length = old_length - result_path_component->length;
}

void vfs_pathNameDup(struct vfs_pathname * src,struct vfs_pathname * dest,char * dest_str)
{
    /* keep track of src string */
    char * src_str = src->str;
    /* init dest */
    dest->length = src->length;
    dest->str=dest_str;
    /* if length is > 0 => do the copy */
    if(src->length)
    {
        strncpy(dest->str,src_str,src->length+1);
    }
    else
    {
        dest->str[0]='\0';
    }
}

int vfs_pathnameSplit(struct vfs_pathname * path,
                    struct vfs_pathname * result_first_component,
                    struct vfs_pathname * result_remaining_path)
{
  result_first_component->str      = path->str;
  result_first_component->length   = path->length;

  /* Skip any leading slash */
  vfs_pathnameEatSlashes(result_first_component,result_first_component);

  /* Extract the first component */
  vfs_pathnameEatNonSlashes(result_first_component,result_remaining_path);

  if(result_remaining_path->length<0)
  {
    printk("[FATAL ERROR] in vfs_pathnameSplit bad lencth in result\n");
    return 0;
  }

  result_first_component->length -= result_remaining_path->length;

  /* Return true if there is something left (at least one slash) */
  return (result_remaining_path->length > 0);
}

int vfs_pathnameIsEq(struct vfs_pathname * p1,
                    struct vfs_pathname * p2)
{
  if (!p1->str && p1->length != 0)
  {
    printk("[FATAL ERROR] in vfs_pathnameIsEq inconsistent p1\n");
    return 0;
  }

  if (!p2->str && p2->length != 0)
  {
    printk("[FATAL ERROR] in vfs_pathnameIsEq inconsistent p2\n");
    return 0;
  }

  if (p1->length != p2->length)
    return 0;

  if (p1->length == 0)
    return 1;
#warning we use FAT like cmp for now
    return (0 == strncasecmp(p1->str, p2->str, p1->length));
//  return (0 == memcmp(p1->str, p2->str, p1->length));
}

