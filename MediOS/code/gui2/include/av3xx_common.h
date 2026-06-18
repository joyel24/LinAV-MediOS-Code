/*
* asm/arch/av3xx_common.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
* this header is intended to be used in userland application, it contains all exported ioctl and structures
*/

#ifndef __ASM_ARCH_AV3XX_COMMON_H
#define __ASM_ARCH_AV3XX_COMMON_H

/* rtc */
struct av_tm {
    int tm_ms;
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_wday;
    int tm_mday;
    int tm_mon;
    int tm_year;
};

/* buttons, joystick */

struct av3xx_pos {
	int x;
	int y;
};

struct mouseParam {
	int freq; // amount of time between each check of buttons state
	int repeated_press; // repeate rate
};

struct av_couple {
        void * v1;
        void * v2;
};

/* mp3 player */

struct mp3_play {
        int size;
        char * buffer;
        int endOfFile;
        int buffer_read;
        int buffer_write;
	int buffer_len;
	int finished;
	char * filename;
	int pos;
        int freqPeakDraw;
        void * (*peakDraw)(int left,int right);
};

struct av_peak {
	int left;
	int right;
};

struct timer_val {
    int num;
    int val;
};

#define AV_TIMER_ON_BAT        0
#define AV_TIMER_ON_DC         1

#define CREATE_TIMER_VAR(var)                      struct timer_val var;
#define SET_VAL_TIMER(timer,val,var)               var.num=timer;var.val=val;
#define GET_VAL_TIMER(var)                         var.val;

/* ioctl */

#define AV_OP_IOC_MAGIC_USB    'a'
#define AV_OP_IOC_MAXNR_USB    4
#define AV_USB_IOC_STATE       _IOR(AV_OP_IOC_MAGIC_USB, 1, int)
#define AV_USB_IOC_ENABLE      _IO (AV_OP_IOC_MAGIC_USB, 2 )
#define AV_USB_IOC_DISABLE     _IO (AV_OP_IOC_MAGIC_USB, 3 )
#define AV_FW_EXT_IOC_STATE    _IOR(AV_OP_IOC_MAGIC_USB, 4, int)


#define AV_OP_IOC_MAGIC_POWER   'b'
#define AV_OP_IOC_MAXNR_POWER   7
#define AV_POWER_IOC_STATE     _IOR(AV_OP_IOC_MAGIC_POWER, 1, int)
#define AV_LEVEL_BAT0_IOC      _IOR(AV_OP_IOC_MAGIC_POWER, 2, int)
#define AV_LEVEL_BAT1_IOC      _IOR(AV_OP_IOC_MAGIC_POWER, 3, int)
#define AV_LEVEL_TEMP0_IOC     _IOR(AV_OP_IOC_MAGIC_POWER, 4, int)
#define AV_LEVEL_TEMP1_IOC     _IOR(AV_OP_IOC_MAGIC_POWER, 5, int)
#define AV_LEVEL_IN0_IOC       _IOR(AV_OP_IOC_MAGIC_POWER, 6, int)
#define AV_LEVEL_IN1_IOC       _IOR(AV_OP_IOC_MAGIC_POWER,7, int)

#define AV_OP_IOC_MAGIC_RTC    'c'
#define AV_OP_IOC_MAXNR_RTC    3
#define AV_RTC_GET_TIME_IOC    _IOR(AV_OP_IOC_MAGIC_RTC,1, struct av_tm)
#define AV_RTC_SET_TIME_IOC    _IOW(AV_OP_IOC_MAGIC_RTC,2, struct av_tm)
#define AV_RTC_GET_JIFFY_IOC   _IOR(AV_OP_IOC_MAGIC_RTC,3,int)

#define AV_OP_IOC_MAGIC_EVT    'd'
#define AV_OP_IOC_MAXNR_EVT    14
#define AV_GET_MOUSE_IOC       _IOR(AV_OP_IOC_MAGIC_EVT,1, struct av3xx_pos)
#define AV_SET_MOUSE_IOC       _IOW(AV_OP_IOC_MAGIC_EVT,2, struct av3xx_pos)
#define AV_SET_MOUSE_PARAM     _IOW(AV_OP_IOC_MAGIC_EVT,3, struct mouseParam)
#define AV_GET_MOUSE_PARAM     _IOR(AV_OP_IOC_MAGIC_EVT,4, struct mouseParam)
#define AV_GET_EVENT           _IOR(AV_OP_IOC_MAGIC_EVT,5, int)
#define AV_CLEAR_EVENTS        _IO(AV_OP_IOC_MAGIC_EVT,6)
#define AV_WAIT_EVENT          _IOR(AV_OP_IOC_MAGIC_EVT,7,int)
#define AV_SET_TIMER_FREQ      _IOW(AV_OP_IOC_MAGIC_EVT,8,int)
#define AV_START_TIMER         _IO(AV_OP_IOC_MAGIC_EVT,9)
#define AV_STOP_TIMER          _IO(AV_OP_IOC_MAGIC_EVT,10)
#define AV_TIMER_STATE         _IOW(AV_OP_IOC_MAGIC_EVT,11,int) /* -1 => timer stop, >=0 => elapsed time */
#define AV_DO_WAKEUP           _IO(AV_OP_IOC_MAGIC_EVT,12)
#define AV_PAUSE_APP           _IO(AV_OP_IOC_MAGIC_EVT,13)
#define AV_RELEASE_APP         _IO(AV_OP_IOC_MAGIC_EVT,14)

#define AV_OP_IOC_MAGIC_SOUND  'e'
#define AV_OP_IOC_MAXNR_SOUND  29
#define AV_GET_MIX_VOLUME      _IOR(AV_OP_IOC_MAGIC_SOUND,1, int)
#define AV_SET_MIX_VOLUME      _IOW(AV_OP_IOC_MAGIC_SOUND,2, int)
#define AV_GET_MIX_BALANCE     _IOR(AV_OP_IOC_MAGIC_SOUND,3, int)
#define AV_SET_MIX_BALANCE     _IOW(AV_OP_IOC_MAGIC_SOUND,4, int)
#define AV_GET_MIX_BASS        _IOR(AV_OP_IOC_MAGIC_SOUND,5, int)
#define AV_SET_MIX_BASS        _IOW(AV_OP_IOC_MAGIC_SOUND,6, int)
#define AV_GET_MIX_TREBLE      _IOR(AV_OP_IOC_MAGIC_SOUND,7, int)
#define AV_SET_MIX_TREBLE      _IOW(AV_OP_IOC_MAGIC_SOUND,8, int)
#define AV_GET_MIX_LOUDNESS    _IOR(AV_OP_IOC_MAGIC_SOUND,9, int)
#define AV_SET_MIX_LOUDNESS    _IOW(AV_OP_IOC_MAGIC_SOUND,10, int)
#define AV_GET_MIX_MIC_GAIN    _IOR(AV_OP_IOC_MAGIC_SOUND,11, int)
#define AV_SET_MIX_MIC_GAIN    _IOW(AV_OP_IOC_MAGIC_SOUND,12, int)
#define AV_GET_MIX_ADC_L_GAIN  _IOR(AV_OP_IOC_MAGIC_SOUND,13, int)
#define AV_SET_MIX_ADC_L_GAIN  _IOW(AV_OP_IOC_MAGIC_SOUND,14, int)
#define AV_GET_MIX_ADC_R_GAIN  _IOR(AV_OP_IOC_MAGIC_SOUND,15, int)
#define AV_SET_MIX_ADC_R_GAIN  _IOW(AV_OP_IOC_MAGIC_SOUND,16, int)
#define AV_GET_MIX_MUTE        _IOR(AV_OP_IOC_MAGIC_SOUND,17, int)
#define AV_SET_MIX_MUTE        _IOW(AV_OP_IOC_MAGIC_SOUND,18, int)
#define AV_DSP_INI_MP3         _IOW(AV_OP_IOC_MAGIC_SOUND,19, int)
#define AV_DSP_START_MP3       _IOW(AV_OP_IOC_MAGIC_SOUND,20, int)
#define AV_DSP_STOP_MP3        _IOW(AV_OP_IOC_MAGIC_SOUND,21, int)
#define AV_DSP_FRAME_CNT       _IOR(AV_OP_IOC_MAGIC_SOUND,22, int)
#define AV_DSP_IN_PEAK         _IOW(AV_OP_IOC_MAGIC_SOUND,23, struct av_peak) /* normalized version range=0->100 */
#define AV_DSP_OUT_PEAK        _IOW(AV_OP_IOC_MAGIC_SOUND,24, struct av_peak) /* normalized version range=0->100 */
#define AV_DSP_IN_PEAK_REAL    _IOW(AV_OP_IOC_MAGIC_SOUND,25, struct av_peak) /* direct read: range=0->0x7FFF    */
#define AV_DSP_OUT_PEAK_REAL   _IOW(AV_OP_IOC_MAGIC_SOUND,26, struct av_peak) /* direct read: range=0->0x7FFF    */
#define AV_DSP_PAUSE_MP3       _IOW(AV_OP_IOC_MAGIC_SOUND,27, int)
#define AV_DSP_START_PEAK      _IO(AV_OP_IOC_MAGIC_SOUND,28)
#define AV_DSP_STOP_PEAK       _IO(AV_OP_IOC_MAGIC_SOUND,29)

#define AV_OP_IOC_MAGIC_STATE  'f'
#define AV_OP_IOC_MAXNR_STATE  24
#define AV_LCD_GET_TIMOUT      _IOR(AV_OP_IOC_MAGIC_STATE,1,struct timer_val)
#define AV_LCD_SET_TIMOUT      _IOW(AV_OP_IOC_MAGIC_STATE,2,struct timer_val)
#define AV_LCD_TIMOUT_OFF      _IOW(AV_OP_IOC_MAGIC_STATE,3,struct timer_val)
#define AV_LCD_TIMOUT_ON       _IOW(AV_OP_IOC_MAGIC_STATE,4,struct timer_val)
#define AV_LCD_TIMOUT_STATE    _IOR(AV_OP_IOC_MAGIC_STATE,5,struct timer_val)
#define AV_LCD_ON              _IOW(AV_OP_IOC_MAGIC_STATE,6,struct timer_val)
#define AV_LCD_OFF             _IOW(AV_OP_IOC_MAGIC_STATE,7,struct timer_val)
#define AV_LCD_GET_STATE       _IOR(AV_OP_IOC_MAGIC_STATE,8,int)
#define AV_HD_GET_TIMOUT       _IOR(AV_OP_IOC_MAGIC_STATE,9,struct timer_val)
#define AV_HD_SET_TIMOUT       _IOW(AV_OP_IOC_MAGIC_STATE,10,struct timer_val)
#define AV_HD_STOP             _IO(AV_OP_IOC_MAGIC_STATE,11)
#define AV_HD_TIMOUT_OFF       _IOW(AV_OP_IOC_MAGIC_STATE,12,struct timer_val)
#define AV_HD_TIMOUT_ON        _IOW(AV_OP_IOC_MAGIC_STATE,13,struct timer_val)
#define AV_HD_TIMOUT_STATE     _IOR(AV_OP_IOC_MAGIC_STATE,14,struct timer_val)
#define AV_HALT_GET_TIMOUT     _IOR(AV_OP_IOC_MAGIC_STATE,15,struct timer_val)
#define AV_HALT_SET_TIMOUT     _IOW(AV_OP_IOC_MAGIC_STATE,16,struct timer_val)
#define AV_HALT_TIMOUT_OFF     _IOW(AV_OP_IOC_MAGIC_STATE,17,struct timer_val)
#define AV_HALT_TIMOUT_ON      _IOW(AV_OP_IOC_MAGIC_STATE,18,struct timer_val)
#define AV_HALT_TIMOUT_STATE   _IOR(AV_OP_IOC_MAGIC_STATE,19,struct timer_val)
#define AV_HALT_DEVICE         _IO(AV_OP_IOC_MAGIC_STATE,20)
#define AV_LCD_GET_BRIGHT      _IOR(AV_OP_IOC_MAGIC_STATE,21,int)
#define AV_LCD_SET_BRIGHT      _IOW(AV_OP_IOC_MAGIC_STATE,22,int)
#define AV_CF_MOD_IS_CONNECTED _IOR(AV_OP_IOC_MAGIC_STATE,23,int)
#define AV_CF_IS_CONNECTED     _IOR(AV_OP_IOC_MAGIC_STATE,24,int)

#define AV_OP_IOC_MAGIC_FM     'g'
#define AV_OP_IOC_MAXNR_FM     25
#define AV_FM_SET_TXT          _IOW(AV_OP_IOC_MAGIC_FM,1,char) 
#define AV_FM_GET_TXT          _IOR(AV_OP_IOC_MAGIC_FM,2,char)
#define AV_FM_SCROLL_ON        _IO(AV_OP_IOC_MAGIC_FM,3)
#define AV_FM_SCROLL_OFF       _IO(AV_OP_IOC_MAGIC_FM,4)
#define AV_FM_GET_SCROLL       _IOR(AV_OP_IOC_MAGIC_FM,5,int)
#define AV_FM_ON_ICONS         _IO(AV_OP_IOC_MAGIC_FM,6)
#define AV_FM_OFF_ICONS        _IO(AV_OP_IOC_MAGIC_FM,7)
#define AV_FM_GET_ICONS        _IOWR(AV_OP_IOC_MAGIC_FM,8,int)
#define AV_FM_SET_BAT          _IOW(AV_OP_IOC_MAGIC_FM,9,int)
#define AV_FM_GET_BAT          _IOR(AV_OP_IOC_MAGIC_FM,10,int)
#define AV_FM_SET_VOL          _IOW(AV_OP_IOC_MAGIC_FM,11,int)
#define AV_FM_GET_VOL          _IOR(AV_OP_IOC_MAGIC_FM,12,int)
#define AV_FM_IS_CONNECTED     _IOR(AV_OP_IOC_MAGIC_FM,13,int)
#define AV_FM_INI_TXT          _IO(AV_OP_IOC_MAGIC_FM,14)
#define AV_FM_SET_TMP_TXT      _IOW(AV_OP_IOC_MAGIC_FM,15,struct av_couple*)
#define AV_FM_SET_BACK_LIGHT   _IOW(AV_OP_IOC_MAGIC_FM,16,int)
#define AV_FM_SET_REC_LIGHT    _IOW(AV_OP_IOC_MAGIC_FM,17,int)
#define AV_FM_GET_BACK_LIGHT   _IOR(AV_OP_IOC_MAGIC_FM,18,int)
#define AV_FM_GET_REC_LIGHT    _IOR(AV_OP_IOC_MAGIC_FM,19,int)
#define AV_FM_RADIO_ON         _IO(AV_OP_IOC_MAGIC_FM,20)
#define AV_FM_RADIO_OFF        _IO(AV_OP_IOC_MAGIC_FM,21)
#define AV_FM_MIC_ON           _IO(AV_OP_IOC_MAGIC_FM,22)
#define AV_FM_MIC_OFF          _IO(AV_OP_IOC_MAGIC_FM,23)
#define AV_FM_SET_FREQ         _IOW(AV_OP_IOC_MAGIC_FM,24,int)
#define AV_FM_GET_FREQ         _IOR(AV_OP_IOC_MAGIC_FM,25,int)

/* MAJOR / MINOR */

#define AV_STATE_MAJOR          254

#define AV_STATE_NR_MINOR       7

#define AV_STATE_RTC_MINOR      0       /* time        /dev/avrtc             */
#define AV_STATE_POWER_MINOR    1       /* power       /dev/avpower           */
#define AV_STATE_TSC2003_MINOR  2       /* battery     /dev/avtsc             */
#define AV_STATE_USB_MINOR      3       /* usb         /dev/avusb             */
#define AV_STATE_MOUSE_MINOR    4       /* mouse       /dev/mouse             */
#define AV_STATE_STATE_MINOR    5       /* state       /dev/avstate           */
#define AV_STATE_FM_CTL_MINOR   6       /* FM_CTL      /dev/avfm              */

#define SND_DEV_CTL             0       /* Control port     /dev/mixer     */
#define SND_DEV_DSP             3       /* Digitized voice  /dev/dsp       */
#define SND_DEV_AUDIO           4       /* Sparc compatible /dev/audio     */

/* remote FM icons */
#define FM_PLAY           0x0
#define FM_REC            0x1
#define FM_PAUSE          0x2
#define FM_LOOP           0x3
#define FM_1PLAY          0x4
#define FM_BAT            0x5
#define FM_VOL            0x6

#endif
