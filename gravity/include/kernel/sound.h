/* 
*   include/mas.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SOUND_K_H
#define __SOUND_K_H


#include <sound.h>

#include <kernel/mas.h>

/********************* DSP                    ***************************/
/* dev functions */
void    dsp_interrupt     (int irq);
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

