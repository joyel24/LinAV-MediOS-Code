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
struct tm {
    unsigned char tm_ms;
    unsigned char tm_sec;
    unsigned char tm_min;
    unsigned char tm_hour;
    unsigned char tm_wday;
    unsigned char tm_mday;
    unsigned char tm_mon;
    unsigned char tm_year;
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

/* mp3 player */

struct mp3_play {
	char * nxt;
	char * cur;
	char * tmp;
	int needData;
	int decRunning;
	int size;
	int finished;
};

struct av_peak {
	int left;
	int right;
};

/* ioctl */

#define AV_OP_IOC_MAGIC        'a'

#define AV_USB_IOC_STATE       _IOR(AV_OP_IOC_MAGIC, 1, int)
#define AV_USB_IOC_ENABLE      _IO (AV_OP_IOC_MAGIC, 2 )
#define AV_USB_IOC_DISABLE     _IO (AV_OP_IOC_MAGIC, 3 )

#define AV_POWER_IOC_STATE     _IOR(AV_OP_IOC_MAGIC, 4, int)

#define AV_LEVEL_BAT0_IOC      _IOR(AV_OP_IOC_MAGIC, 5, int)
#define AV_LEVEL_BAT1_IOC      _IOR(AV_OP_IOC_MAGIC, 6, int)
#define AV_LEVEL_TEMP0_IOC     _IOR(AV_OP_IOC_MAGIC, 7, int)
#define AV_LEVEL_TEMP1_IOC     _IOR(AV_OP_IOC_MAGIC, 8, int)
#define AV_LEVEL_IN0_IOC       _IOR(AV_OP_IOC_MAGIC, 9, int)
#define AV_LEVEL_IN1_IOC       _IOR(AV_OP_IOC_MAGIC,10, int)

#define AV_RTC_GET_TIME_IOC    _IOR(AV_OP_IOC_MAGIC,11, struct tm)
#define AV_RTC_SET_TIME_IOC    _IOW(AV_OP_IOC_MAGIC,12, struct tm)

#define AV_GET_MOUSE_IOC       _IOR(AV_OP_IOC_MAGIC,13, struct av3xx_pos)
#define AV_SET_MOUSE_IOC       _IOW(AV_OP_IOC_MAGIC,14, struct av3xx_pos)

#define AV_GET_MIX_VOLUME      _IOR(AV_OP_IOC_MAGIC,15, int)
#define AV_SET_MIX_VOLUME      _IOW(AV_OP_IOC_MAGIC,16, int)
#define AV_GET_MIX_BALANCE     _IOR(AV_OP_IOC_MAGIC,17, int)
#define AV_SET_MIX_BALANCE     _IOW(AV_OP_IOC_MAGIC,18, int)
#define AV_GET_MIX_BASS        _IOR(AV_OP_IOC_MAGIC,19, int)
#define AV_SET_MIX_BASS        _IOW(AV_OP_IOC_MAGIC,20, int)
#define AV_GET_MIX_TREBLE      _IOR(AV_OP_IOC_MAGIC,21, int)
#define AV_SET_MIX_TREBLE      _IOW(AV_OP_IOC_MAGIC,22, int)
#define AV_GET_MIX_LOUDNESS    _IOR(AV_OP_IOC_MAGIC,23, int)
#define AV_SET_MIX_LOUDNESS    _IOW(AV_OP_IOC_MAGIC,24, int)
#define AV_GET_MIX_MIC_GAIN    _IOR(AV_OP_IOC_MAGIC,25, int)
#define AV_SET_MIX_MIC_GAIN    _IOW(AV_OP_IOC_MAGIC,26, int)
#define AV_GET_MIX_ADC_L_GAIN  _IOR(AV_OP_IOC_MAGIC,27, int)
#define AV_SET_MIX_ADC_L_GAIN  _IOW(AV_OP_IOC_MAGIC,28, int)
#define AV_GET_MIX_ADC_R_GAIN  _IOR(AV_OP_IOC_MAGIC,29, int)
#define AV_SET_MIX_ADC_R_GAIN  _IOW(AV_OP_IOC_MAGIC,30, int)
#define AV_GET_MIX_MUTE        _IOR(AV_OP_IOC_MAGIC,31, int)
#define AV_SET_MIX_MUTE        _IOW(AV_OP_IOC_MAGIC,32, int)

#define AV_DSP_INI_MP3         _IOW(AV_OP_IOC_MAGIC,33, int)
#define AV_DSP_START_MP3       _IOW(AV_OP_IOC_MAGIC,34, int)
#define AV_DSP_STOP_MP3        _IOW(AV_OP_IOC_MAGIC,35, int)
#define AV_DSP_FRAME_CNT       _IOR(AV_OP_IOC_MAGIC,36, int)
#define AV_DSP_IN_PEAK         _IOW(AV_OP_IOC_MAGIC,37, struct av_peak) /* normalized version range=0->100 */
#define AV_DSP_OUT_PEAK        _IOW(AV_OP_IOC_MAGIC,38, struct av_peak) /* normalized version range=0->100 */
#define AV_DSP_IN_PEAK_REAL    _IOW(AV_OP_IOC_MAGIC,39, struct av_peak) /* direct read: range=0->0x7FFF    */
#define AV_DSP_OUT_PEAK_REAL   _IOW(AV_OP_IOC_MAGIC,40, struct av_peak) /* direct read: range=0->0x7FFF    */

#define AV_SET_MOUSE_PARAM     _IOW(AV_OP_IOC_MAGIC,41, struct mouseParam)
#define AV_GET_MOUSE_PARAM     _IOR(AV_OP_IOC_MAGIC,42, struct mouseParam)

#define AV_OP_IOC_MAXNR        42

/* MAJOR / MINOR */

#define AV_STATE_MAJOR          254

#define AV_STATE_NR_MINOR       5

#define AV_STATE_RTC_MINOR      0       /* time /dev/avrtc             */
#define AV_STATE_POWER_MINOR    1       /* time /dev/avpower           */
#define AV_STATE_TSC2003_MINOR  2       /* time /dev/avtsc             */
#define AV_STATE_USB_MINOR      3       /* time /dev/avusb             */
#define AV_STATE_MOUSE_MINOR    4       /* time /dev/mouse             */

#define SND_DEV_CTL             0       /* Control port /dev/mixer     */
#define SND_DEV_DSP             3       /* Digitized voice /dev/dsp    */
#define SND_DEV_AUDIO           4       /* Sparc compatible /dev/audio */

#endif
