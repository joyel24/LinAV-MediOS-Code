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

char            **dir_list;
int             dir_listsize;
int             dir_listused;

char            **file_list;
int             file_listsize;
int             file_listused;

struct dir_entry * list;
int                listused;

int nbFile=0,nbDir=0,totSize=0;

void cleanList(void)
{
    int i;
    for (i = 0; i < listused; i++)
    {
        free(list[i].name);
        //free(&list[i]);
    }
    free(list);
    listused=0;
}

int ini_lists(void)
{
    if (dir_listsize == 0) {
        dir_list = (char **) malloc(LISTSIZE * sizeof(char *));
        if (dir_list == NULL) {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        dir_listsize = LISTSIZE;
    }
    dir_listused = 0;

    if (file_listsize == 0) {
        file_list = (char **) malloc(LISTSIZE * sizeof(char *));
        if (file_list == NULL) {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        file_listsize = LISTSIZE;
    }
    file_listused = 0;
    return 0;
}

int add_dir(char * name)
{
    char **newlist;
    
    if (dir_listused >= dir_listsize)
    {
        newlist = (char **)malloc((sizeof(char **)) * (dir_listsize + LISTSIZE));
        if (newlist == NULL)
        {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        memcpy(newlist, dir_list, sizeof(char**) * dir_listsize);
        free(dir_list);
        dir_list=newlist;
        dir_listsize += LISTSIZE;
    }

    dir_list[dir_listused] = strdup(name);
    if (dir_list[dir_listused] == NULL) {
        fprintf(stderr, "No memory for filenames\n");
        return -1;
    }
    dir_listused++;
    return 0;

}

int add_file(char * name)
{
    char            **newlist;

    if (file_listused >= file_listsize)
    {
        newlist = (char **)malloc((sizeof(char **)) * (file_listsize + LISTSIZE));
        if (newlist == NULL)
        {
            fprintf(stderr, "No memory for ls buffer\n");
            return -1;
        }
        memcpy(newlist, file_list, sizeof(char**) * file_listsize);
        free(file_list);
        file_list=newlist;
        file_listsize += LISTSIZE;
    }

    file_list[file_listused] = strdup(name);
    if (file_list[file_listused] == NULL) {
        fprintf(stderr, "No memory for filenames\n");
        return -1;
    }
    file_listused++;
    return 0;

}

int doLs(char * name)
{
    DIR             *dirp;
    struct dirent   *dp;
    struct stat     statbuf;
    char            fullname[PATHLEN];
    int             endslash;
    int             i;
    
    totSize=0;
    
    endslash = (*name && (name[strlen(name) - 1] == '/'));

    if(ini_lists()<0)
        return -1;

    dirp = opendir(name);
    if (dirp == NULL) {
        fprintf(stderr, "[dols] error\n");
        perror(name);
        return -1;
    }
    
    i=0;

    while ((dp = readdir(dirp)) != NULL)
    {
        /*if(dp->d_name[0]='\0')
            continue;*/
        /*int pos=0;*/
        
        /*if(dp->d_name[0] == '.' && dp->d_name[1] =='/')
            pos+=2;*/
            
        if ((dp->d_name[0] == '.') && !SHOW_ALL)
            continue;
         

        fullname[0] = '\0';

        /*if ((*name != '.') || (name != '\0'))
        {
            strcpy(fullname, name);
            if (!endslash)
                strcat(fullname, "/");
        }*/

        strcat(fullname, dp->d_name);

        if (stat(dp->d_name, &statbuf) < 0)
        {
            fprintf(stderr, "[dols] error in stat\n");
            perror(dp->d_name);
            return -1;
        }

        if(S_ISDIR(statbuf.st_mode))
        {
            if(add_dir(fullname)<0)
                return -1;
        }
        else
        {
            totSize+=statbuf.st_size;
            if(add_file(fullname)<0)
                return -1;
        }
        i++;
    }

    closedir(dirp);
    
    qsort((char *) dir_list, dir_listused, sizeof(char *), qNameSort);
    qsort((char *) file_list, file_listused, sizeof(char *), qNameSort);

    listused=dir_listused+file_listused;
    
    nbFile=file_listused;
    nbDir=dir_listused-2; /* removing the too dummy folders: . & .. */

    list=(struct dir_entry*)malloc((sizeof(struct dir_entry))*listused);

    if (list == NULL)
    {
        fprintf(stderr, "No memory for ls buffer\n");
        return -1;
    }

    for(i=0;i<dir_listused;i++)
    {
        list[i].name=dir_list[i];
        list[i].type=TYPE_DIR;
    }

    for(i=0;i<file_listused;i++)
    {
        list[dir_listused+i].name=file_list[i];
        list[dir_listused+i].type=TYPE_FILE;
    }

    free(dir_list);
    dir_listsize=0;
    free(file_list);
    file_listsize=0;   
    
    //printf("ls finished: %d files, %d folders tot size=%d\n",nbFile,nbDir,totSize);
    
    return 0;
}
