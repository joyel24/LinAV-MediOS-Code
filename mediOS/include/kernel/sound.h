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


#include <kernel/irq.h>
#include <kernel/mas.h>

/********************* Sound buffer           ***************************/

#define FREE_SPACE_IN_BUFF(BUFF)                           \
    ({                                                     \
            int __free_space=BUFF.read-BUFF.write;         \
            if(__free_space<=0)                            \
                __free_space+=BUFF.size;                   \
            __free_space;                                  \
    })

typedef struct _SOUND_BUFFER
{
	/*unsigned char* data;
	unsigned long  size;
	unsigned long  bytes_played;
	unsigned long  loop_counter;
	unsigned long  loops_played;
	struct _SOUND_BUFFER* next_buffer;*/
        unsigned long  size;
        unsigned long  read;
        unsigned long  write;
        unsigned char* data;
        
} sound_buffer_s;




typedef struct _sound_api_param
{
	int (*reader_fct)(char * data,int count,void* param);
	int count;
} sound_api_param;

extern int sound_buff_write(sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),
            int count,void * param);


/********************* DSP                    ***************************/
/* dev functions */
void    dsp_interrupt     (int irq,struct pt_regs * regs);
void    dsp_ctl           (unsigned int cmd, void * arg);

/* mp3 functions */

int ini_mas_for_mp3(void);

/* line in functions */

void mas_line_in_on(void);
void mas_line_in_off(void);

/********************* MIXER                  ***************************/
void mixer_ctl(unsigned int cmd, int dir, void * arg);

/********************* OSS init               ***************************/
void init_sound(void);

#endif

