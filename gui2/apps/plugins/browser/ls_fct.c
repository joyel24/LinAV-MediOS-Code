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

#include "ls_main.h"

#define LISTSIZE   256

#define SHOW_ALL   1

struct dir_entry * list;
int                listused=0;
int                listsize=0;

int nbFile=0,nbDir=0,totSize=0;

void cleanList(void)
{
    int i;
    for (i = 0; i < listused; i++)
        free(list[i].name);
    free(list);
    listused=0;
    listsize=0;
}

int ini_lists(void)
{
   
    if (listsize == 0) {
        list = (struct dir_entry *) malloc(LISTSIZE * sizeof(struct dir_entry));
        if (list == NULL) {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        listsize = LISTSIZE;
    }
    listused = 0;
    
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

int addEntry(char * name,int type,int size)
{
    struct dir_entry * newlist;
    
    if (listused >= listsize) /* do we need to increase the list size? */
    {
        newlist = (struct dir_entry *) malloc((LISTSIZE+listsize) * sizeof(struct dir_entry));
        if (!newlist)
        {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        memcpy(newlist, list, sizeof(struct dir_entry) * listsize);
        free(list);
        list=newlist;
        listsize += LISTSIZE;
    }
    list[listused].name = strdup(name);
    
    if (list[listused].name == NULL)
    {
        fprintf(stderr, "No memory for filenames\n");
        return -1;
    }
    list[listused].type=type;
    list[listused].size=size;
    
    listused++;
    return 0;
}

int doLs(char * name)
{
    DIR             *dirp;
    struct dirent   *dp;
    struct stat     statbuf;
    char            fullname[PATHLEN];
    
    totSize=0;
    nbFile=0;nbDir=0;
    
    if(ini_lists()<0)
        return -1;

    dirp = opendir(name);
    if (dirp == NULL) {
        fprintf(stderr, "[dols] error\n");
        perror(name);
        return -1;
    }
    

    while ((dp = readdir(dirp)) != NULL)
    {
        if(dp->d_name[0]=='\0')
            continue;            
          
        if ((dp->d_name[0] == '.') && !SHOW_ALL)
            continue;            

        fullname[0] = '\0';
        strcat(fullname, dp->d_name);
        
        if (stat(dp->d_name, &statbuf) < 0)
        {
            fprintf(stderr, "[dols] error in stat\n");
            //perror(dp->d_name);
            continue;
        }

        if(S_ISDIR(statbuf.st_mode))
        {
            if(addEntry(fullname,TYPE_DIR,0)<0)
                return -1;
            nbDir++;
        }
        else
        {
            if(addEntry(fullname,TYPE_FILE,statbuf.st_size)<0)
                return -1;
            totSize+=statbuf.st_size;
            nbFile++;
        }
    }

    closedir(dirp);
    
    qsort(list,listused,sizeof(struct dir_entry),qSortEntry);
    
   /* removing the too dummy folders: . & .. */
    nbDir-=2;
 
    return 0;
}
