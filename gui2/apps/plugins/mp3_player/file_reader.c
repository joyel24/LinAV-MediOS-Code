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

#define MIN(a, b) (((a)<(b))?(a):(b))

void * mp3_read_more(void * arg)
{
    int size,nb_read,pos,free_space; 
    
    stopThread=0;
    
    while(!data.endOfFile && !stopThread)
    {
        free_space=data.buffer_read-data.buffer_write;
        if(free_space<=0)
            free_space+=data.buffer_len;
        if(free_space>((data.buffer_len*50)/100))
        {
            size=free_space;
        
            //printf("freespace:%d => size=%d\n",free_space,size);
            pos=ftell(fd);
            //printf("File pos:%x size asked:%d\n",ftell(fd),size);
            size = MIN(size,data.buffer_len-data.buffer_write);
            size = MIN(size,file_size-pos);
            
            //printf("trying to read: %d\n",size);
    
            nb_read=fread(data.buffer+data.buffer_write,1,size,fd);
            
            //printf("read: %d\n",nb_read);
            
            if(nb_read<0)
            {
                fprintf(stderr,"ERROR reading file\n");
                return NULL;
            }
            
            data.buffer_write+=nb_read;
            if(data.buffer_write>=data.buffer_len)
                data.buffer_write=0;
               
            if(ftell(fd)>=file_size)
            {
                data.endOfFile=1;
                printf("EOF\n");
            }                       
        }       
        
    } 
    return NULL;   
}