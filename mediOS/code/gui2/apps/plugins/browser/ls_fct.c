/*
* ls_fct.c
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

#include "browser.h"
#include "avstring.h"
#include "graphics.h"
#include "colordef.h"
#include "cops.h"

#define LISTSIZE   256

/*struct dir_entry * list;
int                listused=0;
int                listsize=0;

int nbFile=0,nbDir=0,totSize=0;*/

extern struct client_operations * cops;

#define    toLower(chr)  ((chr>64 && chr<91)?chr+32:chr)

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
    printf("[cleanList] %d entries to be cleaned\n",bdata->listused);
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
            fprintf(stderr, "No memory for ls buffer\n");
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
            fprintf(stderr, "No memory for ls buffer\n");
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
        fprintf(stderr, "No memory for filenames\n");
        return 0;
    }
    bdata->list[bdata->listused].type=type;
    bdata->list[bdata->listused].size=size;
    bdata->list[bdata->listused].selected=0;
    
    bdata->listused++;
    return 1;
}



int doLs(struct browser_data * bdata,char * name)
{
    DIR             *dirp=NULL;
    struct dirent   *dp=NULL;
    struct stat     statbuf;
    char            fullname[PATHLEN];
    char            tmpP[4];
    int             isRoot=0;
    
    bdata->totSize=0;
    bdata->nbFile=0;
    bdata->nbDir=0;
    
    fprintf(stderr, "[dols] entering\n");
    
    if(!ini_lists(bdata))
        return 0;

    dirp = opendir(name);
    if(!dirp)
    {
        fprintf(stderr, "[dols] error\n");
        perror(name);
        return 0;
    }
    
    getcwd(tmpP, 4);
    if(tmpP[0]=='/' && tmpP[1]=='\0')
        isRoot=1;
    
    fprintf(stderr, "[dols] parsing folder (%x)\n",(int)dirp);
    
    while ((dp = readdir(dirp)) != NULL)
    {
        fprintf(stderr, "[dols] found %s ",dp->d_name);
        if(dp->d_name[0]=='\0')
            continue;            
          
        if ((dp->d_name[0] == '.') && !bdata->show_dot_files)
            continue; 
                       
        fullname[0] = '\0';
        strcat(fullname, dp->d_name);
        
        fprintf(stderr, "-in fullname ");
               
        if (stat(dp->d_name, &statbuf) < 0)
        {
            fprintf(stderr, "[dols] error in stat\n");
            //perror(dp->d_name);
            continue;
        }
        
        fprintf(stderr, "-stat ok ");
        
        if(S_ISDIR(statbuf.st_mode))
        {
            fprintf(stderr, "-addig as dir ");
            if(fullname[0]=='.' && fullname[1]=='\0')
                continue;
            if(fullname[0]=='.' && fullname[1]=='.' && fullname[2]=='\0')
            {
                if(!isRoot)
                    if(!addEntry(bdata,"<-Back",TYPE_BACK,0))
                        return 0;
            }
            else
            {
                if(!addEntry(bdata,fullname,TYPE_DIR,0))
                    return 0;
                bdata->nbDir++;
            }
            fprintf(stderr, "-done\n");
        }
        else
        {
            fprintf(stderr, "-addig as file ");
            if(!addEntry(bdata,fullname,TYPE_FILE,statbuf.st_size))
                return 0;
            bdata->totSize+=statbuf.st_size;
            bdata->nbFile++;
            fprintf(stderr, "-done\n");
        }
    }
    
    fprintf(stderr, "[dols] folder parsed: find %d files %d folder => %d entires\n",bdata->nbFile,bdata->nbDir,bdata->listused);
    
    fprintf(stderr, "[dols] closing dir %x\n",(int)dirp);
    
    closedir(dirp);
    
    fprintf(stderr, "[dols] sorting entries\n");
    
    qsort(bdata->list,bdata->listused,sizeof(struct dir_entry),qSortEntry);
   
    fprintf(stderr, "[dols] exiting\n");
    
    return 1;
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
