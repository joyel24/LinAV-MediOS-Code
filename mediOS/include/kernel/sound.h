/*
*   include/kernel/mas.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SOUND_K_H
#define __SOUND_K_H

#include <kernel/playList.h>

typedef struct _SOUND_BUFFER
{
    unsigned char* data;
    unsigned long  size;
    unsigned long  read;
    unsigned long  write;
    int playing;
    void (*startPlayback)(void);
    /*unsigned long  bytes_played;
    unsigned long  loop_counter;
    unsigned long  loops_played;
    struct _SOUND_BUFFER* next_buffer;*/
} sound_buffer_s;

void sound_initPlayer(void);

void sound_initMp3(void);
void sound_freeMp3(void);

void sound_readMore(void);
int openFileFromList(struct list_entry * cur_entry);

/********************* MIXER                  ***************************/
void mixer_ctl(unsigned int cmd, int dir, void * arg);

/********************* sound init               ***************************/
void sound_init(void);


#endif

