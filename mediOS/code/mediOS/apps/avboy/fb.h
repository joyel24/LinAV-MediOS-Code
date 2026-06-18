/* 
*   apps/avboy/include/fb.h
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

#ifndef __FB_H__
#define __FB_H__

struct fb
{
	byte *ptr;
	int w, h;
	int pelsize;
	int pitch;
	int indexed;
	struct
	{
		int l, r;
	} cc[4];
	int yuv;
	int enabled;
	int dirty;
};


extern struct fb fb;


#endif




