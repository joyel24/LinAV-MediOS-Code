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

/** power related functions **/
int powerConnected(void);
int av3xx_power_state_read(char *buf, char **start, off_t offset,int len, int *eof, void *data);

/** time related functions **/
int av3xx_time_read(char *buf, char **start, off_t offset,int len, int *eof, void *data);

/** tsc2003 related functions **/
int av3xx_tsc2003_read(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data);

/** general functions **/
int av_state_open(struct inode *inode, struct file *filp);
int av_state_release(struct inode *inode, struct file *filp);
int av_state_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);
int av3xx_state_init(void);


#endif
