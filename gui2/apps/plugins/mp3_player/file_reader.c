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

#include "cops.h"
#include "mp3_player.h"

/*extern variables */
extern FILE * fd;
extern struct mp3_play data;

#define CHUNK_SIZE  1020

int  mp3_need_more(void)
{
    int size;
    if(!data.endOfFile)
    {
        size=data.buffer_read-data.buffer_write;
        if(size<=0)
            size+=data.buffer_len;
        if(size>((data.buffer_len*50)/100))
            return size;
    }
    return 0;
}

int read_from_HD(int size)
{
    int nb=0;
    int tot=0;
    while(size>CHUNK_SIZE)
    {
        nb=fread(data.buffer+data.buffer_write,1,CHUNK_SIZE,fd);
        tot+=nb;
        if(nb<0)
        {
            fprintf(stderr,"ERROR reading file\n");
            return -1;
        }
        data.buffer_write+=nb;
        if(nb<CHUNK_SIZE)
        {
            size=0;
            break;
        }
        size-=nb ;       
    }
    
    if(size>0)
    {
        nb+=fread(data.buffer+data.buffer_write,1,CHUNK_SIZE,fd); 
        tot+=nb;
        if(nb<0)
        {
            fprintf(stderr,"ERROR reading file\n");
            return -1;
        }
        data.buffer_write+=nb;        
    }
    return tot;
}
void mp3_read_more(int size)
{
    int nb_read;
    int part1_size;
    
    /*if(size==0)
    {
        size=data.buffer_read-data.buffer_write;
        if(size<=0)
            size+=data.buffer_len;
    }*/
    
    printf("File pos:%x\n",ftell(fd));
    
    if(!data.endOfFile)
    {
        printf("size=%d\n",size);
        if(size>=(data.buffer_len-data.buffer_write))
        {
            part1_size=data.buffer_len-data.buffer_write;
            size-=part1_size;
        }
        else
        {
            part1_size=size;
            size=0;
        }
        
        printf("trying to read: %d left:%d\n",part1_size,size);
        
        nb_read=read_from_HD(part1_size);
        //nb_read=fread(data.buffer+data.buffer_write,1,part1_size,fd);
        
        printf("read: %d\n",nb_read);
        
        if(nb_read<0)
        {
            fprintf(stderr,"ERROR reading file\n");
            return;
        }
        //data.buffer_write+=nb_read;        
        if(nb_read<part1_size)
        {
            data.endOfFile=1;
            return; /* goto out*/
        }
        if(size>0)
        {
            data.buffer_write=0;
            nb_read=read_from_HD(size);
            //nb_read=fread(data.buffer+data.buffer_write,1,size,fd);
            if(nb_read<0)
            {
                fprintf(stderr,"ERROR reading file\n");
                return;
            }
            //data.buffer_write+=nb_read;       
            if(nb_read<size)
            {
                data.endOfFile=1;                
            }           
        }
    }
}