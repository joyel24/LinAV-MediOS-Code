/*
* asm/arch/av3xx_ide.h
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

#ifndef __ASM_ARCH_AV3XX_IDE_H
#define __ASM_ARCH_AV3XX_IDE_H

int  av_ide_open (struct inode *inode, struct file *filp);
int  av_ide_release (struct inode *inode, struct file *filp);
int  av_ide_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
int  av_ide_check_change(kdev_t i_rdev);
int  av_ide_revalidate(kdev_t i_rdev);
int  av_ide_transfer(const struct request * req);
void av_ide_request(request_queue_t *q);
void av3xx_cf_connected(void);
void av3xx_cf_disconnected(void);
void av3xx_cf_reader_connected(void);
void av3xx_cf_reader_disconnected(void);
int  av3xx_ide_init(void);
void av3xx_ide_exit(void);

void av3xx_ide_halt(void);
void av3xx_hd_launchTimer(void);
void av3xx_hd_timer_fct(unsigned long ptr);
void av3xx_hd_timer_on(int num);
void av3xx_hd_timer_off(int num);
int  av3xx_hd_timer_state(int num);

#endif
