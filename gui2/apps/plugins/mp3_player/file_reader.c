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

/*extern variables */
extern FILE * fd;
extern struct mp3_play data;
extern int file_size;
extern int stopThread;
extern pthread_t read_thread;

#define MIN(a, b) (((a)<(b))?(a):(b))

int threadActive=0;

void mp3_read_more(void)
{
    int free_space;
    fprintf(stderr,"r=%x w=%x dif=%x\n",data.buffer_read,data.buffer_write,data.buffer_write-data.buffer_read);
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

#define CHUNK_SIZE 512

void * mp3_read_data(void * arg)
{
    int size,nb_read,pos,read_size; 
    
    size=*(int*)arg;
    threadActive=1;
    
    fprintf(stderr,"in thread: %d pos:%x/%x (r=%x)\n",size,data.buffer_write,data.buffer_len,data.buffer_read);
    
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
        printf("Interupting read\n");
    
    if(data.buffer_write>=data.buffer_len)
        data.buffer_write=0;
        
    if(ftell(fd)>=file_size)
    {
        data.endOfFile=1;
        printf("EOF\n");
    } 
            
    fprintf(stderr,"out thread: %d\n",size);
    threadActive=0;
    return NULL;   
}