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

#include <linux/init.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
/*#include <linux/soundcard.h>
#include <linux/sound.h>*/
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_mas.h>
#include <asm/arch/av3xx_sound.h>
#include <asm/arch/av3xx_common.h>


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
int last=0;

/*char * cur;
char * nxt;
char * tmp;
char buff0[1020];
char buff1[1020];*/


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
	last=0;
	pos=0;
	
	disable_irq(MAS_INTERRUPT);
	
	ini_mp3_dec();
	
	
	
	printk("DSP is now open\n");
	return 0;
}

int av3xx_dsp_close(struct inode *inode, struct file *filp)
{
	int i;
	
	disable_irq(MAS_INTERRUPT);
	
	av3xx_mas_app_select(MASS_APP_NONE);
	printk("Sending MAS stop\n");
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
	printk("MAS stop confirmed\n");
	av_dsp_dev_count--;
	printk("DSP is now closed\n");
	return 0;
}

int cont=0;

void av3xx_dsp_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	int val;
	spin_lock(&av_dsp_state_lock);	
	if(data->decRunning && running)
	{		
		if(pos<(data->size))
		{
			pos+=av3xx_mas_pio_write((void *) (data->cur+pos),60);
		}
		
		if(pos>=(data->size))
		{
			if(data->nxt==NULL)
			{
				printk("n ");				
				data->decRunning=0;
				if(last)
				{
					printk("last\n");
					data->finished=1;
					//disable_irq(MAS_INTERRUPT);
				}
				else
				{
					pos=0;
					data->needData=1;
				}
				spin_unlock(&av_dsp_state_lock);
				return;
			}
			else
			{	
				data->tmp=data->cur;
				data->cur=data->nxt;
				data->nxt=NULL;
				data->needData=1;
				pos=0;	
				//wake_up_interruptible(&dsp_queue); // ??????????????
			}
			pos=0;
		}
		
		//pos+=av3xx_mas_pio_write((void *) (cur+pos),60);
	}
	spin_unlock(&av_dsp_state_lock);
}

ssize_t av3xx_dsp_write(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	/*int i;
	
	for(i=0;i<count/1020;i++)
	{
		memcpy(tmp,buf+(i*1020),1020);
		if(!decRunning)
		{
			printk("not running\n");
			pos=0;
			nxt=tmp;
			tmp=cur;
			cur=nxt;
			nxt=NULL;
			decRunning=1;
			av3xx_dsp_interrupt(MAS_INTERRUPT,NULL,NULL);
		}
		else
		{
			nxt=tmp;
			interruptible_sleep_on(&dsp_queue);
			//tmp=nxt;
			nxt=NULL;
		}
	}
	*ppos+=count;*/
	return count;
}

int av3xx_dsp_read(struct file *filp, const char *buf, size_t count, loff_t *ppos)
{
	printk("dsp read called for %d",count);
	return count;
}

int av3xx_dsp_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{	
	int * val;
	struct av_peak * av_p;
	switch(cmd)
	{
		case AV_DSP_INI_MP3:
			data=(struct mp3_play *)arg;
			running=1;
			enable_irq(MAS_INTERRUPT);
			break;
		case AV_DSP_START_MP3:
			pos=0;
			av3xx_dsp_interrupt(MAS_INTERRUPT,NULL,NULL);
			break;
		case AV_DSP_STOP_MP3:
			last=1;
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


int ini_mp3_dec(void)
{
	int i;
	printk("Starting mp3 dec\n");
	av3xx_mas_write_codec(MAS_REG_AUDIO_CONF,MAS_INPUT_AD | MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
	                           | 0xf  << 4 // mic gain
				   | 0xf << 8  // adc gain right
				   | 0xf <<12  // adc gain left
				   );
	printk("[ini mp3] conf 1\n");
	av3xx_mas_write_codec(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_MONO);
	av3xx_mas_write_codec(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
	av3xx_mas_write_codec(MAS_REG_MIX_DSP_SCALE,0x40 << 8);
	av3xx_mas_write_codec(MAS_REG_DA_OUTPUT_MODE,0x0);
	printk("[ini mp3] conf 2\n");
	av3xx_mas_control_config(MAS_SET,MAS_BALANCE,50);
	av3xx_mas_control_config(MAS_SET,MAS_VOLUME,50);
	/*av3xx_mas_write_codec(MAS_REG_BALANCE,0x00<<8);
	av3xx_mas_write_codec(MAS_REG_VOLUME,0x60<<8);*/
	printk("[ini mp3] conf 3\n");
	for(i=0;i<20;i++) /* NOTHING */ ;
	
	av3xx_mas_app_select(MASS_APP_NONE);
	printk("[ini mp3] try stop\n");
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
	printk("[ini mp3] stop confirmed\n");
	av3xx_mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
	av3xx_mas_set_clk_speed(0x4800);
	av3xx_mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);
	printk("[ini mp3] config done\n");
	for(i=0;i<20;i++) /* NOTHING */ ;
	
	av3xx_mas_app_select(MASS_APP_MPEG3_DEC | MASS_APP_MPEG2_DEC);	
	printk("[ini mp3] try to start\n");
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
	//while(!av3xx_mas_app_running(MASS_APP_MPEG3_DEC) | !av3xx_mas_app_running(MASS_APP_MPEG2_DEC)) /* NOTHING */ ;
	printk("[ini mp3] start confirmed\n");
	
/*	cur=buff0;
	tmp=buff1;
	nxt=NULL;*/
	
	//enable_irq(MAS_INTERRUPT);
		
	printk("MAS configured for mp3 playing, waiting for data\n");
	
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

int av3xx_sound_init(void)
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

void av3xx_sound_exit(void)
{
	unregister_chrdev(SOUND_MAJOR, "av3xx_sound");
	free_irq(MAS_INTERRUPT,NULL);
}
