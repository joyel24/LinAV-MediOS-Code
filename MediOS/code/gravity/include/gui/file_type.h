/*
* include/gui/file_type.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __GUI_FILE_TYPE_H
#define __GUI_FILE_TYPE_H

#define BIN_TYPE          0
#define SCRIPT_TYPE       1
#define IMG_TYPE          2
#define MP3_TYPE          3
#define TXT_TYPE          4
#define UKN_TYPE          5

int  get_file_type        (char * filename);
void handle_type_other    (char *filename);

#endif

