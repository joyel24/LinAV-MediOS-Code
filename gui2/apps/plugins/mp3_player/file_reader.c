/*
* file_reader.c
*
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
#include <pthread.h>

#include "cops.h"
#include "mp3_player.h"

FILE * fd=NULL;
int file_size;

/*extern variables */
extern struct mp3_play data;
extern int stopThread;
extern pthread_t read_thread;

#define MIN(a, b) (((a)<(b))?(a):(b))

int threadActive=0;

void mp3_read_more(void)
{
    int free_space;
    //fprintf(stderr,"r=%x w=%x dif=%x\n",data.buffer_read,data.buffer_write,data.buffer_write-data.buffer_read);
    if(!threadActive && !data.endOfFile )
    {        
        free_space=data.buffer_read-data.buffer_write;
        if(free_space<=0)
            free_space+=data.buffer_len;
        if(free_space>((data.buffer_len*50)/100))
        {
            if (pthread_create(&read_thread, NULL, mp3_read_data,(void*) &free_space) != 0)
                printf("Error, can't create read thread\n");         
        }
    }
}

int openFileFromList(struct list_entry * cur_entry)
{
    if(!cur_entry)
    {        
        printf("no file in list\n");
        return 0;
    }
    if(fd)
        fclose(fd);
    fd=fopen(cur_entry->id3.path,"ro");
    if(fd<0)
    {
        fprintf(stderr,"Can't open file %s\n",cur_entry->id3.path);
        return 0;
    }
    
    fseek(fd,0,SEEK_END);
    file_size = ftell(fd);
    fseek(fd,0,SEEK_SET);
    
    //fprintf(stderr,"%s (s=%d) opened\n",cur_entry->id3.path,file_size);
    
    // fseek(fd,fTag.id3.first_frame_offset,SEEK_SET);
    return 1;
}

#define CHUNK_SIZE 512

void * mp3_read_data(void * arg)
{
    int size,nb_read,pos,read_size; 
    
    size=*(int*)arg;
    threadActive=1;
    
    //fprintf(stderr,"in thread: %d pos:%x/%x (r=%x)\n",size,data.buffer_write,data.buffer_len,data.buffer_read);
    
    if(!fd) /* this should only happen for the first file*/
    {
        if(!openFileFromList(curEntryInList()))
        {
            data.endOfFile=1;
            return NULL;
        }
        
    }
    
    pos=ftell(fd);
    size = MIN(size,(data.buffer_len*50)/100);
    size = MIN(size,data.buffer_len-data.buffer_write);
    size = MIN(size,file_size-pos);
    while(size>0 && !stopThread)
    {
        read_size=MIN(size,CHUNK_SIZE);
        nb_read=fread(data.buffer+data.buffer_write,1,read_size,fd);
        
        if(nb_read<0)
        {
            fprintf(stderr,"ERROR reading file\n");
            threadActive=0;
            return NULL;
        }
        
        data.buffer_write+=nb_read;
        
        size-=nb_read;       
        
    }

    if(stopThread)
    {
        printf("Interupting read\n");
        threadActive=0;
        return NULL;  
    }
    
    if(data.buffer_write>=data.buffer_len)
        data.buffer_write=0;
        
    if(ftell(fd)>=file_size)
    {
        printf("EOF\n");
        /* trying to open nxt file */
        if(!openFileFromList(nxtEntryInList()))
        {
            data.endOfFile=1;
            return NULL;
        }
        
    } 
            
    //fprintf(stderr,"out thread: %d\n",size);
    threadActive=0;
    return NULL;   
}