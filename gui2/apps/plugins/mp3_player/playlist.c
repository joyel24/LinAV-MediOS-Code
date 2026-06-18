/*
* playlist.c
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
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include "cops.h"
#include "mp3_player.h"

struct list_entry * head=NULL; // first entry
struct list_entry * tail=NULL; // nxt empty entry
struct list_entry * cur=NULL; // current entry

extern bool v1first;

int addPlaylist(char * filename)
{
    struct list_entry * ptr;
    
    ptr=(struct list_entry*)malloc(sizeof(struct list_entry));
    if(!ptr)
    {
        fprintf(stderr,"Can't malloc new entry\n");
        return 0;    
    }
    
    fprintf(stderr,"%s\n",filename);
    
    if(mp3info(&(ptr->id3), filename, v1first))
    {
        fprintf(stderr,"Bad mp3 %s\n",filename);
        free(ptr);
        return 0;
    }
    else
    {
        fprintf(stderr,"\n\n");
        /*new mp3 ok add it at the end */
        ptr->prev=tail;
        ptr->nxt=NULL;
        if(tail)
            tail->nxt=ptr;
        tail=ptr;
        if(!head)
            cur=head=ptr;
        return 1;
    }        
}

struct list_entry * curEntryInList(void)
{
    return cur;
}

struct list_entry * nxtEntryInList(void)
{
    if(cur)
    {
        if(cur->nxt)
            cur=cur->nxt;
        else
            return NULL;
    }
    return cur;
}

struct list_entry * prevEntryInList(void)
{
    if(cur)
    {
        if(cur->prev)
            cur=cur->prev;
        else
            return NULL;
    }
    return cur;
}

int nbEntryInList(void)
{
    struct list_entry * ptr;
    int nb=0;
    for(ptr=head;ptr!=NULL;ptr=ptr->nxt)
        nb++;    
    return nb;
}

void iniPlaylist(void)
{    
    cur=head=tail=NULL;
}

void cleanPlaylist(void)
{
    struct list_entry * ptr,*ptr2;
    ptr=head;
    while(ptr)
    {
        ptr2=ptr;
        ptr=ptr->nxt;
        free(ptr2);
    }
    cur=NULL;
    tail=head=cur=NULL;          
}

int addDir(char * dirName)
{
    DIR             *dirp;
    struct dirent   *dp;
    struct stat     statbuf;
    char            *ext;
    int             nb=0;

    dirp = opendir(dirName);
    if(!dirp)
    {
        fprintf(stderr, "[addDir] error, can't open dir %s\n",dirName);
        return 0;
    }
    
    while ((dp = readdir(dirp)))
    {
        if (stat(dp->d_name, &statbuf) < 0)
        {
            fprintf(stderr, "[addDir] error in stat\n");
            continue;
        }
        
        if(!S_ISDIR(statbuf.st_mode))
        {
            /* check extension now */                
            ext = strrchr(dp->d_name, '.');
            if(ext!=0      && ext[0]=='.'
                           && (ext[1]=='m'||ext[1]=='M')
                           && (ext[2]=='p'||ext[2]=='P')
                           &&  ext[3]=='3')
                if(addPlaylist(dp->d_name))
                    nb++;
        }
    }
    
    return nb;
}

void showList(void)
{
    struct list_entry * ptr;
    int i=0;
    for(ptr=head;ptr!=NULL;ptr=ptr->nxt)
    {
        printf("%d: |%s|%s\n",i,ptr->id3.path,cur==ptr?"  --->cur":"");
        i++;
    }
}









