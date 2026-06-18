/*
* test.c
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

#include <api.h>
#include <sys_def/arch.h>


void app_main(int argc,char* argv)
{
    int ver,w,h;

    printf("in test\n");
    
    ver=getArch();
    getResolution(&w,&h);
    printf("We are on %s\n resolution is: (%d,%d)\n",ver==AV3XX_ARCH?"AV3XX":ver==GMINI4XX_ARCH?"Gmini4xx":"UKN ARCH",
        w,h
    );
}
