/* 
*   kernel/fs/ide.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kfile.h>
#include <kernel/kdir.h>
#include <api.h>


int fs_swi(int cmd,void * data1, void * data2)
{
    COUPLE_DATA * data=((COUPLE_DATA *)data1);
    off_t * off;
    ssize_t * size;
    switch(cmd) {
        case 0x000:            
            *(int*)data2=kfopen((const char*) data1, *(int*)data2);
            break;
        case 0x001:
            *(int*)data2=kfclose((int)data1);
            break;
        case 0x002:
            *(int*)data2=kfsync((int)data1);
            break;
        case 0x003:            
            *(ssize_t *)data2=kread((int)data->a,data->b,*(ssize_t *)data2);
            break;
        case 0x004:
            off=(off_t *)data2;
            *off=klseek((int)data->a,*off,(int)data->b);
            break;
        case 0x005:
            off=(off_t *)data2;
            *off=kftell((int)data1);
            break;
        case 0x006:
            *(int *)data2=kcreat((const char *)data->a,(mode_t)data->b);
            break;
        case 0x007:
            size=(ssize_t *)data2;
            *size=kwrite((int)data->a,data->b,*size);
            break;
        case 0x008:
            *(int *)data2=kremove((const char*)data1);
            break;
        case 0x009:
            *(int *)data2=krename((const char*)data->a,(const char*)data->b);
            break;
        case 0x00A:
            *(int *)data2=kftruncate((int)data->a,(off_t)data->b);
            break;
        case 0x00B:
            *(int *)data2=kremove((int)data1);
            break;
            
        case 0x100:
            *(DIR**)data2=kopendir((const char*) data1);
            break;
        case 0x101:
            *(int*)data2=kclosedir((DIR*) data1);
            break;
         case 0x102:
            *(int*)data2=kmkdir((const char*) data1, *(int*)data2 );
            break;
         case 0x103:
            *(int*)data2=krmdir((const char*) data1);
            break;
         case 0x104:
            *(struct dirent**)data2=kreaddir((DIR*) data1);
            break;
         default:
             printk("FS undefined swi (%d)\n",cmd);
    }
    return 0;
}

