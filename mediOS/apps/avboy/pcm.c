/* 
*   apps/avboy/pcm.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
* 
*  Date:     18/10/2005
* Author:   GliGli

*  Modified by CjNr11 06/12/2005
*/

#include <sys_def/string.h>

#include "defs.h"
#include "pcm.h"

void pcm_init(void)
{
    pcm.hz = 0;
    pcm.stereo = 0;
    pcm.buf = NULL;
    pcm.len = 0;
    pcm.pos = 0;
}

int pcm_submit()
{
    pcm.pos = 0;
    
    return 1;
}

void pcm_close(void)
{
    printf("pcm_close\n");
}
