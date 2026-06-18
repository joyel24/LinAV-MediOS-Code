/*
*   include/kernel/mp3_data.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __PLAY_LIST_H
#define __PLAY_LIST_H

#include <kernel/id3.h>


struct list_entry
{
    struct mp3entry id3;
    struct list_entry * nxt;
    struct list_entry * prev;
};

int  addPlaylist(char * filename);
struct list_entry * curEntryInList(void);
struct list_entry * nxtEntryInList(void);
struct list_entry * prevEntryInList(void);
int  nbEntryInList(void);
void iniPlaylist(void);
void cleanPlaylist(void);
int  addDir(char * dirName);
void showList(void);

#endif
