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


/********************* MIXER                  ***************************/
void mixer_ctl(unsigned int cmd, int dir, void * arg);

/********************* OSS init               ***************************/
void sound_init(void);

#endif

