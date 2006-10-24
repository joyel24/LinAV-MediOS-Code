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

#include <kernel/cache.h>
#include <kernel/io.h>

#include <kernel/irq.h>

__attribute__((section(".fwuncomp_code"))) int bin_load(char * path)
{
    void (*callBin)(void)=(void(*)(void))SDRAM_START;
    int fd,size,ret;

    int i;
    void * buf;
    long long * src;
    long long * dest;

    /* trying to open file */
    fd = open(path,ATTR_READ_ONLY);
    if(fd<0)
    {
        printk("Error opening file: %s, err=\n",path,-fd);
        return -MED_ENOENT;
    }

    size=filesize(fd);
    printk("File %s open, need to read %d bytes\n",path,size);

    buf=malloc(size);

    ret=read(fd,buf,size);
    printk("Read %d bytes done\n",ret);

    // copy data to sdram start
    src=buf;
    dest=(void*)SDRAM_START;
    for(i=size/8;i>=0;--i){
        *(dest++)=*(src++);
    }

    // take care of the cache (I directly use macros because functions are overwritten at this point)
    CACHE_CLEAN();
    CACHE_DISABLE(CACHE_ALL);
    CACHE_INVALIDATE(CACHE_ALL);

    callBin();

    /* we should never get here !!!!!*/
    return MED_OK;
}

