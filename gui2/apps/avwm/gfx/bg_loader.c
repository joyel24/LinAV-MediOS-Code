/*
* bg_loader.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "graphics.h"
#include "osd.h"

#include "bg_loader.h"

extern struct graphicsBuffer VIDEO_1;

void load_bg(char * name)
{
    int fd;int tot=0;int val;
    int state;
    
    fd = open(name, 0);
    if (fd < 0)
    {
        perror(name);
        return ;
    }
    
    while ((val=read(fd,(int*)(VIDEO_1.offset + tot), 320)) > 0)
        tot+=val;
    fprintf(stderr,"Read: %d\n",tot);
    
    showPlane(VID1);
    
    state=getState(BMAP1);
    //setState(BMAP1,state | AV3XX_OSD_BITMAP_A7);
     setState(BMAP1,AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT  | AV3XX_OSD_BITMAP_A7);
     showPlane(BMAP1);
    
}
