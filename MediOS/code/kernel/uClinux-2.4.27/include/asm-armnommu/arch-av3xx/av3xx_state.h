/*
* asm/arch/av3xx_usb_state.h
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

#ifndef __ASM_ARCH_AV3XX_USB_STATE_H
#define __ASM_ARCH_AV3XX_USB_STATE_H



/** usb related functions **/
int usbConnected(void);
int usbEnabled(void);
void setUsb(int state);
void enableUsb(void);
void disableUsb(void);
void forceDisableUsb(void);
int av3xx_usb_state_read(char *buf, char **start, off_t offset,int len, int *eof, void *data);
int av_usb_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

/** power related functions **/
int powerConnected(void);
int av3xx_power_state_read(char *buf, char **start, off_t offset,int len, int *eof, void *data);
int av_power_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

/** time related functions **/
int av3xx_time_read(char *buf, char **start, off_t offset,int len, int *eof, void *data);
int av_rtc_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

/** mouse related functions **/
int av_mouse_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

/** lcd timeout functions **/

/* 1 minute timeout */
#define LCD_FREQ_DEFAULT_0  60
#define LCD_FREQ_DEFAULT_1  60
#define HALT_FREQ_DEFAULT_0 180
#define HALT_FREQ_DEFAULT_1 180

void av3xx_lcd_set_state(int state);
int  av3xx_lcd_get_state(void);
void av3xx_lcd_off(void);
void av3xx_lcd_on(void);
void av3xx_lcd_launchTimer(void);
void av3xx_lcd_timer_action(unsigned long ptr);
void av3xx_halt_launchTimer(void);
void av3xx_halt_timer_action(unsigned long ptr);
void av3xx_lcd_keyPress(void);
void chgTimer(void);
int  getCurrentTimer(void);
int  av_state_sate_open(struct inode *inode, struct file *filp);
int  av_state_sate_release(struct inode *inode, struct file *filp);
int  av_state_sate_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);


/** general functions **/
int av_state_open(struct inode *inode, struct file *filp);
int av_state_release(struct inode *inode, struct file *filp);
int av_state_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);
static int av3xx_state_init(void);
static void av3xx_state_exit(void);


#endif
