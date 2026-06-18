/* 
*   kernel/gfx/gui/mp3Player/playlist.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/stdfs.h>

#include <sys_def/string.h>

#include <kernel/playList.h>

struct list_entry * head=NULL; // first entry
struct list_entry * tail=NULL; // nxt empty entry
struct list_entry * cur=NULL; // current entry

extern int v1first;

int addPlaylist(char * filename)
{
    struct list_entry * ptr;
    
    ptr=(struct list_entry*)malloc(sizeof(struct list_entry));
    if(!ptr)
    {
        printk("Can't malloc new entry\n");
        return 0;    
    }
    
    printk("%s\n",filename);
    
    if(mp3info(&(ptr->id3), filename, v1first))
    {
        printk("Bad mp3 %s\n",filename);
        free(ptr);
        return 0;
    }
    else
    {
        printk("\n\n");
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
    char            *ext;
    int             nb=0;

    char file_path[256];
    
    dirp = opendir(dirName);
    if(!dirp)
    {
        printk( "[addDir] error, can't open dir %s\n",dirName);
        return 0;
    }
    
    printk("Searching MP3 in folder\n");
    
    while ((dp = readdir(dirp)))
    {
        if(dp->type!=VFS_TYPE_DIR)
        {
            /* check extension now */                
            ext = strrchr(dp->d_name, '.');
            if(ext!=0      && ext[0]=='.'
                           && (ext[1]=='m'||ext[1]=='M')
                           && (ext[2]=='p'||ext[2]=='P')
                           &&  ext[3]=='3')
            {
                sprintf(file_path,"/%s/%s",dirName,dp->d_name);
                if(addPlaylist(file_path))
                    nb++;
            }
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









