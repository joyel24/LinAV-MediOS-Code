/* 
*   apps/avboy/loader.h
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


#ifndef __LOADER_H__
#define __LOADER_H__


typedef struct loader_s
{
	char *rom;
	char *base;
	char *sram;
	char *state;
	int ramloaded;
} loader_t;


extern loader_t loader;


int rom_load();
int sram_load();
int sram_save();



#endif


