/* 
*   kernel/driver/fm_remote.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <kernel/io.h>
#include <kernel/irqs.h>
#include <kernel/irq.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/gio.h>
#include <kernel/buttons.h>
#include <kernel/bat_power.h>
#include <kernel/uart.h>

#include <evt.h>
#include <kernel/evt.h>

#include <kernel/fm_remote.h>



#define INI_TXT       "--mediOS--"

int FM_connected=0;
int nbPingSend=0;
int nbNonGet=0;
int nbPongGet=0,minPongGet=0xFFFF;
char cmd=0;
char radio_param[5];
int radio_index;
int getPong=0,firstPong=0,pingSend=0;

int inHold=0;

int light_state=0,save_lights=0;
int lights_OnOff=1;

struct txt_data {
    char txt[256];
    int scroll;
    int pos;
    int len;
    int scroll_inc;
};

#define NORMAL_TXT   0
#define TMP_TXT      1

struct txt_data txt[2];

int cur_txt=NORMAL_TXT;
int txt_scroll_count=0;
int tmp_iter=0;

int txt_scroll_ctrl=1;

int contrast=0;
int bat_state=0,vol_state=0,icons=0;

int bat_state_array[]={0x0000,0x0002,0x000A,0x000E,0x001E};
int vol_state_array[]={0x0000,0x0004,0x0006,0x0007,0x0807,0x0C07,0x0D07};

int key_evt_array[NB_KEY][2] = {
    { BTN_FM_REC   , 0x20 }, /* REC */
    { -1           , 0x80 }, /* HOLD */
    { BTN_FM_MP3FM , 0x10 }, /* MP3_FM */
    { BTN_UP       , 0x08 }, /* UP */
    { BTN_DOWN     , 0x40 }, /* DOWN */
    { BTN_LEFT     , 0x04 }, /* LEFT */
    { BTN_RIGHT    , 0x01 }, /* RIGHT */
    { BTN_ON       , 0x02 }  /* PRESS */
};

void fm_remote_INT(int irq_num,struct pt_regs * reg)
{
    char c;
    int count;
    while(uart_in(&c,UART_1))
    {
        if(FM_connected)
        {
            switch(c)
            {
                case 0xf8:
                    FM_connected=0;
                    nbPingSend=0;
                    break;
                case 'V':
                    cmd=3;
                    nbPingSend=0;
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
                                //printk("[FM Remote] get key cmd: %x\n",c);
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
            irq_disable(IRQ_UART1);
            switch(c)
            {
                case 0xf8:
                    while(1)
                    {
                        count=0;
                        while(!uart_in(&c,UART_1) && count<4200) count++;
                        if(count >=4200)
                        {
                            uart_out('v',UART_1);
                            printk("is this a pause?\n");
                            break;
                        }
                    }
                    break;
                 case 'V':
                    FM_connected=1;
                    nbPingSend=0;
                    inHold=0;
                    FM_do_ini_call();
                    printk("[FM Remote] connected\n");
                    break;
                 default:
                    printk("don't know what to do with %c %02x\n",c,c);
            }
            irq_enable(IRQ_UART1);
        }
    }
        
}

void processKey(int key)
{
    int i;
    
    /* special process for HOLD key */    
    if(key & 0x80 && key_evt_array[1][0]==-1)
    {
        inHold=~inHold;
        
        printk("[FM key] hold %s\n",inHold?"enable":"disable");
        return;
    }
    
    for(i=0;i<NB_KEY;i++)
    {
        if(key_evt_array[i][0]!=-1 && (key & key_evt_array[i][1]) )
        {     
            struct evt_t _evt;           
            if(lcd_get_state()==0)
            {
                /* the lcd is off => turn on and discard the event */
                lcd_keyPress();
                break;
            }
            else
                lcd_launchTimer(); /* postpone the lcd timer */
            halt_launchTimer(); /* postpone the poweroff timer */
            _evt.evt= key_evt_array[i][0];
            _evt.evt_class=BTN_CLASS;
            _evt.data=NULL;
            evt_send(&_evt);
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
        FM_do_setIcon();
    }    
}

int FM_is_connected(void)
{
    return FM_connected;
}

void FM_lightsOFF(void)
{
    if(lights_OnOff==1)
    {    
        save_lights=light_state;
        light_state=0;
        lights_OnOff=0;
        FM_do_setLight();
    }
}

void FM_lightsON(void)
{
    if(lights_OnOff==0)
    {    
        light_state=save_lights;
        lights_OnOff=1;
        FM_do_setLight();
    }
}

int FM_lightsState(void)
{
    return lights_OnOff;
}

void FM_setLight(int type,int direction)
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
        FM_do_setLight();
}

void FM_do_setLight(void)
{
    uart_out('P',UART_1);
    uart_out(light_state&0xFF,UART_1);
}

int FM_getLight(int type)
{
    return ((light_state & (0x1 << type)) != 0);
}

void FM_setScroll(int OnOff)
{
    if(txt_scroll_ctrl && !OnOff)
        txt[cur_txt].pos=0;
    txt_scroll_ctrl=OnOff;
}

int FM_getScroll(void)
{
    return txt_scroll_ctrl;
}

void FM_getText(char * str)
{    
    strcpy(str,txt[cur_txt].txt);
}

void FM_put_iniTxt(void)
{
    FM_savText(INI_TXT,NORMAL_TXT);
    cur_txt=NORMAL_TXT;
    //FM_do_putText();
}

void FM_putText(char * str)
{
    //printk("[FM] get normal txt:%s\n",str);
    FM_savText(str,NORMAL_TXT);
    if(cur_txt==NORMAL_TXT)
        FM_do_putText();
}

void FM_putTmpText(char * str,int iter)
{
    printk("[FM] get tmp txt:%s (iter=%d)\n",str,iter);
    FM_savText(str,TMP_TXT);
    tmp_iter=iter;
    cur_txt=TMP_TXT;
    FM_do_putText();
}

void FM_savText(char * str,int num)
{
    int i=0;
    int len=strlen(str);
    
    if(len>256)
    {
        strncpy(txt[num].txt,str,256);
        len=256;
    }
    else
        strcpy(txt[num].txt,str);
        
    if(len<11)
    {
        for(i=len;i<11;i++)
        {
            txt[num].txt[i]=' ';
        }
        txt[num].txt[11]='\0';
        txt[num].scroll=0;        
    }
    else
    {
        txt[num].len=len;
        txt[num].scroll=1;
        txt[num].scroll_inc=1;        
    }
    
    txt[num].pos=0;    
}

void FM_do_putText(void)
{
    FM_send_data('T',&txt[cur_txt].txt[txt[cur_txt].pos],11);
            
    if(txt[cur_txt].scroll && txt_scroll_ctrl)
    {
        txt[cur_txt].pos+=txt[cur_txt].scroll_inc;
        if(txt[cur_txt].pos<0 || txt[cur_txt].pos+11>txt[cur_txt].len)
        {
            txt[cur_txt].scroll_inc*=-1;
        }
    }
        
}

void FM_setContrast(int val)
{
    contrast=val;
    uart_out('C',UART_1);
    uart_out(contrast&0xFF,UART_1);
}

int FM_getContrast(void)
{
    return contrast;
}

void FM_setIcon(int icon,int val)
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
    FM_do_setIcon();
}

int FM_getIcon(int icon)
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

void FM_do_setIcon(void)
{
    char icon_state[11]={0,0,0,0,0,0,0,0,0,0,0};
    
    /* bat */
    icon_state[0]=bat_state_array[bat_state]&0xFF;
    //icon_state[1]=(bat_state_array[bat_state]>>8)&0xFF;
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
      
    FM_send_data('I',icon_state,11);    
}

int FM_radio_search_mode=FM_IN_SEARCH_MODE;
int FM_radio_search_dir=FM_SEARCH_DOWN;
int FM_radio_main_mute=FM_ON;
int FM_radio_mic_State=FM_OFF;
int FM_radio_stby=FM_ON;
int fm_freq=87800;

void FM_do_turn_on_radio(void)
{
    printk("in turn on radio %d\n",fm_freq);
    FM_radio_search_mode=FM_IN_SET_MODE;
    FM_radio_main_mute=FM_OFF;
    FM_radio_stby=FM_OFF;
    FM_radio_mic_State=FM_OFF;
    FM_do_setRadio();
    /* ask MAS to turn on line in */
   // mas_line_in_on();
}

void FM_do_turn_off_radio(void)
{
    printk("in turn off radio %d\n",fm_freq);
    FM_radio_main_mute=FM_ON;
    FM_radio_stby=FM_ON;
    FM_do_setRadio();
    /* ask MAS to turn off line in */
    //mas_line_in_off();
}

void FM_do_turn_on_mic(void)
{
    FM_radio_main_mute=FM_ON;
    FM_radio_stby=FM_ON;
    FM_radio_mic_State=FM_OFF;
    FM_do_setRadio();
    /* ask MAS to turn on line in */
    //mas_line_in_on();
}

void FM_do_turn_off_mic(void)
{
    FM_radio_mic_State=FM_ON;
    FM_do_setRadio();
    /* ask MAS to turn off line in */
    //mas_line_in_off();
}

void FM_do_setRadio(void)
{
    char radio_data[5]={0,0,0,0,0};
    int PLL;
    
    /* byte 0 */
    radio_data[0] |= (FM_radio_main_mute & 0x1) << 7;
    radio_data[0] |= (FM_radio_search_mode & 0x1) << 6;
    
#warning we need bound for freq
    if(fm_freq<(60*1000) || fm_freq > (200*1000))
        fm_freq=100*1000;
    
    /* compute PLL, assuming we have XTAL of 13MHz, inter freq of 225KHz and HIGH injection*/
    
    PLL=(4*(fm_freq+225))/50;
    PLL&=0x3FFF;
    radio_data[0] |= (PLL >> 8) & 0x3F;
    radio_data[1] = PLL & 0xFF;
    
    radio_data[2] |= (FM_radio_search_dir & 0x1) << 7;
    /* search stop level default to HIGH */
    radio_data[2] |= 0x0  << 5;
    /* HIGH injection */
    radio_data[2] |= 0x1  << 4;
    /* Stereo */
    radio_data[2] |= 0x0  << 3;
    /* unMute right */
    radio_data[2] |= 0x0  << 2;
    /* unMute left */
    radio_data[2] |= 0x0  << 1;
    
    radio_data[2] |= 0x1;
    //radio_data[2] |= (FM_radio_mic_State & 0x1);
    
    /* setting default val for byte 3 : 0x0E  see doc for details */
    radio_data[3] = 0x0E;
    
    /* setting default val for byte 4 : 0x40  see doc for details */
    radio_data[4] = 0x40;
    
    printk("R: %02x %02x %02x %02x %02x\n",radio_data[0],radio_data[1],radio_data[2],radio_data[3],radio_data[4]);
    
    FM_send_data('R',radio_data,5);
}

void FM_do_ini_call(void)
{
    FM_do_setLight();
    FM_do_putText();
    FM_setContrast(contrast);
}

void FM_send_data(char cmd,char * data,int size)
{
    int i;
    uart_out(cmd,UART_1);
    for(i=0;i<size;i++)
        uart_out(data[i]&0xFF,UART_1);
}

void init_fm_remote(void)
{
    char c;
    /* setting the gio and cpld */
    GIO_DIRECTION(GIO_SPDIF,GIO_OUT);
    GIO_DIRECTION(GIO_VID_OUT,GIO_OUT);
    GIO_SET(GIO_SPDIF);
    GIO_SET(GIO_VID_OUT);
    CPLD_SET_PORT3(CPLD_FM);
    
    /*setting up the UART1 port */
    outw(0x015F,UART1_BASE+UART_BRSR); /* 9600 BAUD */
    //outw(0x8000,UART1_BASE+UART_MSR);
    //outw(0x0000,UART1_BASE+UART_RFCR);
    //outw(0x0300,UART1_BASE+UART_TFCR);
    
    /* initiale state */
    
    inHold=0;
    FM_connected=0;
    
    light_state=0x1;
    FM_put_iniTxt();    
    contrast=0x00;

    irq_changeHandler(IRQ_UART1,fm_remote_INT);
    /* launch the INT handler once */
    while(uart_in(&c,UART_1)) /*nothing*/;
    //fm_remote_INT(IRQ_UART1);
    /* everything is ok */
    printk("[init] fm remote\n");
}

