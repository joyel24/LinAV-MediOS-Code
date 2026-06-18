/*
*   apps/avboy/avboy.h
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


void vid_update(int scanline);
void vid_init(void);
void vid_begin(void);
void vid_end(void);
void vid_settitle(char * title);
void vid_setpal(int i, int r, int g, int b);

void *sys_timer(void);
int  sys_elapsed(long * oldtick);
void sys_sleep(int us);

int doevents(void);
int do_user_menu(void);
void loadstate(int fd);
void savestate(int fd);
void browser(char * rom);
void loader_init(char *s);

void emu_run();
void emu_reset();
void cleanup();


void refresh_1(byte *dest, byte *src, byte *pal, int cnt);
void refresh_1_2x(byte *dest, byte *src, byte *pal, int cnt);


#define DIE(fmt, arg...) {printf(fmt,## arg); while(1) /*nothing*/;}
