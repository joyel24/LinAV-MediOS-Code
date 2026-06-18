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

#include <kernel/disk.h>
#include <kernel/ata.h>

#include <kernel/ide.h>

extern int hd_sleep_state;

void halt_HD(void)
{
    disk_umount(HD_DRIVE,FLUSH); 
    hd_sleep_state=1;
    ata_stop_HD();
}

