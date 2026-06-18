/*
* kernel/gui/file_type.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <api.h>

#include <gui/file_type.h>

int is_grv_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".grv") == 0;
}

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

int get_file_type(char * filename)
{
    char *ext;    
    ext = strrchr(filename, '.');
    
    if(ext == 0)
        return BIN_TYPE;
    else if(is_grv_type(ext))
        return BIN_TYPE;
    else if(is_script_type(ext))
        return SCRIPT_TYPE;
    else if(is_image_type(ext))
        return IMG_TYPE;
    else if(is_mp3_type(ext))
        return MP3_TYPE;
    else if(is_text_type(ext))
        return TXT_TYPE;
    else
        return UKN_TYPE;
}

int execBin(char * path, ...)
{
    HTASK hTask;
    API_RUN_GRV (path, &hTask);
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
    int type=get_file_type(filename);
    printf("[handle_type_other]: %s, (type=%d)\n",filename,type);
    
    switch(type)
    {
        case BIN_TYPE:
            launchBin(filename);
            break;
        case SCRIPT_TYPE:
            launchScript(filename);
            break;
        case IMG_TYPE:
            launchViewer(filename);
            break;
        case MP3_TYPE:
            //start_mp3_player(filename);
            //playMp3(filename);
            break;
        case TXT_TYPE:
            launchTxtView(filename);
            break;
        case UKN_TYPE:
        default:
            printf("[handle_type_other]: unknown type\n");
    }
}
