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

#define LISTSIZE   256

/*struct dir_entry * list;
int                listused=0;
int                listsize=0;

int nbFile=0,nbDir=0,totSize=0;*/

void cleanList(struct browser_data * bdata)
{
    int i;
    printf("[cleanList] %d entries\n",bdata->listused);
    for (i = 0; i < bdata->listused; i++)
        free(bdata->list[i].name);
    free(bdata->list);
    bdata->listused=0;
    bdata->listsize=0;
}

int ini_lists(struct browser_data * bdata)
{
   
    if (bdata->listsize == 0) {
        bdata->list = (struct dir_entry *) malloc(LISTSIZE * sizeof(struct dir_entry));
        if (bdata->list == NULL) {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        bdata->listsize = LISTSIZE;
    }
    bdata->listused = 0;
    
    return 0;
}

int qSortEntry(const void * a1,const void * a2)
{
    struct dir_entry * e1=(struct dir_entry *)a1;
    struct dir_entry * e2=(struct dir_entry *)a2;
    if(e1->type != e2->type)
        return e1->type-e2->type;
    return namesort((char**) &e1->name,(char**) &e2->name);
}

#define DEBUG_DO_LS

int addEntry(struct browser_data * bdata,char * name,int type,int size)
{
    struct dir_entry * newlist;

#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] in");
#endif    
    if (bdata->listused >= bdata->listsize) /* do we need to increase the list size? */
    {
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] need more");
#endif 
        newlist = (struct dir_entry *) malloc((LISTSIZE+bdata->listsize) * sizeof(struct dir_entry));
        if (!newlist)
        {
            fprintf(stderr, "No memory for ls buffer\n");
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "No memory for ls buffer");
#endif
            return -1;
        }
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] malloc done");
#endif 
        memcpy(newlist, bdata->list, sizeof(struct dir_entry) * bdata->listsize);
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] memcopy done");
#endif 
        free(bdata->list);
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] free done");
#endif 
        bdata->list=newlist;
        bdata->listsize += LISTSIZE;
    }
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] real add");
#endif     
    bdata->list[bdata->listused].name = strdup(name);
    
    if (bdata->list[bdata->listused].name == NULL)
    {
        fprintf(stderr, "No memory for filenames\n");
#ifdef DEBUG_DO_LS
        fillRect(COLOR_WHITE,2, 220,316,10);
        putS(COLOR_BLUE, COLOR_WHITE,2, 220, "No memory for filenames");
#endif
        return -1;
    }
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[addEntry] name copy");
#endif 
    bdata->list[bdata->listused].type=type;
    bdata->list[bdata->listused].size=size;
    bdata->list[bdata->listused].selected=0;
    
    bdata->listused++;
    return 0;
}



int doLs(struct browser_data * bdata,char * name)
{
    DIR             *dirp;
    struct dirent   *dp;
    struct stat     statbuf;
    char            fullname[PATHLEN];
    char            tmpP[4];
    int             isRoot=0;
    
    bdata->totSize=0;
    bdata->nbFile=0;
    bdata->nbDir=0;
    
    if(ini_lists(bdata)<0)
        return -1;

    dirp = opendir(name);
    if(!dirp)
    {
        fprintf(stderr, "[dols] error\n");
#ifdef DEBUG_DO_LS
        fillRect(COLOR_WHITE,2, 220,316,10);
        putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] error");
#endif
        perror(name);
        return -1;
    }
    
    getcwd(tmpP, 4);
    if(tmpP[0]=='/' && tmpP[1]=='\0')
        isRoot=1;
    

    while ((dp = readdir(dirp)) != NULL)
    {
        if(dp->d_name[0]=='\0')
            continue;            
          
        if ((dp->d_name[0] == '.') && !bdata->show_dot_files)
            continue; 
                       
#ifdef DEBUG_DO_LS
        fillRect(COLOR_WHITE,2, 230,316,10);
        putS(COLOR_BLUE, COLOR_WHITE,2, 230, dp->d_name);
#endif
            
        fullname[0] = '\0';
        strcat(fullname, dp->d_name);

#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] strcat");
#endif        
                
        if (stat(dp->d_name, &statbuf) < 0)
        {
            fprintf(stderr, "[dols] error in stat\n");
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] error in stat");
#endif
            //perror(dp->d_name);
            continue;
        }
        
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] stat");
#endif                

        if(S_ISDIR(statbuf.st_mode))
        {
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] is_dir");
#endif  
            if(fullname[0]=='.' && fullname[1]=='\0')
                continue;
            if(fullname[0]=='.' && fullname[1]=='.' && fullname[2]=='\0')
            {
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] .. entry");
#endif
                if(!isRoot)
                    if(addEntry(bdata,"<-Back",TYPE_BACK,0)<0)
                        return -1;
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] .. added");
#endif                        
            }
            else
            {
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] norm entry");
#endif    
                if(addEntry(bdata,fullname,TYPE_DIR,0)<0)
                    return -1;
                bdata->nbDir++;
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] norm entry added");
#endif
            }
        }
        else
        {
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] is_file");
#endif         
            if(addEntry(bdata,fullname,TYPE_FILE,statbuf.st_size)<0)
                return -1;
            bdata->totSize+=statbuf.st_size;
            bdata->nbFile++;
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] file added");
#endif
        }
    }
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] out add");
#endif


    closedir(dirp);

    
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] dir closed");
#endif    


    qsort(bdata->list,bdata->listused,sizeof(struct dir_entry),qSortEntry);
   
    
#ifdef DEBUG_DO_LS
            fillRect(COLOR_WHITE,2, 220,316,10);
            putS(COLOR_BLUE, COLOR_WHITE,2, 220, "[dols] qsort done");
#endif    
   return 0;
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
