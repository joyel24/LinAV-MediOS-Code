/*
* kernel/gui/file_type.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>


#include <file_type.h>

int is_med_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".med") == 0;
}

int is_gb_type(char *extension)
{
    strlwr(extension);
    return strcmp(extension, ".gb") == 0;
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
    else if(is_med_type(ext))
        return MED_TYPE;
    else if(is_image_type(ext))
        return IMG_TYPE;
    else if(is_mp3_type(ext))
        return MP3_TYPE;
    else if(is_text_type(ext))
        return TXT_TYPE;
    else if(is_gb_type(ext))
        return GB_TYPE;
    else
        return UKN_TYPE;
}

