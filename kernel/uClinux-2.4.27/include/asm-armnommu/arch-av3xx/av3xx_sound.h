/*
* asm/arch/av3xx_sound.h
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
#ifndef __ASM_ARCH_AV3XX_SOUND_H
#define __ASM_ARCH_AV3XX_SOUND_H

/********************* DSP                    ***************************/
/* dev functions */
int av3xx_dsp_open(struct inode *inode, struct file *filp);
int av3xx_dsp_close(struct inode *inode, struct file *filp);
void av3xx_dsp_interrupt(int irq, void *dev_id, struct pt_regs *regs);
ssize_t av3xx_dsp_write(struct file *filp, const char *buf, size_t count, loff_t *ppos);
ssize_t av3xx_dsp_read(struct file *filp, const char *buf, size_t count, loff_t *ppos);
int av3xx_dsp_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

/* mp3 functions */
void mp3_read_more(void);
int start_mp3_playback(void);
int stop_mp3_playback(void);
int ini_mp3_playback(struct mp3_play * arg);
int ini_mas_for_mp3(void);

/********************* MIXER                  ***************************/
int av3xx_mix_open(struct inode *inode, struct file *filp);
int av3xx_mix_close(struct inode *inode, struct file *filp);
int av3xx_mix_write(struct file *filp, const char *buf, size_t count, loff_t *ppos);
int av3xx_mix_read(struct file *filp, const char *buf, size_t count, loff_t *ppos);
int av3xx_mix_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);

/********************* sound general open/close   ***************************/
int av3xx_sound_open(struct inode *inode, struct file *filp);
int av3xx_sound_close(struct inode *inode, struct file *filp);

/********************* OSS init               ***************************/
static int av3xx_sound_init(void);
static void av3xx_sound_exit(void);

#endif

