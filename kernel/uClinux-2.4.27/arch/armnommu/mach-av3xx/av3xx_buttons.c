#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

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
#include <asm/arch/av3xx_gio.h>

#define NB_BUTTONS 10

#define AV3XX_MAX_X      320
#define AV3XX_MAX_Y      240

#define BUFFER_SIZE      50

DECLARE_WAIT_QUEUE_HEAD(button_queue);
DECLARE_WAIT_QUEUE_HEAD(app_queue);

struct timer_list av3xx_button_timer;

struct event_q {
	int evt;
	struct event_q * nxt;
};
struct event_q * head;
struct event_q * tail;

int freq_rep = AV_FREQ;
int mx_press = MAX_PRESSED;

int timerState = 0;
int timerMaxCnt=10;;
int timerCnt=0;

int usbState,pwrState;

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

int nb_pressed[NB_BUTTONS];
int nb_off_press=0;
int wakeUP=0;

int av3xx_chk_button(unsigned long ptr)
{
    int i,val,fastDir;
    int doWake=0;
    int keyPressed=0;
    
    fastDir=0;
        
    if(wakeUP)
    {
        wakeUP=0;
        av3xx_clear_buffer();
        av3xx_add_event(EVT_AV300_WKUP);
        wake_up_interruptible(&button_queue);
        av3xx_button_timer.expires = jiffies + freq_rep; /* 1s timer */
        add_timer(&av3xx_button_timer);
        return 0;
    }
        
    val=inw(0x2600680)&0x3;
    val|=((inw(0x2600700)&0x7)<<2);
    val|=((inw(0x2600780)&0x7)<<5);
#warning should use gio fction
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
                {
                    nb_off_press++;
                    if(nb_off_press>MAX_OFF)
                        av_halt_system();
                }
                if(!(i==4 && fastDir))
                {
                    if(nb_pressed[i]==0)
                    {
                        nb_pressed[i]=mx_press;
                        if(av3xx_lcd_get_state()==0)
                        {
                            /* the lcd is off => turn on and discard the event */
                            av3xx_lcd_keyPress();
                            break;
                        }
                        else
                            av3xx_lcd_launchTimer(); /* postpone the lcd timer */
                        av3xx_add_event(i);
                        //handle_scancode(keys_code[i], 1);                        
                        doWake=1;
                        keyPressed=1;
                        
                    }
                    else
                        nb_pressed[i]--;
                    if(i<4)
                    {
                        fastDir=1;
                        //av3xx_move_mouse(i);
                    }
                }
                
            }
        }
    }
    
    
        //tasklet_schedule(&av_halt);
    
           
    if(timerState)
    {
        timerCnt++;
        if(timerCnt>timerMaxCnt)
        {
            av3xx_add_event(EVT_AV300_TIMER);
            timerCnt=0;
            doWake=1;
        }
    }
    
    if(usbConnected()!=usbState)
    {
        usbState=usbConnected();
        av3xx_add_event(EVT_AV300_USB);
        doWake=1;
    }
    
    if(powerConnected()!=pwrState)
    {
        pwrState=powerConnected();
        /* change the timers */
        if(pwrState)
            chgTimer(AV_TIMER_ON_DC);
        else
            chgTimer(AV_TIMER_ON_BAT);
            
        av3xx_add_event(EVT_AV300_PWR);
        doWake=1;
    }
    
    if(doWake)
    {
        wake_up_interruptible(&button_queue);
    }
        
    doWake=0;
    
    av3xx_button_timer.expires = jiffies + freq_rep; /* 1s timer */
    add_timer(&av3xx_button_timer);
    return 0;
}

int x,y;

void av3xx_add_event(int evt)
{
	if(head->evt!=-1)
	{
		tail=tail->nxt;
	}
	head->evt=evt;
	head=head->nxt;
}

int av3xx_wait_event(void)
{
    if(tail->evt!=-1)
        return av3xx_get_event();
    else
    {
        while(tail->evt==-1)
            interruptible_sleep_on(&button_queue);
        return av3xx_get_event();
    }
}

void av3xx_wakeup_evt(void)
{
	wakeUP=1;
}

void av3xx_do_pause(void)
{
	interruptible_sleep_on(&app_queue);
}

void av3xx_release_app(void)
{
	wake_up_interruptible(&app_queue);
}

int av3xx_get_event(void)
{
	int val;
	if(tail->evt!=-1)
	{
		val=tail->evt;
		tail->evt=-1;
		if(head!=tail)
			tail=tail->nxt;
		return val;
	}
	else
		return -1;
}

void av3xx_clear_buffer(void)
{
	while(tail->evt!=-1)
	{
		tail->evt=-1;
		tail=tail->nxt;
	}
}

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
	return 0;
}

int av3xx_button_get_mouse_param(struct mouseParam * ptrParam)
{
	ptrParam->freq=freq_rep;
	ptrParam->repeated_press=mx_press;
	return 0;
}

void av3xx_start_timer(void)
{
	timerCnt=0;
	timerState=1;
}

void av3xx_stop_timer(void)
{
	timerState=0;
}

int av3xx_timer_state(void)
{
	if(timerState)
	{
		return (timerCnt*freq_rep*10)/HZ;
	}
	else
		return -1;
}

void av3xx_set_timer_freq(int val) // in 1/10 of s
{
	timerMaxCnt=(val*HZ)/(freq_rep*10);
	timerCnt=0;
}

static int __init av3xx_button_init(void)
{
	int i;
	struct event_q * evt;
	x=AV3XX_MAX_X/2;
	y=AV3XX_MAX_Y/2;
		
	for(i=0;i<NB_BUTTONS;i++)
		nb_pressed[i]=0;
		
	// init event's buffer
	
	if(!(head=(struct event_q*)kmalloc(sizeof(struct event_q),GFP_KERNEL)))
	{
		printk("[Av3xx init] buttons driver ====> error ini event buffer\n");
		return -1;
	}
	head->evt=-1;
	tail=head; // using tail as the previous evt to build the chain
	for(i=1;i<BUFFER_SIZE;i++)
	{
		if(!(evt=(struct event_q*)kmalloc(sizeof(struct event_q),GFP_KERNEL)))
		{
			printk("[Av3xx init] buttons driver ====> error ini event buffer\n");
			return -1;
		}
		evt->evt=-1;
		tail->nxt=evt;
		tail=evt;
	}
	tail->nxt=head;
	tail=head;
	
	timerState = 0;
	timerMaxCnt=10;;
	timerCnt=0;
        
        usbState=usbConnected();
        pwrState=powerConnected();
        
        /* set GIO for ON/OFF to input */
        av3xx_gio_dir(AV3XX_GIO_ON_BTN,GIO_IN);
        av3xx_gio_dir(AV3XX_GIO_OFF_BTN,GIO_IN);
		
	init_timer(&av3xx_button_timer);
	av3xx_button_timer.function = av3xx_chk_button;
	av3xx_button_timer.expires = jiffies + freq_rep; /* 1s timer */
	add_timer(&av3xx_button_timer);
	
	printk("av3xx-buttons driver by oxygen77@free.fr\n");

	return 0;
}

static void __exit av3xx_button_exit(void)
{
	/* remove timer ?? */
}

module_init(av3xx_button_init);
module_exit(av3xx_button_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Button driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");

