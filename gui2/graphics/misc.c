/*
* misc.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "misc.h"

#define doGenIoctl(dev,ioctl_cmd,param,error)  {if(gen_ioctl(dev,ioctl_cmd,param)<0){printf(error);return 0;}}

int getTimeS(char * timeSt)
{    
    struct av_tm date_time={0,0,0,0,0,0,0,0};
    
    if(getTime(&date_time))
    {
    	sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", date_time.tm_hour,date_time.tm_min,date_time.tm_sec,
                                                         date_time.tm_mday,date_time.tm_mon,date_time.tm_year);
        return 1;
    }
    else
    {
        sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", 0,0,0,0,0,0);
    	return 0;
    }
}

int getTime(struct av_tm * date_time)
{
    doGenIoctl("/dev/avrtc",AV_RTC_GET_TIME_IOC,date_time,"Error getting time and date\n");
     
    if(date_time->tm_hour<0 || date_time->tm_hour>23) date_time->tm_hour=0;
    if(date_time->tm_min<0 || date_time->tm_min>59) date_time->tm_min=0;
    if(date_time->tm_sec<0 || date_time->tm_sec>59) date_time->tm_sec=0;
    if(date_time->tm_mday<0 || date_time->tm_mday>31) date_time->tm_mday=0;
    if(date_time->tm_mon<0 || date_time->tm_mon>12) date_time->tm_mon=0;
    //if(date_time->tm_year<0 || date_time->tm_year>999) date_time->tm_year=0;
    
    return 1;
}

int setTime(struct av_tm * date_time)
{
    doGenIoctl("/dev/avrtc",AV_RTC_SET_TIME_IOC,date_time,"Error setting time and date\n");
    return 1;
}

int set_mouseParam(int freq, int repeat)
{
    struct mouseParam param;
    param.freq = freq;
    param.repeated_press = repeat;
    doGenIoctl("/dev/mouse",AV_SET_MOUSE_PARAM,&param,"Error setting mouse params\n");
    return 1;
}

int get_mouseFreq()
{
    struct mouseParam param;
    doGenIoctl("/dev/mouse",AV_GET_MOUSE_PARAM,&param,"Error getting mouse params\n");
    return param.freq;
}

int get_mouseRepeat()
{
    struct mouseParam param;
    doGenIoctl("/dev/mouse",AV_GET_MOUSE_PARAM,&param,"Error getting mouse params\n");
    return param.repeated_press;
}

int getBat(void)
{
    int power;
    doGenIoctl("/dev/avpower",AV_LEVEL_BAT0_IOC,&power,"Error getting bat level\n");
    return power;
}

int getPwr(void)
{
    int plug;
    doGenIoctl("/dev/avpower",AV_POWER_IOC_STATE,&plug,"Error getting DC state\n");
    return plug;
}

int getUSB(void)
{
    int usb;
    doGenIoctl("/dev/avusb",AV_USB_IOC_STATE,&usb,"Error getting usb connector state\n");
    return usb;
}

int getFwExt(void)
{
    int fw_ext;
    doGenIoctl("/dev/avusb",AV_FW_EXT_IOC_STATE,&fw_ext,"Error getting FW_EXT state\n");  
    return fw_ext;
}

int setUSB(int state)
{
    int usb_ioctl;
    if(state)
        usb_ioctl=AV_USB_IOC_ENABLE;
    else
        usb_ioctl=AV_USB_IOC_DISABLE;
    doGenIoctl("/dev/avusb",usb_ioctl,NULL,"Error setting usb state\n");       
    return 1;
}

int getTick(void)
{
    int tick;
    doGenIoctl("/dev/avrtc",AV_RTC_GET_JIFFY_IOC,&tick,"Error getting tick val\n");    
    return tick;
}

int halt_device(void)
{
    doGenIoctl("/dev/avstate",AV_HALT_DEVICE,NULL,"Error sending halt\n");
    return 1;    
}

int fmIsConnected(void)
{
    int res;
    doGenIoctl("/dev/avfm",AV_FM_IS_CONNECTED,&res,"Error getting FM con. state\n");
    return 1;
}

int fmSetBat(int val)
{
    doGenIoctl("/dev/avfm",AV_FM_SET_BAT,&val,"Error setting bat. level on FM\n");
    return 1;
}

int fmSetVol(int val)
{
    doGenIoctl("/dev/avfm",AV_FM_SET_VOL,&val,"Error setting volume level on FM\n");    
    return 1;
}

int fmSetTxt(char * txt)
{
    doGenIoctl("/dev/avfm",AV_FM_SET_TXT,txt,"Error sending normal text to FM\n");
    return 1;
}

int fmSetTmpTxt(char * txt,int iter)
{
    struct av_couple var;
    var.v1=(void*)txt;
    var.v2=(void*)&iter;
    doGenIoctl("/dev/avfm",AV_FM_SET_TMP_TXT,&var,"Error sending tmp text to FM\n");
    return 1;
}

int fmSetBackLight(int val)
{
    doGenIoctl("/dev/avfm",AV_FM_SET_BACK_LIGHT,&val,"Error setting backlight on FM\n");
    return 1;
}

int fmGetBackLight(void)
{
    int val;
    doGenIoctl("/dev/avfm",AV_FM_GET_BACK_LIGHT,&val,"Error getting backlight on FM\n");
    return val;
}

int fmSetRecLight(int val)
{
    doGenIoctl("/dev/avfm",AV_FM_SET_REC_LIGHT,&val,"Error setting REC light on FM\n");
    return 1;
}

int fmGetRecLight(void)
{
    int val;
    doGenIoctl("/dev/avfm",AV_FM_GET_REC_LIGHT,&val,"Error getting REC light on FM\n");
    return val;
}

int fmOnRadio(void)
{
    doGenIoctl("/dev/avfm",AV_FM_RADIO_ON,NULL,"Error turning on radio\n");
    return 1;
}

int fmOffRadio(void)
{
    doGenIoctl("/dev/avfm",AV_FM_RADIO_OFF,NULL,"Error turning off radio\n");
    return 1;
}

float fmGetFreq(void)
{
    int val;
    doGenIoctl("/dev/avfm",AV_FM_GET_FREQ,&val,"Error getting freq from radio\n");
    printf("received: %d\n",val);
    return (float)val/1000;
}

int fmSetFreq(float freq)
{
    int val=(int)(freq*1000);
    doGenIoctl("/dev/avfm",AV_FM_SET_FREQ,&val,"Error setting freq from radio\n");
    return 1;
}

int set_lcd_TimeOutParam(int state, int value)
{
    struct timer_val param;
    param.num = state;
    param.val = value;
    doGenIoctl("/dev/avstate",AV_LCD_SET_TIMOUT,&param,"Error setting lcd timeout\n");
    //printf("SET lcd timeout : %d\n", param.val);
    return 1;
}

int get_lcd_TimeOutParam(int state)
{
    struct timer_val param;
    param.num = state;
    param.val = 0;
    doGenIoctl("/dev/avstate",AV_LCD_GET_TIMOUT,&param,"Error getting lcd timeout\n");
    //printf("GET lcd timeout : %d\n", param.val);
    return param.val;
}

int CF_mod_is_connected(void)
{
    int state;
    doGenIoctl("/dev/avstate",AV_CF_MOD_IS_CONNECTED,&state,"Error getting CF module state\n");
    return state;
}

int powerOff_timer_on(void)
{
    doGenIoctl("/dev/avstate",AV_HALT_TIMOUT_ON,NULL,"Error turning on POWEROFF timer\n");
    return 1;
}

int powerOff_timer_off(void)
{
    doGenIoctl("/dev/avstate",AV_HALT_TIMOUT_OFF,NULL,"Error turning off POWEROFF timer\n");
    return 1;
}

int get_power_off_timer_state(void)
{
   int state;
   doGenIoctl("/dev/avstate",AV_HALT_TIMOUT_STATE,&state,"Error getting POWEROFF state\n"); 
   return state;
}

int CF_is_connected(void)
{
    int state;
    doGenIoctl("/dev/avstate",AV_CF_IS_CONNECTED,&state,"Error getting CF state\n");
    return state;
}

int gen_ioctl(char * dev_name, int ioctl_cmd,void * param)
{
    int fd = 0;
    fd=open(dev_name,O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        printf("Can't open %s\n",dev_name);
        return -1;
    }
    
    if(ioctl(fd,ioctl_cmd,param)<0)
    {
      close(fd);
      return -2;
    }
    close(fd);
    return 0;
}

int set_hd_TimeOutParam(int state, int value)
{
    struct timer_val param;
    param.num = state;
    param.val = value;
    doGenIoctl("/dev/avstate",AV_HD_SET_TIMOUT,&param,"Error setting hd timeout\n");
    //printf("SET lcd timeout : %d\n", param.val);
    return 1;
}

int get_hd_TimeOutParam(int state)
{
    struct timer_val param;
    param.num = state;
    param.val = 0;
    doGenIoctl("/dev/avstate",AV_HD_GET_TIMOUT,&param,"Error getting hd timeout\n");
    //printf("GET lcd timeout : %d\n", param.val);
    return param.val;
}

int set_halt_TimeOutParam(int state, int value)
{
    struct timer_val param;
    param.num = state;
    param.val = value;
    doGenIoctl("/dev/avstate",AV_HALT_SET_TIMOUT,&param,"Error setting halt timeout\n");
    //printf("SET lcd timeout : %d\n", param.val);
    return 1;
}

int get_halt_TimeOutParam(int state)
{
    struct timer_val param;
    param.num = state;
    param.val = 0;
    doGenIoctl("/dev/avstate",AV_HALT_GET_TIMOUT,&param,"Error getting halt timeout\n");
    //printf("GET lcd timeout : %d\n", param.val);
    return param.val;
}

int getBrightness(void)
{
    int val;
    doGenIoctl("/dev/avstate",AV_LCD_GET_BRIGHT,&val,"Error getting brightness\n");
    return val;
}

int setBrightness(int val)
{
    doGenIoctl("/dev/avstate",AV_LCD_SET_BRIGHT,&val,"Error setting brightness\n");
    return 1;
}
