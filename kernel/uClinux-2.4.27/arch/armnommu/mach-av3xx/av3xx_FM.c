#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/io.h>
#include <linux/sched.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_FM.h>
#include <asm/arch/av3xx_cpld.h>
#include <asm/arch/av3xx_gio.h>
#include <asm/arch/av3xx_buttons.h>
#include <asm/arch/av3xx_common.h>

#define FM_DELAY      HZ/50
#define MAX_PING      3
#define MAX_NON_GET   5
#define NB_KEY        8

#define INI_TXT       "-- LinAv by oxygen --"

int FM_connected=0;
int nbPingSend=0;
int nbNonGet=0;
int c;
int nbPongGet=0,minPongGet=0xFFFF;
char cmd=0;
char radio_param[5];
int radio_index;
int getPong=0,firstPong=0,pingSend=0;

struct timer_list av3xx_FM_timer;
spinlock_t av3xx_uart1_lock;

int light_state=0,save_lights=0;
int lights_OnOff=1;
char cur_txt[256];
int txt_scroll=0,txt_pos=0,txt_len,txt_scroll_inc,txt_scroll_count=0,txt_scroll_ctrl=1;
int contrast=0;
int bat_state=0,vol_state=0,icons=0;

int bat_state_array[]={0x0000,0x0002,0x000A,0x000E,0x001E};
int vol_state_array[]={0x0000,0x0004,0x0006,0x0007,0x0807,0x0C07,0x0D07};

int key_evt_array[NB_KEY][2] = {
    { -1,                    0x20 }, /* REC */
    { -1,                    0x80 }, /* HOLD */
    { -1,                    0x10 }, /* MP3_FM */
    { BUTTONS_AV300_UP,      0x08 }, /* UP */
    { BUTTONS_AV300_DOWN,    0x40 }, /* DOWN */
    { BUTTONS_AV300_LEFT,    0x04 }, /* LEFT */
    { BUTTONS_AV300_RIGHT,   0x01 }, /* RIGHT */
    { BUTTONS_AV300_ON,      0x02 }  /* PRESS */
};

int av3xx_chk_FM(unsigned long ptr)
{
    char c;
    
    if(FM_connected)
    {
        if(txt_scroll && txt_scroll_ctrl)
        {
            txt_scroll_count++;
            if(txt_scroll_count>15)
            {
                txt_scroll_count=0;
                av3xx_FM_do_putText();
            }
        }
        
        //test_icons();
        if(av3xx_uart_getc(&c))
        {
            nbNonGet=0;
            nbPingSend=0;
            switch(c)
            {
                case 0xf8:
                    //av3xx_uart_putc('v');
                    FM_connected=0;
                    minPongGet=0xFFFF;
                    break;
                case 'V':
                    cmd=3;
                    break;
                case 'K':
                    cmd=1;
                    radio_index=0;
                    break;
                case 'R':
                    cmd=2;
                default:
                    switch(cmd)
                    {
                        case 0:
                            printk("[FM Remote] get unknown cmd: %x\n",c);
                            break;
                        case 1:
                            if(c!=0)
                            {
                                printk("[FM Remote] get key cmd: %x\n",c);
                                processKey(c);
                            }
                            cmd=0;
                            break;
                        case 2:
                            radio_param[radio_index++]=c;
                            if(radio_index==5)
                            {
                                printk("[FM Remote] get radio cmd: %x%x%x%x%x\n",radio_param[0],radio_param[1]
                                    ,radio_param[2],radio_param[3],radio_param[4]);
                                cmd=0;
                            }
                            break;
                        case 3:
                            //printk("[FM Remote] get pong cmd: %x\n",c);
                            cmd=0;                                    
                        default:
                            cmd=0;
                            break;
                    }
                    break;
            }
            
        }
        else
        {
            nbNonGet++;
            if(nbNonGet>MAX_NON_GET)
            {
                nbNonGet=0;
                spin_lock(&av3xx_uart1_lock);
                av3xx_uart_putc('v');
                spin_unlock(&av3xx_uart1_lock);
                nbPingSend++;
                if(nbPingSend>MAX_PING)
                {
                    FM_connected=0;
                    minPongGet=0xFFFF;
                    printk("[FM Remote] timeout\n");
                    nbPingSend=0;
                }
            }
        }
    }
    else
    {
        /* check if we get some data */
        if(av3xx_uart_getc(&c))
        {            
            getPong=0;
            switch(c)
            {
                case 0xf8:         
                    getPong=1;                    
                    pingSend=0;
                    
                    if(nbPongGet<minPongGet)
                        minPongGet=nbPongGet;
                    nbPongGet=0;
                    
                    break;
                case 'V':
                    FM_connected=1;
                    minPongGet=0xFFFF;
                    nbPingSend=0;                    
                    cmd=3;                    
                    printk("[FM Remote] connected\n");
                    av3xx_FM_do_ini_call();
                    break;

            }
        }
        else
        {
            if(getPong)
            {
                nbPongGet++;
                if(nbPongGet>minPongGet && !pingSend)
                {
                    spin_lock(&av3xx_uart1_lock);
                    av3xx_uart_putc('v');
                    spin_unlock(&av3xx_uart1_lock);
                    pingSend=1;
                }
            }            
        }        
    }  

    av3xx_FM_timer.expires = jiffies + FM_DELAY; /* 1s timer */
    add_timer(&av3xx_FM_timer);
    return 0;
}

void processKey(int key)
{
    int i;
    for(i=0;i<NB_KEY;i++)
    {
        if(key_evt_array[i][0]!=-1 && (key & key_evt_array[i][1]) )
        {           
            if(av3xx_lcd_get_state()==0)
            {
                /* the lcd is off => turn on and discard the event */
                av3xx_lcd_keyPress();
                break;
            }
            else
                av3xx_lcd_launchTimer(); /* postpone the lcd timer */
            av3xx_add_event(key_evt_array[i][0]);
        }
    }
    
}

void test_icons(void)
{
    static int count=0;
    static int icon_num=0;
    count++;
    if(count > 20)
    {
        count=0;
        bat_state++;
        if(bat_state==5)
            bat_state=0;
        vol_state++;
        if(vol_state==7)
            vol_state=0;
        
        if(icon_num==0)
        {
            FMClearIcon(0x4,icons);
        }
        else
        {
            FMClearIcon(icon_num-1,icons);
        }
            
        FMSetIcon(icon_num,icons);
        icon_num++;
        if(icon_num==5)
            icon_num=0;
        av3xx_FM_do_setIcon();
    }    
}

int av3xx_FM_is_connected(void)
{
    return FM_connected;
}

void av3xx_FM_lightsOFF(void)
{
    if(lights_OnOff==1)
    {    
        save_lights=light_state;
        light_state=0;
        lights_OnOff=0;
        av3xx_FM_do_setLight();
    }
}

void av3xx_FM_lightsON(void)
{
    if(lights_OnOff==0)
    {    
        light_state=save_lights;
        lights_OnOff=1;
        av3xx_FM_do_setLight();
    }
}

int av3xx_FM_lightsState(void)
{
    return lights_OnOff;
}

void av3xx_FM_setLight(int type,int direction)
{
    if(direction)
        if(lights_OnOff)
            light_state |= (0x1 << type);
        else
            save_lights |= (0x1 << type);
    else
        if(lights_OnOff)
            light_state &= ~(0x1 << type);
        else
            save_lights &= ~(0x1 << type);
     if(lights_OnOff)       
        av3xx_FM_do_setLight();
}

void av3xx_FM_do_setLight(void)
{
    spin_lock(&av3xx_uart1_lock);
    av3xx_uart_putc('P');
    av3xx_uart_putc(light_state&0xFF);
    spin_unlock(&av3xx_uart1_lock);
}

int av3xx_FM_getLight(int type)
{
    return ((light_state & (0x1 << type)) != 0);
}

void av3xx_FM_setScroll(int OnOff)
{
    if(txt_scroll_ctrl && !OnOff)
        txt_pos=0;
    txt_scroll_ctrl=OnOff;
}

int av3xx_FM_getScroll(void)
{
    return txt_scroll_ctrl;
}

void av3xx_FM_getText(char * txt)
{    
    strcpy(txt,cur_txt);
}

void av3xx_FM_put_iniTxt(void)
{
    av3xx_FM_putText(INI_TXT);
}

void av3xx_FM_putText(char * txt)
{
    int i=0;
    
    if(strlen(txt)>256)
        strncpy(cur_txt,txt,256);
    else
        strcpy(cur_txt,txt);
        
    if(strlen(txt)<11)
    {
        for(i=strlen(txt);i<11;i++)
        {
            cur_txt[i]=' ';
        }
        cur_txt[11]='\0';
        txt_scroll=0;        
    }
    else
    {
        txt_len=strlen(txt);
        txt_scroll=1;
        txt_scroll_inc=1;
        txt_scroll_count=0;
    }
    
    txt_pos=0;
        
    av3xx_FM_do_putText();
}

void av3xx_FM_do_putText(void)
{
    int i=0;
    spin_lock(&av3xx_uart1_lock);    
    av3xx_uart_putc('T');
    for(i=0;i<11;i++)
        av3xx_uart_putc(cur_txt[txt_pos+i]);
    if(txt_scroll && txt_scroll_ctrl)
    {
        txt_pos+=txt_scroll_inc;
        if(txt_pos<0 || txt_pos+11>txt_len)
        {
            txt_scroll_inc*=-1;
        }
    }
    spin_unlock(&av3xx_uart1_lock);    
}

void av3xx_FM_setContrast(int val)
{
    contrast=val;
    spin_lock(&av3xx_uart1_lock);    
    av3xx_uart_putc('C');
    av3xx_uart_putc(contrast&0xFF);
    spin_unlock(&av3xx_uart1_lock);  
}

int av3xx_FM_getContrast(void)
{
    return contrast;
}

void av3xx_FM_setIcon(int icon,int val)
{
    switch(icon)
    {
        case FM_BAT:
            if(val>=0 && val<5)
                bat_state=val;
            break;
        case FM_PLAY:
        case FM_REC:
        case FM_PAUSE:
        case FM_LOOP:
        case FM_1PLAY:
            if(val>=0)
            {
                FMSetIcon(icon,icons);
            }
            else
            {
                FMClearIcon(icon,icons);
            }
            break;
        case FM_VOL:
            if(val>=0 && val<7)
                vol_state=val;
            break;
        default:
            return;
    }
    av3xx_FM_do_setIcon();
}

int av3xx_FM_getIcon(int icon)
{
    switch(icon)
    {
        case FM_BAT:
            return bat_state;
        case FM_PLAY:
        case FM_REC:
        case FM_PAUSE:
        case FM_LOOP:
        case FM_1PLAY:
            return FMIsSetIcon(icon,icons);
        case FM_VOL:
            return vol_state;
            break;
        default:
            return -1;
    }
}

void av3xx_FM_do_setIcon(void)
{
    char icon_state[11]={0,0,0,0,0,0,0,0,0,0,0};
    int i;
    
    /* bat */
    icon_state[0]=bat_state_array[bat_state]&0xFF;
    icon_state[1]=(bat_state_array[bat_state]>>8)&0xFF;
    /* play */
    if(FMIsSetIcon(FM_PLAY,icons))
        icon_state[3]=0x10;
    /* rec */
    if(FMIsSetIcon(FM_REC,icons))
        icon_state[4]=0x10;
    /* pause */
    if(FMIsSetIcon(FM_PAUSE,icons))
        icon_state[5]=0x02;
    /* loop */
    if(FMIsSetIcon(FM_LOOP,icons))
        icon_state[6]=0x02;
    /* 1 play */
    if(FMIsSetIcon(FM_1PLAY,icons))
        icon_state[7]=0x01;
    /* bat */
    icon_state[9]=vol_state_array[vol_state]&0xFF;
    icon_state[10]=(vol_state_array[vol_state]>>8)&0xFF;    
      
    
    spin_lock(&av3xx_uart1_lock);    
    av3xx_uart_putc('I');
    for(i=0;i<11;i++)
        av3xx_uart_putc(icon_state[i]&0xFF);
    spin_unlock(&av3xx_uart1_lock);   
}

void av3xx_FM_do_ini_call(void)
{
    av3xx_FM_do_setLight();
    av3xx_FM_do_putText();
    av3xx_FM_setContrast(contrast);
}

int av3xx_uart_getc(char * c)
{
    if(inw(AV3XX_UART1_BASE+AV3XX_UART_RFCR)&0xFF)
    {
        *c=inw(AV3XX_UART1_BASE+AV3XX_UART_DTRR)&0xFF;
        return 1;
    }
    else
       return 0;
}

void av3xx_uart_putc(char c)
{
    /* reading transmission triger status*/
    while(!(inw(AV3XX_UART1_BASE+AV3XX_UART_SR)&0x0400)) /* nothing */;    
    outw(c&0x00FF,AV3XX_UART1_BASE+AV3XX_UART_DTRR);
}

static int __init av3xx_FM_init(void)
{
    /* setting the gio and cpld */
    av3xx_gio_dir(AV3XX_GIO_SPDIF,GIO_OUT);
    av3xx_gio_dir(AV3XX_GIO_VID_OUT,GIO_OUT);
    av3xx_gio_set(AV3XX_GIO_SPDIF);
    av3xx_gio_set(AV3XX_GIO_VID_OUT);
    cpld_set_port_3(AV3XX_CPLD_FM);
    
    /*setting up the UART1 port */
    outw(0x015F,AV3XX_UART1_BASE+AV3XX_UART_BRSR); /* 9600 BAUD */
    outw(0x8000,AV3XX_UART1_BASE+AV3XX_UART_MSR);
    outw(0x0000,AV3XX_UART1_BASE+AV3XX_UART_RFCR);
    outw(0x0300,AV3XX_UART1_BASE+AV3XX_UART_TFCR);
    
    /* init the uart spinlock used to send packet of data */
    spin_lock_init(&av3xx_uart1_lock);
    
    /* initiale state */
    
    light_state=0x1;
    av3xx_FM_put_iniTxt();    
    contrast=0x00;
    
   /* setting up a timer to watch FM state */     
    init_timer(&av3xx_FM_timer);
    av3xx_FM_timer.function = av3xx_chk_FM;
    av3xx_FM_timer.expires = jiffies + FM_DELAY; /* 1s timer */
    add_timer(&av3xx_FM_timer);
   
    /* everything is ok */
    printk("av3xx-FM driver by oxygen77@free.fr\n");
    return 0;
}

static void __exit av3xx_FM_exit(void)
{
	/* remove timer ?? */
}

module_init(av3xx_FM_init);
module_exit(av3xx_FM_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Cpld FM for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
