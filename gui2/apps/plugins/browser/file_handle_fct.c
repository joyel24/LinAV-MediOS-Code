/*
* file_handle_fct.c
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
#include <stdarg.h>
#include <string.h>

#include "ls_main.h"


#define MAXargs 31

/*extern variables */
extern struct client_operations * cops;
/************************/

int is_script_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".sh") == 0;
}

int is_image_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".jpg") == 0
        || strcmp(extension, ".gif") == 0
        || strcmp(extension, ".bmp") == 0;
}

int is_mp3_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".mp3") == 0;
}

int is_text_type(char * extension)
{
   int retVal = 0;

    strlwr(extension);
   if(strcmp(extension, ".txt") == 0)
       retVal = 1;
   else if(strcmp(extension, ".cfg") == 0)
       retVal = 1;
   else if(strcmp(extension, ".c") == 0)
       retVal = 1;
   else if(strcmp(extension, ".cpp") == 0)
       retVal = 1;
   else if(strcmp(extension, ".h") == 0)
       retVal = 1;
   else if(strcmp(extension, ".ini") == 0)
       retVal = 1;
   else if(strcmp(extension, ".csv") == 0)
       retVal = 1;

    return retVal;
}

int simpleLaunch(char * name)
{
   int pid;
   int status;
   
   pid = vfork();
   if (pid == 0)
   {
       execl("/bin/mount", "mount","/dev/avcf1","/cf",(char*)NULL);
       fprintf(stderr, "exec failed! %s\n",name);
       _exit(1);       
   }
   else
   {
       if(pid>0)
           waitpid(pid, &status, 0);
       else
       {
           fprintf(stderr, "vfork failed %s\n", name);
           return 0;
       }
   }
   return 1;
}

//execBin(path,arg0,arg1,arg2,...,(char*)0) arg0 should be the name of the app
int execBin(char * path, ...)
{
    int pid;
    va_list ap;
    char *array [MAXargs];
    int argno=0;

    cops->clearEventQueue();
    pid = vfork();
    if (pid == 0)
    {
        va_start (ap, path);
        while ((array[argno++] = va_arg(ap, char*)) != (char*)0) /* NOTHING */ ;
        va_end(ap);
        execv(path, array);

        fprintf(stderr, "exec failed!\n");
        _exit(1);
    }
    else
    {
        if (pid > 0)
        {
            int status;
            cops->closeScreen();
            waitpid(pid, &status, 0);
            cops->openScreen();
        }
        else
        {
            fprintf(stderr, "vfork failed %d\n", pid);
        }
    }
}

int launchBin(char * name)
{
    execBin(name, name, (char*)0);
}

void launchSoundPlayer(char *name)
{
    char vol[5];
    char rep[5];
    sprintf(vol, "%d",70);
    sprintf(rep, "%d",0);

    execBin("/mnt/avwm/apps/play", "play", name, vol, rep, (char*)0);
}

void launchViewer(char *name)
{
    execBin("/mnt/avwm/apps/viewer", "viewer", name, (char*)0);
}

void launchTxtView(char *name)
{
    execBin("/mnt/avwm/apps/txtviewer", "txtviewer", name, (char*)0);
}

int launchScript(char * name)
{
    execBin("/bin/sh", "sh", name, (char*)0);
}

void handle_type_other(char *filename)
{
    char *ext;
    
    ext = strrchr(filename, '.');
    
    printf("[handle_type_other]: %s, %s\n",filename,ext);
    if (ext == 0)
    {
      // no extension
        launchBin(filename);
    }
    else if (is_image_type(ext))
    {
       launchViewer(filename);
    }
    else if (is_text_type(ext))
    {
       launchTxtView(filename);
    }
    else if (is_mp3_type(ext))
    {
       //launchSoundPlayer(filename);
       cops->playMp3(filename);
    }
    else if (is_script_type(ext))
    {
       launchScript(filename);
    }
    else
    {
       // unknown type
    }
}
