
#include <sys_def/string.h>



#include "defs.h"

#include "avboy.h"
#include "sound.h"
#include "regs.h"
#include "hw.h"
#include "cpu.h"
#include "mem.h"
#include "lcd.h"
#include "rtc.h"
#include "pcm.h"

static int framelen = 16743;
static int framecount;

void emu_init()
{
	
}


/*
 * emu_reset is called to initialize the state of the emulated
 * system. It should set cpu registers, hardware registers, etc. to
 * their appropriate values at powerup time.
 */

void emu_reset()
{
	hw_reset();
	lcd_reset();
	cpu_reset();
	mbc_reset();
	sound_reset();
}




/* This mess needs to be moved to another module; it's just here to
 * make things work in the mean time. */

void *sys_timer();

__IRAM_CODE void emu_run()
{
	void *timer = sys_timer();
	int delay;
	bool run=true;
//	int ot;

	vid_begin();
	lcd_begin();
	do
	{
    //printf("%d\n",get_tick());//-ot);
		//ot=get_tick();
    cpu_emulate(2280);
		while (R_LY > 0 && R_LY < 144)
    	cpu_emulate(cpu.lcdc);

		vid_end();
  	rtc_tick();
	//	sound_mix();
		if (!pcm_submit())
		{
			delay = framelen - sys_elapsed(timer);
			sys_sleep(delay);
			sys_elapsed(timer);
		}
		run=doevents();
		vid_begin();
		if (framecount) { if (!--framecount) DIE("finished\n"); }

		if (!(R_LCDC & 0x80))
			cpu_emulate(32832);

		while (R_LY > 0) /* wait for next frame */
    	cpu_emulate(cpu.lcdc);
	}
	while(run);
}












