#include <asm/io.h>
#include <asm/irq.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_common.h>
#include <asm/arch/av3xx_state.h>
#include <asm/arch/av3xx_buttons.h>
#include <asm/arch/av3xx_tsc2003.h>
#include <asm/arch/av3xx_rtc.h>

int av_usb_dev_count,av_power_dev_count,av_rtc_dev_count,av_tsc_dev_count,av_mouse_dev_count;
spinlock_t av_usb_dev_lock,av_power_dev_lock,av_rtc_dev_lock,av_tsc_dev_lock,av_mouse_dev_lock;

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

int av_usb_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_usb_dev_lock);
	if (av_usb_dev_count) {
		spin_unlock(&av_usb_dev_lock);
		return -EBUSY; 
	}
	av_usb_dev_count++;
	spin_unlock(&av_usb_dev_lock);	
	return 0;
}

int av_usb_release(struct inode *inode, struct file *filp)
{
	av_usb_dev_count--;
	return 0;
}

int av_usb_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	int * ptr=(int *)arg;
	printk("in av_usb_ioctl\n");
	if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC) return -ENOTTY;
	
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
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations av_usb_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_usb_ioctl,
    open:       av_usb_open,
    release:    av_usb_release,
};

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

int av_power_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_power_dev_lock);
	if (av_power_dev_count) {
		spin_unlock(&av_power_dev_lock);
		return -EBUSY; 
	}
	av_power_dev_count++;
	spin_unlock(&av_power_dev_lock);	
	return 0;
}

int av_power_release(struct inode *inode, struct file *filp)
{
	av_power_dev_count--;
	return 0;
}

int av_power_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	int * ptr=(int *)arg;
	printk("in av_power_ioctl\n");
	if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC) return -ENOTTY;	 
	
	switch(cmd) {
		case AV_POWER_IOC_STATE:
			*ptr=powerConnected();
			break;		
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations av_power_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_power_ioctl,
    open:       av_power_open,
    release:    av_power_release,
};

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

int av_rtc_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_rtc_dev_lock);
	if (av_rtc_dev_count) {
		spin_unlock(&av_rtc_dev_lock);
		return -EBUSY; 
	}
	av_rtc_dev_count++;
	spin_unlock(&av_rtc_dev_lock);	
	return 0;
}

int av_rtc_release(struct inode *inode, struct file *filp)
{
	av_rtc_dev_count--;
	return 0;
}

int av_rtc_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	struct tm * ptrTm=(struct tm * )arg;
	printk("in av_rtc_ioctl\n");
	if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC) return -ENOTTY;	 
	
	switch(cmd) {
		case AV_RTC_GET_TIME_IOC:
			return av3xx_rtc_getTime(ptrTm);
		case AV_RTC_SET_TIME_IOC:
			return av3xx_rtc_setTime(ptrTm);
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations av_rtc_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_rtc_ioctl,
    open:       av_rtc_open,
    release:    av_rtc_release,
};

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

int av_tsc_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_tsc_dev_lock);
	if (av_tsc_dev_count) {
		spin_unlock(&av_tsc_dev_lock);
		return -EBUSY; 
	}
	av_tsc_dev_count++;
	spin_unlock(&av_tsc_dev_lock);	
	return 0;
}

int av_tsc_release(struct inode *inode, struct file *filp)
{
	av_tsc_dev_count--;
	return 0;
}

int av_tsc_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	int * ptr=(int *)arg;
	
	printk("in av_tsc_ioctl\n");
	
	if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC) return -ENOTTY;	 
	
	switch(cmd) {
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
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations av_tsc_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_tsc_ioctl,
    open:       av_tsc_open,
    release:    av_tsc_release,
};

/** mouse **/

int av_mouse_open(struct inode *inode, struct file *filp)
{
	/*spin_lock(&av_mouse_dev_lock);
	if (av_mouse_dev_count) {
		spin_unlock(&av_mouse_dev_lock);
		return -EBUSY; 
	}
	av_mouse_dev_count++;
	spin_unlock(&av_mouse_dev_lock);*/	
	return 0;
}

int av_mouse_release(struct inode *inode, struct file *filp)
{
	/*av_mouse_dev_count--;*/
	return 0;
}

int av_mouse_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	struct mouseParam * ptrParam=(struct mouseParam *)arg;
	struct av3xx_pos * ptrPos=(struct av3xx_pos *)arg;
	printk("in av_mouse_ioctl\n");
	if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC) return -ENOTTY;	 
	
	switch(cmd) {
		case AV_GET_MOUSE_IOC:
			return av3xx_button_get_mouse(ptrPos);
		case AV_SET_MOUSE_IOC:
			return av3xx_button_set_mouse(ptrPos);
		case AV_SET_MOUSE_PARAM:
			return av3xx_button_set_mouse_param(ptrParam);
		case AV_GET_MOUSE_PARAM:
			return av3xx_button_get_mouse_param(ptrParam);
		default:
			return -ENOTTY;
	}
	return 0;
}

struct file_operations av_mouse_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_mouse_ioctl,
    open:       av_mouse_open,
    release:    av_mouse_release,
};

/** General functions**/

struct file_operations * av_fops_array[] = {
	&av_rtc_fops,
	&av_power_fops,	
	&av_tsc_fops,
	&av_usb_fops,
	&av_mouse_fops
};

int av_state_open(struct inode *inode, struct file *filp)
{
	int type=MINOR(inode->i_rdev);
	if(type < AV_STATE_NR_MINOR)
	{
		filp->f_op=av_fops_array[type];
		return filp->f_op->open(inode,filp);
	}
	else
	{
		printk("wrong minor\n");
		return -ENODEV;
	}
	return 0;
}

// shouldn't be call
int av_state_release(struct inode *inode, struct file *filp)
{
	return -ENODEV;
}

// ioctl should be handle by seperate functions, one per minor, 
// so nothing to do here
int av_state_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	printk("ERROR STATE_IOCTL CALLED !!!\n");
	return -ENOTTY;
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
	int result;
	struct proc_dir_entry * avop_proc;
	result = register_chrdev(AV_STATE_MAJOR, "avop", &avop_fops);
	if (result < 0)
	{
        	printk(KERN_WARNING "av3xx_state : can't get a major number %d\n",AV_STATE_MAJOR);
        	return result;
    	}
	
	spin_lock_init(&av_usb_dev_lock);
	spin_lock_init(&av_upower_dev_lock);
	spin_lock_init(&av_rtc_dev_lock);
	spin_lock_init(&av_tsc_dev_lock);
	spin_lock_init(&av_mouse_dev_lock);
	
	av_usb_dev_count=0;
	av_power_dev_count=0;
	av_rtc_dev_count=0;
	av_tsc_dev_count=0;
	av_mouse_dev_count=0;
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
