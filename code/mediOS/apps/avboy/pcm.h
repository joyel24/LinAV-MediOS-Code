/* 
*   apps/avboy/include/pcm.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
* 
*  Date:     18/10/2005
* Author:   GliGli

*  Modified by CjNr11 06/12/2005
*/

#ifndef __PCM_H__
#define __PCM_H__

struct pcm
{
	int hz, len;
	int stereo;
	byte *buf;
	int pos;
};

extern struct pcm pcm;

int  pcm_submit(void);
void pcm_init(void);

#endif


