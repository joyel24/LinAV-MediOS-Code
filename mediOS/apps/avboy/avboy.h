/* By CjNr11 07/12/2005 */

#include "defs.h"


void vid_update(int scanline);
void vid_init(void);
void vid_begin(void);
void vid_end(void);
void die(char *fmt, ...);
void vid_settitle(char * title);
void *sys_timer(void);
int  sys_elapsed(long * oldtick);
void sys_sleep(int us);
int  pcm_submit(void);
void pcm_init(void);
bool doevents(void);
int do_user_menu(void);
void loadstate(int fd);
void savestate(int fd);
void browser(char * rom);
void loader_init(char *s);
__IRAM_CODE void emu_run();
void emu_reset();
void cleanup();
void pad_release(byte k);
void pad_press(byte k);


