/* 
*   gui/avwm.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <graphics.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/time.h>
#include <sys_def/font.h>
#include <api.h>
#include <fs_io.h>

#include <gui/icons.h>
#include <sound.h>

#include <kernel/io.h>
#include <kernel/gio.h>
#include <kernel/hardware.h>


extern void ini_status_bar(void);
extern void drawStatusLine(void);

#define MP3_SIZE 1020*2000


void avwm(void)
{
    DIR * dir;
    struct dirent* entry;
    

    
    int fd,size,cnt;
    struct mp3_play data;
    
    
    printf("Starting AvWm\n");
    
    open_graphics();
    clearScreen(COLOR_WHITE);
    setFont(STD6X9);
    iniIcon();
    
    ini_status_bar();
    drawStatusLine(); /* should be done via EVT_REDRAW */
    
    fd=fopen("/file.mp3",O_RDONLY);
    if(fd<0)
    {
        printf("Can't open mp3 file\n");
    }
    else
    {
        size=filesize(fd);
        if(size>MP3_SIZE)
            size=MP3_SIZE;
        data.buffer=malloc(size);
        if(!data.buffer)
        {
            printf("can't allocate mp3 buffer\n");
        }
        else
        {
            cnt=fread(fd,data.buffer,size);
            if(cnt<=0)
            {
                printf("Error reading file\n");
            }
            else
            {
                if(cnt<size)
                {
                    size=cnt;
                    printf("only read: %x\n",cnt);
                }                
                else
                    printf("read: %x\n",cnt);
                data.buffer_len=size;
                data.pos=0;
                data.finished=0;
                data.buffer_read=0;
                data.buffer_write=size;
                data.endOfFile=0;
                data.freqPeakDraw=10;
                data.peakDraw=NULL;//drawPeak;
                
                dsp_ini_mp3(&data);
                
                /*while(1)
                {
                    dsp_interrupt(0);
                }*/

                dsp_start_mp3();
                printf("out of play\n");
            }
        }
    }
    
    
    
    while(1) /*nothing*/;
}
