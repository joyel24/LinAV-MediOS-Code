/*
*   kernel/fs/bin_load.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/hardware.h>
#include <kernel/stdfs.h>

#include <kernel/malloc.h>

#include <kernel/bin_load.h>

#include <kernel/errors.h>

void (*callBin)(void)=(void(*)(void))SDRAM_START;

int loadBin(char * path)
{
    int fd,size,ret;

    /* trying to open file */
    fd = open(path,ATTR_READ_ONLY);
    if(fd<0)
    {
        printk("Error opening file: %s, err=\n",path,-fd);
        return -MED_ENOENT;
    }

    size=filesize(fd);
    printk("File %s open, need to read %d bytes\n",size);

    ret=read(fd,(char*)SDRAM_START,size);
    printk("Read %d bytes done\n",ret);
    callBin();
    /* we should never get here !!!!!*/
    return MED_OK;
}

