/*
* kernel/gfx/gui/ls_fct.c
*
*   mediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <sys_def/string.h>

#include <kernel/stdfs.h>
#include <kernel/malloc.h>
#include <gui/file_browser.h>

#define LISTSIZE   256

int namesort(char **s1,char **s2)
{
    char * st1=*s1;
    char * st2=*s2;
    while(*st1!=0 && *st2!=0 && toLower(*st1) == toLower(*st2))
    {
        st1++;
        st2++;
    }

    return (toLower(*st1) - toLower(*st2));
}

void cleanList(struct browser_data * bdata)
{
    int i;
    printk("[cleanList] %d entries to be cleaned (%08x)\n",bdata->listused,bdata->list);
    if(bdata->list)
    {
        for (i = 0; i < bdata->listused; i++)
            if(bdata->list[i].name)
                free(bdata->list[i].name);
    
        free(bdata->list);        
    }
    
    bdata->list=NULL;
    bdata->listused=0;
    bdata->listsize=0;
}

int ini_lists(struct browser_data * bdata)
{
   
    if (bdata->listsize == 0) {
        bdata->list = (struct dir_entry *) malloc(LISTSIZE * sizeof(struct dir_entry));
        if (bdata->list == NULL) {
            printk( "No memory for ls buffer\n");
            return 0;
        }
        bdata->listsize = LISTSIZE;
    }
    bdata->listused = 0;
    
    return 1;
}

int qSortEntry(const void * a1,const void * a2)
{
    struct dir_entry * e1=(struct dir_entry *)a1;
    struct dir_entry * e2=(struct dir_entry *)a2;
    if(e1->type != e2->type)
        return e1->type-e2->type;
    return namesort((char**) &e1->name,(char**) &e2->name);
}

int addEntry(struct browser_data * bdata,char * name,int type,int size)
{
    struct dir_entry * newlist;
   
    if (bdata->listused >= bdata->listsize) /* do we need to increase the list size? */
    {
        newlist = (struct dir_entry *) malloc((LISTSIZE+bdata->listsize) * sizeof(struct dir_entry));
        if (!newlist)
        {
            printk( "No memory for ls buffer\n");
            return 0;
        }
        memcpy(newlist, bdata->list, sizeof(struct dir_entry) * bdata->listsize);
        free(bdata->list);
        bdata->list=newlist;
        bdata->listsize += LISTSIZE;
    }
   
    
    bdata->list[bdata->listused].name = strdup(name);
    
    if (bdata->list[bdata->listused].name == NULL)
    {
        printk( "No memory for filenames\n");
        return 0;
    }
        
    bdata->list[bdata->listused].type=type;
    bdata->list[bdata->listused].size=size;
    bdata->list[bdata->listused].selected=0;
    
    /*printf("Entry |%s|%s| added as %s (s=%d)\n",bdata->list[bdata->listused].name,name,
        type==TYPE_FILE?"File":type==TYPE_DIR?"DIR":"BCK",bdata->list[bdata->listused].size);*/
        
    bdata->listused++;
    return 1;
}



int doLs(struct browser_data * bdata)
{
    DIR             *dirp=NULL;
    struct dirent   *dp=NULL;

    char            fullname[PATHLEN];
    
    bdata->totSize=0;
    bdata->nbFile=0;
    bdata->nbDir=0;
       
    if(!ini_lists(bdata))
        return 0;
        
        printk( "[dols] opening %s\n",bdata->path);
        
    dirp = opendir(bdata->path);   
    
    if(!dirp)
    {
        printk( "[dols] error opening dir\n");
        return 0;
    }
        
    while ((dp = readdir(dirp)) != NULL)
    {
       /* printk( "[dols] found |%s|\n",dp->d_name);*/
        if(dp->d_name[0]=='\0')
            continue;            
          
        if ((dp->d_name[0] == '.') && !bdata->show_dot_files)
            continue; 
                       
        fullname[0] = '\0';
        strcat(fullname, dp->d_name);
                       
        if(dp->attribute & ATTR_DIRECTORY)
        {
            if(fullname[0]=='.' && fullname[1]=='\0')
                continue;
            if(fullname[0]=='.' && fullname[1]=='.' && fullname[2]=='\0')
            {                
                if(!addEntry(bdata,"<-Back",TYPE_BACK,0))
                    return 0;
            }
            else
            {
                if(!addEntry(bdata,fullname,TYPE_DIR,0))
                    return 0;
                bdata->nbDir++;
            }
        }
        else
        {

            if(!addEntry(bdata,fullname,TYPE_FILE,dp->size))
                return 0;
            bdata->totSize+=dp->size;
            bdata->nbFile++;
        }
    }
    
    printk( "[dols] folder parsed: find %d files %d folder => %d entires\n",bdata->nbFile,bdata->nbDir,bdata->listused);
       
    closedir(dirp);

    printk("[dols] closed dir\n");
    
    qsort(bdata->list,bdata->listused,sizeof(struct dir_entry),qSortEntry);
    
    return 1;
}

int upDir(struct browser_data * bdata)
{
    char * ptr;
    if(isRoot(bdata))
    {
        printk("[upDir] cant go up, already at root (%s)\n",bdata->path);
        return 0;
    }
    else
    {
        ptr=strrchr(bdata->path,'/');
        if(!ptr)
        {
            printk("[upDir] error can't find a '/' (%s)\n",bdata->path);
            return 0;
        }
        
        if(ptr==bdata->path)
            *(ptr+1)='\0';
        else
            *ptr='\0';
        
        printk("[upDir] %s\n",bdata->path);
        return 1;
    }
    
    
}

int isRoot(struct browser_data * bdata)
{
    return (strlen(bdata->path)==1 && bdata->path[0]=='/');
}

int inDir(struct browser_data * bdata,char * name)
{
    int len=strlen(bdata->path);
    if(len+strlen(name)+2<PATHLEN)
    {
        if(isRoot(bdata))
        {
            printk("[inDir] from root\n");
            strcpy(bdata->path+len,name);
        }
        else
        {
           printk("[inDir] not from root\n");
           bdata->path[len]='/';
           strcpy(bdata->path+len+1,name);
        } 
        printk("[inDir] %s (%s)\n",bdata->path,name);
        return 1;
    }
    else
    {
        printk("[inDir] error cant go to %s from %s, string too long\n",name,bdata->path);
        return 0;
    }
}

void clearSelection(struct browser_data * bdata)
{
    int i;
    for(i=0;i<bdata->listused;i++)
        bdata->list[i].selected=0;
}

void chgSelect(struct browser_data *bdata,int num)
{
    if(bdata->list[num].type != TYPE_BACK)
    {
        bdata->list[num].selected = ~bdata->list[num].selected;
        /* see if e need to redraw it */
        if(num >= bdata->pos && num < bdata->pos+bdata->nb_disp_entry)
            printName(&bdata->list[num],num-bdata->pos,1,num-bdata->pos==bdata->nselect,bdata);
    }
}

int nbSelected(struct browser_data * bdata)
{
    int i,nb=0;
    for(i=0;i<bdata->listused;i++)
        nb+=bdata->list[i].selected;
    return nb;
}

struct dir_entry * nxtSelect(struct browser_data * bdata,int * pos)
{
    int i;
    if(*pos<0 || *pos>bdata->listused)
        return NULL;
    for(i=*pos;i<bdata->listused;i++)
    {
        if(bdata->list[i].selected)
        {
            *pos=i+1;
            return &bdata->list[i];
        }
    }
    return NULL;
}
