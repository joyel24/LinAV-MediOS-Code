#include <linux/sched.h>
#include <linux/kbd_ll.h>
#include <linux/kbd_kern.h>
#include <linux/input.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/major.h>

#include <asm/keyboard.h>
#include <asm/io.h>

#include <asm/arch/av3xx_common.h>
#include <asm/arch/av3xx_buttons.h>

#define NB_BUTTONS 10

#define AV3XX_MAX_X      320
#define AV3XX_MAX_Y      240

struct timer_list av3xx_button_timer;

int freq_rep = AV_FREQ;
int mx_press = MAX_PRESSED;

extern void av_halt_system(void);
/*DECLARE_TASKLET(av_halt,av_halt_system,0);*/

#if 0 // this config is using escaped chars
int keys_code[NB_BUTTONS] ={
			KEY_UP, KEY_LEFT, KEY_RIGHT, KEY_DOWN,       /* UP, LEFT, RIGHT, DOWN */
			KEY_3, KEY_1, KEY_2,              /* Menu3, Menu1, Menu2 */
			KEY_P,                            /* Joypress */
			KEY_O, KEY_F                      /* On, Off */
};
#else
int keys_code[NB_BUTTONS] ={
			KEY_U, KEY_L, KEY_R, KEY_D,       /* UP, LEFT, RIGHT, DOWN */
			KEY_3, KEY_1, KEY_2,              /* Menu3, Menu1, Menu2 */
			KEY_P,                            /* Joypress */
			KEY_O, KEY_F                      /* On, Off */
};
#endif

/*int av3xx_button_state(int * button_state)
{
	int val,i,pressed,dir;
	pressed=0;
	dir=0;
	val=inw(0x2600680)&0x3;
	val|=((inw(0x2600700)&0x7)<<2);
	val|=((inw(0x2600780)&0x7)<<5);
	val|=((inw(0x30588)&0x1)<<8);
	val|=((inw(0x3058a)>>3)&0x200);

	for(i=0;i<NB_BUTTONS;i++)
	{
		if(val&(0x1<<i))
		{
			button_state[i]=0;
		}
		else
		{
			button_state[i]=1;
			pressed=1;
			if(i<4)
				dir=1;
		}
	}
	if(dir)
		button_state[BUTTONS_AV300_JOYPRESS]=0;

	return pressed;
}*/

int nb_pressed[NB_BUTTONS];
int nb_off_press=0;


int av3xx_chk_button(unsigned long ptr)
{
	int i,val,fastDir;
	
	fastDir=0;
	val=inw(0x2600680)&0x3;
	val|=((inw(0x2600700)&0x7)<<2);
	val|=((inw(0x2600780)&0x7)<<5);
	val|=((inw(0x30588)&0x1)<<8);
	val|=((inw(0x3058a)>>3)&0x200);
	
	for(i=0;i<NB_BUTTONS;i++)
	{
		if(keys_code[i]!=0)
		{
			if(val&(0x1<<i))
			{
				if(nb_pressed[i]!=0)
					nb_pressed[i]=0;
				if(i==9)
					nb_off_press=0;
			}
			else
			{	
				if(i==9)
					nb_off_press++;
				if(!(i==4 && fastDir))
				{
					if(nb_pressed[i]==0)
					{
						handle_scancode(keys_code[i], 1);
						nb_pressed[i]=mx_press;
					}
					else
						nb_pressed[i]--;
					if(i<4)
					{
						fastDir=1;
						av3xx_move_mouse(i);
					}
				}
				
			}
		}
	}
	
	if(nb_off_press>MAX_OFF)
		av_halt_system();
		//tasklet_schedule(&av_halt);
	
	av3xx_button_timer.expires = jiffies + freq_rep; /* 1s timer */
	add_timer(&av3xx_button_timer);
	return 0;
}

int x,y;

void av3xx_move_mouse(int but)
{
	switch(but)
	{
		case BUTTONS_AV300_UP:
			y-=STEP_SIZE;
			if(y<0)
				y=0;
			break;
		case BUTTONS_AV300_LEFT:
			x-=STEP_SIZE;
			if(x<0)
				x=0;
			break;
		case BUTTONS_AV300_RIGHT:
			x+=STEP_SIZE;
			if(x>AV3XX_MAX_X)
				x=AV3XX_MAX_X;
			break;
		case BUTTONS_AV300_DOWN:
			y+=STEP_SIZE;
			if(y>AV3XX_MAX_Y)
				y=AV3XX_MAX_Y;
			break;
		default:
			break;
	}
}

int av3xx_button_get_mouse(struct av3xx_pos * pos)
{
	pos->x=x;
	pos->y=y;
	return 0;
}

int av3xx_button_set_mouse(struct av3xx_pos * pos)
{
	if(pos->x >= 0 && pos->x <= AV3XX_MAX_X && pos->y >= 0 && pos->y <= AV3XX_MAX_Y)
	{
		x=pos->x;
		y=pos->y;
		return 0;
	}
	else	
		return -1;
}

int av3xx_button_set_mouse_param(struct mouseParam * ptrParam)
{
	freq_rep = ptrParam->freq;
	mx_press = ptrParam->repeated_press;
}

int av3xx_button_get_mouse_param(struct mouseParam * ptrParam)
{
	ptrParam->freq=freq_rep;
	ptrParam->repeated_press=mx_press;
}

int av3xx_button_init(void)
{
	int i,result;
	
	x=AV3XX_MAX_X/2;
	y=AV3XX_MAX_Y/2;
	
	for(i=0;i<NB_BUTTONS;i++)
		nb_pressed[i]=0;
		
	init_timer(&av3xx_button_timer);
	av3xx_button_timer.function = av3xx_chk_button;
	av3xx_button_timer.expires = jiffies + freq_rep; /* 1s timer */
	add_timer(&av3xx_button_timer);
	
	printk("[Av3xx init] buttons driver (freq=%d,rep=%d)\n",freq_rep,mx_press);

	return 0;
}

int av3xx_button_exit(void)
{
	/* remove timer ?? */
	return 0;
}
