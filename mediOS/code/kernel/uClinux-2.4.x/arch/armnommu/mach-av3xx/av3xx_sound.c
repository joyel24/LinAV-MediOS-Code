/*
* asm/arch/av3xx_sound.c
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
#define __KERNEL_SYSCALLS__

#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#include <linux/unistd.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/types.h>
/*#include <linux/soundcard.h>
#include <linux/sound.h>*/
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_common.h>
#include <asm/arch/av3xx_mas.h>
#include <asm/arch/av3xx_sound.h>



/********************* DSP                    ***************************/

static struct file_operations  av3xx_dsp_fops = {
	open: av3xx_dsp_open,
	release: av3xx_dsp_close,
	write: av3xx_dsp_write,
	read: av3xx_dsp_read,
	ioctl: av3xx_dsp_ioctl,
};

int av_dsp_dev_count = 0;
int av_dsp_state_count = 0;
spinlock_t av_dsp_dev_lock,av_dsp_state_lock;
DECLARE_WAIT_QUEUE_HEAD(dsp_queue);

//int decRunning=0;
//char * buff=NULL;
int pos=0;
//int size=1020*2;

int running=0;
int lastBuff=0;

char * cur,*tmp,*nxt;
int cur_s,nxt_s;
int in_pause=0;

int fd;

DECLARE_TASKLET(do_read_more,mp3_read_more,0);

struct mp3_play * data;

int av3xx_dsp_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_dsp_dev_lock);
	if (av_dsp_dev_count) {
		spin_unlock(&av_dsp_dev_lock);
		return -EBUSY; 
	}
	av_dsp_dev_count++;
	spin_unlock(&av_dsp_dev_lock);
		
	running=0;
	lastBuff=0;
	in_pause=0;
	pos=0;
	
	disable_irq(MAS_INTERRUPT);
	
	ini_mas_for_mp3();

	printk("DSP is now open\n");
	return 0;
}

int av3xx_dsp_close(struct inode *inode, struct file *filp)
{
	int i;
	
	disable_irq(MAS_INTERRUPT);
	
	av3xx_mas_app_select(MASS_APP_NONE);
	while(1)
	{
		i=av3xx_mas_app_running(MASS_APP_ANY);
		if(i<0)
		{
			printk("error getting app status\n");
			break;
		}
		if(i==0)
			break;
	}
	av_dsp_dev_count--;
	return 0;
}

int cont=0;

void av3xx_dsp_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	spin_lock(&av_dsp_state_lock);	
	if(running)
	{		
		if(pos<cur_s)
		{
			pos+=av3xx_mas_pio_write((void *) (cur+pos),60);
		}
		
		if(pos>=cur_s)
		{
			if(nxt==NULL)
			{
				running=0;
			}
			else
			{	
				tmp=cur;
				cur=nxt;
				nxt=NULL;
				cur_s=nxt_s;
			}
			tasklet_schedule(&do_read_more);
			pos=0;
		}
	}
	spin_unlock(&av_dsp_state_lock);
}

ssize_t av3xx_dsp_write(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	printk("dsp write called !!! no write functions");
	return count;
}

int av3xx_dsp_read(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	printk("dsp read called !!! no read functions");
	return count;
}

int av3xx_dsp_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{	
	int * val;
	struct av_peak * av_p;
	switch(cmd)
	{
		case AV_DSP_INI_MP3:
			return ini_mp3_playback((struct mp3_play *)arg);
		case AV_DSP_START_MP3:
			return start_mp3_playback();
		case AV_DSP_STOP_MP3:
			return stop_mp3_playback();
			break;
		case AV_DSP_PAUSE_MP3:
			running=0;
			in_pause=1;
			break;
		case AV_DSP_FRAME_CNT:
			val=(int*)arg;
			*val=av3xx_mas_get_frame_count();
			break;
		case AV_DSP_IN_PEAK:
			av_p=(struct av_peak *)arg;
			av_p->left=(av3xx_mas_read_codec(MAS_REG_INPEAK_LEFT)*100)/0x7FFF;
			av_p->right=(av3xx_mas_read_codec(MAS_REG_INPEAK_RIGHT)*100)/0x7FFF;
			break;
		case AV_DSP_OUT_PEAK:
			av_p=(struct av_peak *)arg;
			av_p->left=(av3xx_mas_read_codec(MAS_REG_OUTPEAK_LEFT)*100)/0x7FFF;
			av_p->right=(av3xx_mas_read_codec(MAS_REG_OUTPEAK_RIGHT)*100)/0x7FFF;
			break;
		case AV_DSP_IN_PEAK_REAL:
			av_p=(struct av_peak *)arg;
			av_p->left=av3xx_mas_read_codec(MAS_REG_INPEAK_LEFT);
			av_p->right=av3xx_mas_read_codec(MAS_REG_INPEAK_RIGHT);
			break;
		case AV_DSP_OUT_PEAK_REAL:
			av_p=(struct av_peak *)arg;
			av_p->left=av3xx_mas_read_codec(MAS_REG_OUTPEAK_LEFT);
			av_p->right=av3xx_mas_read_codec(MAS_REG_OUTPEAK_RIGHT);
			break;
		default:
			return -ENOTTY;
	}	
	
	return 0;
}

void mp3_read_more(void)
{
	int nb_read;
	if(!lastBuff)
	{
		nb_read=read(fd,tmp,data->size); // !!!!!!!!!!!!!!!!!!!!!!! error handling
		if(nb_read<data->size) // end of file
		{
			lastBuff=1;
		}
		
		if(!running)
		{
			nxt=tmp;
			tmp=cur;
			cur=nxt;
			cur_s=nb_read;
			nxt=NULL;
			running=1;
			av3xx_dsp_interrupt(MAS_INTERRUPT,NULL,NULL);
			mp3_read_more();
		}
		else
		{
			nxt_s=nb_read;
			nxt=tmp;
		}
	}
	else
	{
		if(!running)
		{
			data->finished=1;
			stop_mp3_playback();
		}
	}
}

//*************************************************************************************************************************
//*************************************************************************************************************************
// Appel à close sur fd ??????????????????????????????????????,
//*************************************************************************************************************************
//*************************************************************************************************************************

int start_mp3_playback()
{
	if(in_pause)
	{
		running=1;
		av3xx_dsp_interrupt(MAS_INTERRUPT,NULL,NULL);
	}
	else
	{
		enable_irq(MAS_INTERRUPT);
		mp3_read_more();
	}
	return 0;
}

int stop_mp3_playback(void)
{
	int i=0;
	running=0;
	disable_irq(MAS_INTERRUPT);
	for(i=0;i<100;i++);
	close(fd);
	if(nxt==NULL)
		kfree(tmp);
	else
		kfree(nxt);
	kfree(cur);
	return 0;
}

int ini_mp3_playback(struct mp3_play * arg)
{
	data=arg;
	
	// allocating 2 buffers
	cur=(char*)kmalloc(data->size,GFP_KERNEL);
	if(!cur)
	{
		printk("Can't allocate buff 0\n");
		goto err_buff0;
	}
	tmp=(char*)kmalloc(data->size,GFP_KERNEL);
	if(!tmp)
	{
		printk("Can't allocate buff 1\n");
		goto err_buff1;
	}
	nxt=NULL;
	
	//opening the file	
	fd=open(data->filename,O_RDONLY,0);
	if(fd<0)
	{
		printk("Can't open file %s\n",data->filename);
		goto err_file;
	}
	
	//setting up some data
	data->pos=0;
	pos=0;
	lastBuff=0;
	in_pause=0;
	running=0;
	data->finished=0;
	
	return 0;
	
err_file:
	kfree(tmp);
err_buff1:
	kfree(cur);
err_buff0:
	return -1;
}


int ini_mas_for_mp3(void)
{
	int i;
	av3xx_mas_write_codec(MAS_REG_AUDIO_CONF,MAS_INPUT_AD | MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
	                           | 0xf  << 4 // mic gain
				   | 0xf << 8  // adc gain right
				   | 0xf <<12  // adc gain left
				   );
	av3xx_mas_write_codec(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_MONO);
	av3xx_mas_write_codec(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
	av3xx_mas_write_codec(MAS_REG_MIX_DSP_SCALE,0x40 << 8);
	av3xx_mas_write_codec(MAS_REG_DA_OUTPUT_MODE,0x0);
	av3xx_mas_control_config(MAS_SET,MAS_BALANCE,50);
	av3xx_mas_control_config(MAS_SET,MAS_VOLUME,70);
	for(i=0;i<20;i++) /* NOTHING */ ;
	
	av3xx_mas_app_select(MASS_APP_NONE);
	while(1)
	{
		i=av3xx_mas_app_running(MASS_APP_ANY);
		if(i<0)
		{
			printk("error getting app status\n");
			return -1;
		}
		if(i==0)
			break;
	}
	av3xx_mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
	av3xx_mas_set_clk_speed(0x4800);
	av3xx_mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);
	for(i=0;i<20;i++) /* NOTHING */ ;
	
	av3xx_mas_app_select(MASS_APP_MPEG3_DEC | MASS_APP_MPEG2_DEC);	
	while(1)
	{
		i=av3xx_mas_get_D0(MAS_APP_SELECT);
		if(i<0)
		{
			printk("error getting app status\n");
			return -1;
		}
		if((i & MASS_APP_MPEG3_DEC) && (i & MASS_APP_MPEG2_DEC))
			break;
	}
		
	printk("MAS configured for mp3 playing, waiting for start\n");
	
	return 0;
}

/********************* MIXER                 ***************************/

static struct file_operations  av3xx_mix_fops = {
	open: av3xx_mix_open,
	release: av3xx_mix_close,
	write: av3xx_mix_write,
	read: av3xx_mix_read,
	ioctl: av3xx_mix_ioctl,
};

int av_mixer_dev_count = 0;
spinlock_t av_mixer_dev_lock;


int av3xx_mix_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_mixer_dev_lock);
	if (av_mixer_dev_count) {
		spin_unlock(&av_mixer_dev_lock);
		return -EBUSY; 
	}
	av_mixer_dev_count++;
	spin_unlock(&av_mixer_dev_lock);
	
		
	printk("MIXER is now open\n");
	return 0;
}

int av3xx_mix_close(struct inode *inode, struct file *filp)
{
	av_mixer_dev_count--;
	printk("MIXER is now closed\n");
	return 0;
}

int av3xx_mix_write(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	return 0;
}

int av3xx_mix_read(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	return 0;
}

int oldVol=0;

int av3xx_mix_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
	int * val=(int*)arg;
	int tmp;
        switch(cmd)
	{
		case AV_SET_MIX_VOLUME:
			av3xx_mas_control_config(MAS_SET,MAS_VOLUME,*val);
			oldVol=*val;
			break;
		case AV_GET_MIX_VOLUME:
			*val=av3xx_mas_control_config(MAS_GET,MAS_VOLUME,*val);
			break;
		case AV_SET_MIX_BALANCE:
			av3xx_mas_control_config(MAS_SET,MAS_BALANCE,*val);
			break;
		case AV_GET_MIX_BALANCE:
			*val=av3xx_mas_control_config(MAS_GET,MAS_BALANCE,*val);
			break;	
		case AV_SET_MIX_MUTE:
			if(*val)
				av3xx_mas_control_config(MAS_SET,MAS_VOLUME,0);
			else
				av3xx_mas_control_config(MAS_SET,MAS_VOLUME,oldVol);
			break;
		case AV_GET_MIX_MUTE:
			tmp=av3xx_mas_control_config(MAS_GET,MAS_VOLUME,*val);
			if(tmp==0)
				*val=1;
			else
				*val=0;
			break;
		case AV_SET_MIX_BASS:
			av3xx_mas_control_config(MAS_SET,MAS_BASS,*val);
			break;
		case AV_GET_MIX_BASS:
			*val=av3xx_mas_control_config(MAS_GET,MAS_BASS,*val);
			break;
		case AV_SET_MIX_TREBLE:
			av3xx_mas_control_config(MAS_SET,MAS_TREBLE,*val);
			break;
		case AV_GET_MIX_TREBLE:
			*val=av3xx_mas_control_config(MAS_GET,MAS_TREBLE,*val);
			break;
		case AV_SET_MIX_LOUDNESS:
			av3xx_mas_control_config(MAS_SET,MAS_LOUDNESS,*val);
			break;
		case AV_GET_MIX_LOUDNESS:
			*val=av3xx_mas_control_config(MAS_GET,MAS_LOUDNESS,*val);
			break;
		case AV_SET_MIX_MIC_GAIN:
			av3xx_mas_control_config(MAS_SET,MAS_MICRO_GAIN,*val);
			break;
		case AV_GET_MIX_MIC_GAIN:
			*val=av3xx_mas_control_config(MAS_GET,MAS_MICRO_GAIN,*val);
			break;
		case AV_SET_MIX_ADC_L_GAIN:
			av3xx_mas_control_config(MAS_SET,MAS_ADC_L_GAIN,*val);
			break;
		case AV_GET_MIX_ADC_L_GAIN:
			*val=av3xx_mas_control_config(MAS_GET,MAS_ADC_L_GAIN,*val);
			break;
		case AV_SET_MIX_ADC_R_GAIN:
			av3xx_mas_control_config(MAS_SET,MAS_ADC_R_GAIN,*val);
			break;
		case AV_GET_MIX_ADC_R_GAIN:
			*val=av3xx_mas_control_config(MAS_GET,MAS_ADC_R_GAIN,*val);
			break;
		default:
                	printk("[ioctl MIX] bad ioctl\n");
			return -ENOTTY;
	}	
	
	return 0;
}

/********************* sound general open/close               ***************************/

static struct file_operations  av3xx_sound_fops = {
	open: av3xx_sound_open,
	release: av3xx_sound_close,
	write: NULL,
	read: NULL,
	ioctl: NULL,
};

int av3xx_sound_open(struct inode *inode, struct file *filp)
{
	switch (MINOR(inode->i_rdev))
	{
		/* DSP */
		case SND_DEV_DSP:
		case SND_DEV_AUDIO:
			filp->f_op=&av3xx_dsp_fops;
			return filp->f_op->open(inode,filp);
		
		/* MIXER */
		case SND_DEV_CTL:
			filp->f_op=&av3xx_mix_fops;
			return filp->f_op->open(inode,filp);
		
		default:
			return -ENODEV;
	}
	return 0;
}

int av3xx_sound_close(struct inode *inode, struct file *filp) // this one should never be used
{
	return 0;
}
	
/********************* OSS init               ***************************/

static int __init av3xx_sound_init(void)
{
	int ret;
	struct mas_version version;
	
	if((ret=register_chrdev(SOUND_MAJOR, "av3xx_sound", &av3xx_sound_fops))<0)
	{
		printk(KERN_WARNING "Unable to get a major number for sound driver\n");
		return ret;
	}
	
	spin_lock_init(&av_dsp_dev_lock);
	spin_lock_init(&av_mixer_dev_lock);
	spin_lock_init(&av_dsp_state_lock);
	printk("Loading av3xx sound driver:  ");
	av3xx_mas_gio_init();
	av3xx_mas_reset();
	av3xx_mas_read_version(&version);
	oldVol=av3xx_mas_control_config(MAS_GET,MAS_VOLUME,0);
	disable_irq(MAS_INTERRUPT);
	if((ret=request_irq(MAS_INTERRUPT,av3xx_dsp_interrupt,0,"av3xx_mas",NULL))<0)
	{
		unregister_chrdev(SOUND_MAJOR, "av3xx_sound");
		printk(KERN_WARNING "Unable to set our irq handler (irq=%d)\n",MAS_INTERRUPT);
		return ret;
	}
	printk("MAS%x:%d:%c%d\n",version.major_number,version.derivate,version.char_order_version,version.digit_order_version);
	return 0;
}

static void __exit av3xx_sound_exit(void)
{
	unregister_chrdev(SOUND_MAJOR, "av3xx_sound");
	free_irq(MAS_INTERRUPT,NULL);
}

module_init(av3xx_sound_init);
module_exit(av3xx_sound_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Sound driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
