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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "ls_main.h"


#define MAXargs 31

/*extern variables */
extern struct client_operations * cops;
/************************/

/******************************************************************************* test file types */

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

/***********************************************************************************************/

/****************************************************************************** file launcher */

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
            return 0;
        }
    }
    return 1;
}

int launchBin(char * name)
{
    return execBin(name, name, (char*)0);
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
    return execBin("/bin/sh", "sh", name, (char*)0);
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

/***********************************************************************************************/

/************************************************************************** file basic command */

int do_mv(char * src,char * dest)
{
    if (access(src, F_OK) < 0) /* check if src file exists */
    {
        perror(src);
        return 0;
    }             
       
    if(isadir(dest))                /* if dest is just a folder => build the complete name */
        dest=buildname(dest,src);  
        
    if(rename(src,dest)>= 0)        /* try to move using rename */
        return 1;    
        
    /* rename failed*/        
    if (errno != EXDEV)  /*is it because dev are different*/
    {
        perror(dest); /* no => error */
        return 0;
    }    
    
    /* dev are different => copy + remove */    
    if (!copyfile(src, dest, 1))    /* try to copy */
        return 0;   
                        /* it failed */
    if (unlink(src) < 0)        /* now we can remove src */
    {
        perror(src);
        return 0;
    }    
    
    return 1;
}

int do_cp(char * src,char * dest)
{
    if (access(src, F_OK) < 0) /* check if src file exists */
    {
        perror(src);
        return 0;
    }  
                  
    if(isadir(dest))                /* if dest is just a folder => build the complete name */
        dest=buildname(dest,src);
        
    if (!copyfile(src, dest, 0))    /* try to copy */
        return 0;    
        
    return 1;    
}

int do_mount(char * devFile,char * dirMnt)
{
   int pid;
   int status;
   
   pid = vfork();
   if (pid == 0)
   {
       execl("/bin/mount", "mount",devFile,dirMnt,(char*)NULL);
       fprintf(stderr, "mount failed %s on %s\n",devFile,dirMnt);
       _exit(1);       
   }
   else
   {
       if(pid>0)
           waitpid(pid, &status, 0);
       else
       {
           fprintf(stderr, "vfork failed for mount %s on %s\n",devFile,dirMnt);
           return 0;
       }
   }
   return 1;
}

int isadir(char *name)
{
    struct stat statbuf;
    
    if (stat(name, &statbuf) < 0)
            return 0;
            
    return S_ISDIR(statbuf.st_mode);
}

char * buildname(char * dirname,char * filename)
{
    char  *cp;
    static char buf[PATHLEN]; 
       
    if ((dirname == NULL) || (*dirname == '\0'))
        return filename;    
        
    cp = strrchr(filename, '/');
    if (cp)
        filename = cp + 1; 
           
    strcpy(buf, dirname);
    strcat(buf, "/");
    strcat(buf, filename);    
    return buf;
}

int copyfile(char * src,char * dest,int setmodes)
{
    int  fdSrc;
    int  fdDest;
    int  ccSrc;
    int  ccDest;
    
    char * writeBuf;
    char * readBuf;
    
    struct stat statSrc;
    struct stat statDest;
    
    struct utimbuf times;
    
    int len = 8192-16;

    
    if (stat(src, &statSrc) < 0)
    {
        perror(src);
        return 0;
    }
    
    if (stat(dest, &statDest) < 0)
    {
        statDest.st_ino = -1;
        statDest.st_dev = -1;
    }
    
    if (S_ISREG(statSrc.st_mode) && (statSrc.st_dev == statDest.st_dev) && (statSrc.st_ino == statDest.st_ino))
    {
        fprintf(stderr, "Copying file \"%s\" to itself\n", src);
        return 0;
    }
        
    fdSrc = open(src, 0);
    if (fdSrc < 0)
    {
        perror(src);
        return 0;
    } 
       
    fdDest = open(dest, O_WRONLY|O_CREAT|O_TRUNC, statSrc.st_mode);
    if (fdDest < 0)
    {
        perror(dest);
        close(fdSrc);
        return 0;
    }
       
    readBuf = malloc(len);
    if (!readBuf) 
    {
        fprintf(stderr,"Unable to allocate buffer of %d bytes\n", len);
        return 0;
    }
        
    while ((ccSrc = read(fdSrc, readBuf, len)) > 0)
    {
        writeBuf = readBuf;        
        while (ccSrc > 0)
        {
            ccDest = write(fdDest, writeBuf, ccSrc);
            if (ccDest < 0)
            {
                perror(dest);
                free(readBuf);
                goto error_exit;
            }
            writeBuf += ccDest;
            ccSrc -= ccDest;
        }
    } 
       
    free(readBuf);
        
    if (ccSrc < 0)
    {
        perror(src);
        goto error_exit;
    } 
       
    close(fdSrc);    
    if (close(fdDest) < 0)
    {
        perror(dest);
        return 0;
    }    
    
    if (setmodes)
    {
        chmod(dest, statSrc.st_mode);    
        chown(dest, statSrc.st_uid, statSrc.st_gid);    
        times.actime = statSrc.st_atime;
        times.modtime = statSrc.st_mtime;    
        utime(dest, &times);
    }    
    
    return 1;   
    
error_exit:
    close(fdSrc);
    close(fdDest);    
    return 0;
}

/***********************************************************************************************/
