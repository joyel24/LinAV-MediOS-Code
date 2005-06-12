/*
* kernel/gui/internal_plugin/mp3_player/play.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <api.h>
#include <fs_io.h>
#include <kernel/sound.h>

#if 0

#define MP3_BUFF_SIZE (1020*500)

#define MIN(a, b) (((a)<(b))?(a):(b))

sound_buffer_s mp3_buffer;

int fd;
int file_pos,file_size,file_eof;

int readFromFile(char * data, int count, void * param)
{
#if 0
    int tread=0;
    int res;
    while(count>0)
    {
        res=fread((int)param,(void*)data,MIN(count,100));
        if(res<=0)
            return tread;
        tread+=res;
        count-=res;
    }
    return tread;
#else
    
    count=fread((int)param,(void*)data,count);
    if(count<0)
        count=0;
    return count;
#endif
}


void mp3_read_data(int count)
{
    count=MIN(count,file_size-file_pos);
    if(count==0)
    {
        fclose(fd);
        file_eof=1;
        printf("EOF\n");
        return;
    }
    printf("read: %d (%d/%d)\n",count,file_pos,file_size);
    count=sound_buffer_write(&mp3_buffer, readFromFile,count,(void *) fd);
    file_pos+=count;
}

void mp3_file_reader_thread(void * param)
{
    int free_space;
    while(!file_eof)
    {
        free_space = mp3_buffer.read - mp3_buffer.write;
        if(free_space<=0)
            free_space+=mp3_buffer.size;
        if(free_space>mp3_buffer.size/2)
            mp3_read_data(free_space);
        API_TASK_SLEEP(1000);
    }
}

int start_mp3_player(char * path)
{
    
    /* openning current file */
    
    fd=fopen(path,O_RDONLY);
    if(fd<0)
    {
        printf("Error opening file %s\n",path);
    }
        
    file_pos=0;
    file_size=filesize(fd);
    file_eof=0;
    
    printf("about to play: %s, size=%d\n",path,file_size);
    
    /* setup sound buffer struct */
    mp3_buffer.size=MP3_BUFF_SIZE;
    mp3_buffer.write=0;
    mp3_buffer.read=0;    
    
    mp3_buffer.data=(char*)malloc(sizeof(char)*MP3_BUFF_SIZE);
    if(!mp3_buffer.data)
    {
        printf("Error, can't allocate buffer\n");
        return 0;
    }
    
    dsp_ctl(DSP_INI_MP3,&mp3_buffer);
    
    mp3_read_data(1020*10); /* read some data */
    
    /* start mp3 */
    
    dsp_ctl(DSP_START_MP3,NULL);
    
    /* start file reader */
    
    API_TASK_CREATE (mp3_file_reader_thread, NULL, NULL);
    
#warning need to add code to remove timer chk for power off    

while(1) /*nothing*/;

printf("MP3 Before exit\n");

    free(mp3_buffer.data);

    return 1;
}


#endif
