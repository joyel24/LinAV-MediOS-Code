#include <asm/io.h>
#include <asm/irq.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_state.h>
#include <asm/arch/av3xx_common.h>
#include <asm/arch/av3xx_tsc2003.h>
#include <asm/arch/av3xx_rtc.h>

/** USB **/

int status;

int usbConnected(void)
{
	int val=inw(AV3XX_USB_STATE);
	return (val >> 0x6)&0x1;
}

int usbEnabled(void)
{
	/*int val=inw(AV3XX_USB_EN_DIS_ABLE);
	return val & 0x1;*/
	return status;
}

void setUsb(int state)
{
	int val;
	if(status!=state)
	{
		if(state==1)
			disable_irq(AV3XX_IDE_IRQ);

		val=inw(AV3XX_USB_EN_DIS_ABLE);
		val = (val & 0xFFFFFFFE) | val;
		outw(val,AV3XX_USB_EN_DIS_ABLE);

		AV3XX_POWER_UP_HDD
		AV3XX_SELECT_HDD

		if(state==0)
			enable_irq(AV3XX_IDE_IRQ);

		status=state;
	}
	else
	{
		if(state)
			printk("[usb state] warning: enabling usb while it is already enabled\n");
		else
			printk("[usb state] warning: disabling usb while it is already disabled\n");
	}
}

void forceDisableUsb(void)
{
	int val=inw(AV3XX_USB_EN_DIS_ABLE);
	val = (val & 0xFFFFFFFE);
	outw(val,AV3XX_USB_EN_DIS_ABLE);
}

void enableUsb(void)
{
	setUsb(1);
}

void disableUsb(void)
{
	setUsb(0);
}

int av3xx_usb_state_read(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
	len=sprintf(buf,"usb state:\n ");
	if(usbConnected())
		len += sprintf(buf+len,"\tcable connected\n");
	else
		len += sprintf(buf+len,"\tcable not connected\n");

	if(usbEnabled())
		len += sprintf(buf+len,"\tusb enable\n");
	else
		len += sprintf(buf+len,"\tusb disable\n");

	*eof = 1;
	return len;
}

/** Power **/

int powerConnected(void)
{
	int val=inw(AV3XX_POWER_STATE);
	return (val >> 0x5)&0x1;
}

int av3xx_power_state_read(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
	len=sprintf(buf,"power state:\n ");
	if(powerConnected())
		len += sprintf(buf+len,"\tcable connected\n");
	else
		len += sprintf(buf+len,"\tcable not connected\n");

	*eof = 1;
	return len;
}

/** time **/

int av3xx_time_read(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
	struct tm timeV ;

	av3xx_rtc_getTime(&timeV);

	len=sprintf(buf,"%02x:%02x:%02x - %02x/%02x/20%02x (wd:%02x,ms:%02x)\n ",
			timeV.tm_hour,timeV.tm_min,timeV.tm_sec,
			timeV.tm_mday,timeV.tm_mon,timeV.tm_year,
			timeV.tm_wday,timeV.tm_ms);
	*eof = 1;
	return len;
}

/** tsc2003 **/

int av3xx_tsc2003_read(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

	len=sprintf(buf,"[00] B0: %x, B1: %x, T0: %x, T1: %x; IN0: %x, IN1: %x\n",
			av3xx_tsc2003getVal(CMD_BAT0|INTERNAL_ON),
			av3xx_tsc2003getVal(CMD_BAT1|INTERNAL_ON),
			av3xx_tsc2003getVal(CMD_TEMP0|INTERNAL_ON),
			av3xx_tsc2003getVal(CMD_TEMP1|INTERNAL_ON),
			av3xx_tsc2003getVal(CMD_IN0|INTERNAL_ON),
			av3xx_tsc2003getVal(CMD_IN1|INTERNAL_ON));
	*eof = 1;
	return len;
}

/** General functions**/

int av_state_dev_count[AV_STATE_NR_MINOR] ;
spinlock_t av_state_dev_lock;

int av_state_open(struct inode *inode, struct file *filp)
{
	if(MINOR(inode->i_rdev) < AV_STATE_NR_MINOR)
	{
		spin_lock(&av_state_dev_lock);
		if (av_state_dev_count[MINOR(inode->i_rdev)]) {
			spin_unlock(&av_state_dev_lock);
			return -EBUSY; 
		}
		av_state_dev_count[MINOR(inode->i_rdev)]++;
		spin_unlock(&av_state_dev_lock);
	}
	else
		return -ENODEV;
	return 0;
}

int av_state_release(struct inode *inode, struct file *filp)
{
	if(MINOR(inode->i_rdev) < AV_STATE_NR_MINOR)
	{
		av_state_dev_count[MINOR(inode->i_rdev)]--;
	}
	else
		return -ENODEV;
	return 0;
}

int av_state_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	int * ptr=arg;

	if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC) return -ENOTTY;
    	if (_IOC_NR(cmd) > AV_OP_IOC_MAXNR) return -ENOTTY;

	switch(cmd) {
		case AV_USB_IOC_STATE:
			*ptr=usbConnected();
			break;

		case AV_USB_IOC_ENABLE:
			enableUsb();
			break;

		case AV_USB_IOC_DISABLE:
			disableUsb();
			break;

		case AV_POWER_IOC_STATE:
			*ptr=powerConnected();
			break;

		case AV_LEVEL_BAT0_IOC:
			*ptr=av3xx_tsc2003getVal(CMD_BAT0|INTERNAL_ON);
			break;
		case AV_LEVEL_BAT1_IOC:
			*ptr=av3xx_tsc2003getVal(CMD_BAT1|INTERNAL_ON);
			break;
		case AV_LEVEL_TEMP0_IOC:
			*ptr=av3xx_tsc2003getVal(CMD_TEMP0|INTERNAL_ON);
			break;
		case AV_LEVEL_TEMP1_IOC:
			*ptr=av3xx_tsc2003getVal(CMD_TEMP1|INTERNAL_ON);
			break;
		case AV_LEVEL_IN0_IOC:
			*ptr=av3xx_tsc2003getVal(CMD_IN0|INTERNAL_ON);
			break;
		case AV_LEVEL_IN1_IOC:
			*ptr=av3xx_tsc2003getVal(CMD_IN1|INTERNAL_ON);
			break;
		case AV_RTC_GET_TIME_IOC:
			return av3xx_rtc_getTime(ptr);
		case AV_RTC_SET_TIME_IOC:
			return av3xx_rtc_setTime(ptr);
		case AV_GET_MOUSE_IOC:
			return av3xx_button_get_mouse(ptr);
		case AV_SET_MOUSE_IOC:
			return av3xx_button_set_mouse(ptr);
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations avop_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_state_ioctl,
    open:       av_state_open,
    release:    av_state_release,
};

int av3xx_state_init(void)
{
	/* dev entry setup */
	int result,i;
	struct proc_dir_entry * avop_proc;
	result = register_chrdev(AV_STATE_MAJOR, "avop", &avop_fops);
	if (result < 0)
	{
        	printk(KERN_WARNING "av3xx_state : can't get a major number %d\n",AV_STATE_MAJOR);
        	return result;
    	}
	spin_lock_init(&av_state_dev_lock);
	for(i=0;i<AV_STATE_NR_MINOR;i++)
		av_state_dev_count[i]=0;
	/* usb ini*/
	forceDisableUsb();
	status=0;

	/* power ini */
	/***** nothing to do *****/

	/* time ini */
	av3xx_rtc_init();

	/* tsc2003 ini */

	/***** need to be done *****/

	/* proc entries setup */
	avop_proc=proc_mkdir("avop",0);
	create_proc_read_entry("usb", 0, avop_proc, av3xx_usb_state_read, NULL);
	create_proc_read_entry("power", 0, avop_proc, av3xx_power_state_read, NULL);
	create_proc_read_entry("time", 0, avop_proc, av3xx_time_read, NULL);
	create_proc_read_entry("bat", 0, avop_proc, av3xx_tsc2003_read, NULL);

	/* everything is ok */
	printk("[Av3xx init] state driver\n");
    return 0;
}
