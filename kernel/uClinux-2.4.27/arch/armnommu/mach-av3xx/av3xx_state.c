#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

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
#include <asm/arch/av3xx_cpld.h>
#include <asm/arch/av3xx_gio.h>
#include <asm/arch/av3xx_ata_ide.h>
#include <asm/arch/av3xx_FM.h>

int av_dev_count[AV_STATE_NR_MINOR];
spinlock_t av_dev_lock[AV_STATE_NR_MINOR];

/** USB **/

int status;

int usbConnected(void)
{
    int val=inw(AV3XX_USB_STATE);
    return (val >> 0x6)&0x1;
}

int usbEnabled(void)
{
    return status;
}

void setUsb(int state)
{
    if(status!=state)
    {
        if(state==1)
        {
            cpld_set_port_1(AV3XX_CPLD_USB);
        }
        else
        {
            cpld_clear_port_1(AV3XX_CPLD_USB);
        }
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
    cpld_clear_port_1(AV3XX_CPLD_USB);
}

void enableUsb(void)
{
    setUsb(1);
}

void disableUsb(void)
{
    setUsb(0);
}

int av3xx_usb_state_read(char *buf, char **start, off_t offset,int len, int *eof, void *data)
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
    spin_lock(&av_dev_lock[AV_STATE_USB_MINOR]);
    if (av_dev_count[AV_STATE_USB_MINOR]) {
            spin_unlock(&av_dev_lock[AV_STATE_USB_MINOR]);
            return -EBUSY; 
    }
    av_dev_count[AV_STATE_USB_MINOR]++;
    spin_unlock(&av_dev_lock[AV_STATE_USB_MINOR]);	
    return 0;
}

int av_usb_release(struct inode *inode, struct file *filp)
{
    av_dev_count[AV_STATE_USB_MINOR]--;
    return 0;
}

int av_usb_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    int * ptr=(int *)arg;
    if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC_USB) return -ENOTTY;
    if(_IOC_NR(cmd)<1 || _IOC_NR(cmd) > AV_OP_IOC_MAXNR_USB) return -ENOTTY;
    
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

int av3xx_power_state_read(char *buf, char **start, off_t offset,int len, int *eof, void *data)
{
    len=sprintf(buf,"power state:\n ");
    if(powerConnected())
        len += sprintf(buf+len,"\tcable connected\n");
    else
        len += sprintf(buf+len,"\tcable not connected\n");
    len+=sprintf(buf,"[00] B0: %x, B1: %x, T0: %x, T1: %x; IN0: %x, IN1: %x\n",
                    av3xx_tsc2003getVal(CMD_BAT0|INTERNAL_ON),
                    av3xx_tsc2003getVal(CMD_BAT1|INTERNAL_ON),
                    av3xx_tsc2003getVal(CMD_TEMP0|INTERNAL_ON),
                    av3xx_tsc2003getVal(CMD_TEMP1|INTERNAL_ON),
                    av3xx_tsc2003getVal(CMD_IN0|INTERNAL_ON),
                    av3xx_tsc2003getVal(CMD_IN1|INTERNAL_ON));
    *eof = 1;
    return len;
}

int av_power_open(struct inode *inode, struct file *filp)
{
    spin_lock(&av_dev_lock[AV_STATE_POWER_MINOR]);
    if (av_dev_count[AV_STATE_POWER_MINOR]) {
        spin_unlock(&av_dev_lock[AV_STATE_POWER_MINOR]);
        return -EBUSY; 
    }
    av_dev_count[AV_STATE_POWER_MINOR]++;
    spin_unlock(&av_dev_lock[AV_STATE_POWER_MINOR]);	
    return 0;
}

int av_power_release(struct inode *inode, struct file *filp)
{
    av_dev_count[AV_STATE_POWER_MINOR]--;
    return 0;
}

int av_power_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    int * ptr=(int *)arg;
    if (_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC_POWER) return -ENOTTY;	 
    if(_IOC_NR(cmd)<1 || _IOC_NR(cmd) > AV_OP_IOC_MAXNR_POWER) return -ENOTTY;
    
    switch(cmd) {
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

int av3xx_time_read(char *buf, char **start, off_t offset,int len, int *eof, void *data)
{
    struct av_tm timeV ;
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
    spin_lock(&av_dev_lock[AV_STATE_RTC_MINOR]);
    if (av_dev_count[AV_STATE_RTC_MINOR]) {
        spin_unlock(&av_dev_lock[AV_STATE_RTC_MINOR]);
        return -EBUSY; 
    }
    av_dev_count[AV_STATE_RTC_MINOR]++;
    spin_unlock(&av_dev_lock[AV_STATE_RTC_MINOR]);	
    return 0;
}

int av_rtc_release(struct inode *inode, struct file *filp)
{
    av_dev_count[AV_STATE_RTC_MINOR]--;
    return 0;
}

int av_rtc_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    struct av_tm * ptrTm=(struct av_tm * )arg;
    int * val=(int*)arg;
    if(_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC_RTC) return -ENOTTY;
    if(_IOC_NR(cmd)<1 || _IOC_NR(cmd) > AV_OP_IOC_MAXNR_RTC) return -ENOTTY;
    
    switch(cmd) {
        case AV_RTC_GET_TIME_IOC:
            return av3xx_rtc_getTime(ptrTm);
        case AV_RTC_SET_TIME_IOC:
            return av3xx_rtc_setTime(ptrTm);
        case AV_RTC_GET_JIFFY_IOC:
            *val=jiffies;
            break;
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



/** mouse **/
//#define AV_MOUSE_ONE_CALL 1

int av_mouse_open(struct inode *inode, struct file *filp)
{
#ifdef AV_MOUSE_ONE_CALL
    spin_lock(&av_dev_lock[AV_STATE_MOUSE_MINOR]);
    if (av_dev_count[AV_STATE_MOUSE_MINOR]) {
        spin_unlock(&av_dev_lock[AV_STATE_MOUSE_MINOR]);
        return -EBUSY; 
    }
    av_dev_count[AV_STATE_MOUSE_MINOR]++;
    spin_unlock(&av_dev_lock[AV_STATE_MOUSE_MINOR]);
#endif
    return 0;
}

int av_mouse_release(struct inode *inode, struct file *filp)
{
#ifdef AV_MOUSE_ONE_CALL
    av_dev_count[AV_STATE_MOUSE_MINOR]--;
#endif
    return 0;
}

extern void av_halt_system(void);

int av_mouse_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
    struct mouseParam * ptrParam=(struct mouseParam *)arg;
    struct av3xx_pos * ptrPos=(struct av3xx_pos *)arg;
    int * ptr=(int *)arg;
    if(_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC_EVT) return -ENOTTY;
    if(_IOC_NR(cmd)<1 || _IOC_NR(cmd) > AV_OP_IOC_MAXNR_EVT) return -ENOTTY;
    
    switch(cmd) {
            case AV_GET_MOUSE_IOC:
                return av3xx_button_get_mouse(ptrPos);
            case AV_SET_MOUSE_IOC:
                return av3xx_button_set_mouse(ptrPos);
            case AV_SET_MOUSE_PARAM:
                return av3xx_button_set_mouse_param(ptrParam);
            case AV_GET_MOUSE_PARAM:
                return av3xx_button_get_mouse_param(ptrParam);
            case AV_GET_EVENT:
                *ptr=av3xx_get_event();
                break;
            case AV_CLEAR_EVENTS:
                av3xx_clear_buffer();
                break;
            case AV_WAIT_EVENT:
                *ptr=av3xx_wait_event();
                break;
            case AV_SET_TIMER_FREQ:
                av3xx_set_timer_freq(*ptr);
                break;
            case AV_START_TIMER:
                av3xx_start_timer();
                break;
            case AV_STOP_TIMER:
                av3xx_stop_timer();
                break;
            case AV_TIMER_STATE :
                *ptr=av3xx_timer_state();
                break;
            case AV_DO_WAKEUP :
                av3xx_wakeup_evt();
                break;            
            case AV_PAUSE_APP :
                av3xx_do_pause();
                break;
            case AV_RELEASE_APP :
                av3xx_release_app();
                break;
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

/* lcd on off functions */

int av3xx_lcd_state=1;
int av3xx_lcd_timer_used[2]={1,0};
int lcd_freq_rep[2]={LCD_FREQ_DEFAULT_0,LCD_FREQ_DEFAULT_1};
struct timer_list av3xx_lcdOnOff_timer;

int av3xx_halt_timer_used[2]={1,0};
int halt_freq_rep[2]={HALT_FREQ_DEFAULT_0,HALT_FREQ_DEFAULT_1};
struct timer_list av3xx_halt_timer;

void av3xx_lcd_set_state(int state)
{
    if(state!=av3xx_lcd_get_state())
    {
        av3xx_lcd_state=state;
        if(state) /* turn on */
        {
            cpld_set_port_2(AV3XX_CPLD_LCD);
            av3xx_gio_clear(AV3XX_GIO_LCD_BACKLIGHT);
            printk("Turning on lcd\n");
        }
        else
        {
            cpld_clear_port_2(AV3XX_CPLD_LCD);
            av3xx_gio_set(AV3XX_GIO_LCD_BACKLIGHT);
            printk("Turning off lcd\n");
        }
   }
}

int av3xx_lcd_get_state(void)
{
    return av3xx_lcd_state;
}

void av3xx_lcd_off(void)
{
    av3xx_lcd_set_state(0);    
}

void av3xx_lcd_on(void)
{
    av3xx_lcd_set_state(1);    
}

void av3xx_lcd_launchTimer(void)
{
    int num=getCurrentTimer();
    if(lcd_freq_rep[num]!=0 && av3xx_lcd_timer_used[num] && av3xx_lcd_state)
    {
        del_timer(&av3xx_lcdOnOff_timer);
        av3xx_lcdOnOff_timer.expires = jiffies + (lcd_freq_rep[num]*HZ); /* lcd_freq_rep in sec */
        add_timer(&av3xx_lcdOnOff_timer);
    }
}

void av3xx_lcd_timer_action(unsigned long ptr)
{
    int num=getCurrentTimer();
    if(av3xx_lcd_timer_used[num])
    {
        av3xx_lcd_off();
        if(av3xx_FM_is_connected())
            av3xx_FM_lightsOFF();        
    }
}

void av3xx_halt_launchTimer(void)
{
    int num=getCurrentTimer();
    if(halt_freq_rep[num]!=0 && av3xx_halt_timer_used[num])
    {
        del_timer(&av3xx_halt_timer);
        av3xx_halt_timer.expires = jiffies + (halt_freq_rep[num]*HZ); /* lcd_freq_rep in sec */
        add_timer(&av3xx_halt_timer);
    }
}

void av3xx_halt_timer_action(unsigned long ptr)
{
    int num=getCurrentTimer();
    if(av3xx_halt_timer_used[num])
        av_halt_system();
}

void av3xx_lcd_keyPress(void)
{
    int num=getCurrentTimer();
    if(av3xx_lcd_timer_used[num])
    {
        av3xx_lcd_on();
        if(av3xx_FM_is_connected())
            av3xx_FM_lightsON();
        av3xx_lcd_launchTimer();        
    }
    
    av3xx_halt_launchTimer();
}

int av_state_sate_open(struct inode *inode, struct file *filp)
{
    spin_lock(&av_dev_lock[AV_STATE_STATE_MINOR]);
    if (av_dev_count[AV_STATE_STATE_MINOR])
    {
        spin_unlock(&av_dev_lock[AV_STATE_STATE_MINOR]);
        return -EBUSY; 
    }
    av_dev_count[AV_STATE_STATE_MINOR]++;
    spin_unlock(&av_dev_lock[AV_STATE_STATE_MINOR]);	
    return 0;
}

int av_state_sate_release(struct inode *inode, struct file *filp)
{
    av_dev_count[AV_STATE_STATE_MINOR]--;
    return 0;
}

extern struct timer_list av3xx_hd_timer;
//extern int av3xx_hd_timer_used[2];
void av3xx_hd_launchTimer(void);
extern int hd_freq_rep[2];
void av3xx_hd_timer_on(int num);
void av3xx_hd_timer_off(int num);
int av3xx_hd_timer_state(int num);

void chgTimer()
{
    del_timer(&av3xx_halt_timer);
    del_timer(&av3xx_lcdOnOff_timer);
    del_timer(&av3xx_hd_timer);
    
    av3xx_halt_launchTimer();
    av3xx_lcd_launchTimer();
    av3xx_hd_launchTimer();
}

int getCurrentTimer(void)
{
    if(powerConnected())
        return AV_TIMER_ON_DC;
    else
        return AV_TIMER_ON_BAT;
}

int av_state_sate_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    struct timer_val * ptr=(struct timer_val *)arg;
    int * val=(int*)arg;
    if(_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC_STATE) return -ENOTTY;
    if(_IOC_NR(cmd)<1 || _IOC_NR(cmd) > AV_OP_IOC_MAXNR_STATE) return -ENOTTY;
    
    switch(cmd)
    {
        case AV_LCD_GET_TIMOUT:
            ptr->val=lcd_freq_rep[ptr->num];
            break;
        case AV_LCD_SET_TIMOUT:
            lcd_freq_rep[ptr->num]=ptr->val;
            av3xx_lcd_launchTimer();
            break;
        case AV_LCD_TIMOUT_OFF:
            del_timer(&av3xx_lcdOnOff_timer);            
            av3xx_lcd_timer_used[ptr->num]=0;
            break;
        case AV_LCD_TIMOUT_ON:
            av3xx_lcd_timer_used[ptr->num]=1;
            av3xx_lcd_launchTimer();
            break;
        case AV_LCD_TIMOUT_STATE:
            ptr->val=av3xx_lcd_timer_used[ptr->num];
            break;
        case AV_LCD_ON:
            av3xx_lcd_on();
            av3xx_lcd_launchTimer();
            break;
        case AV_LCD_OFF:
            av3xx_lcd_off();
            del_timer(&av3xx_lcdOnOff_timer);
            break;
        case AV_LCD_GET_STATE:
            *val=av3xx_lcd_get_state();
            break;
            
        case AV_HD_GET_TIMOUT:
            ptr->val=hd_freq_rep[ptr->num];
            break;
        case AV_HD_SET_TIMOUT:
            hd_freq_rep[ptr->num]=ptr->val;
            av3xx_hd_launchTimer();
            break;
        case AV_HD_TIMOUT_OFF:
            av3xx_hd_timer_off(ptr->num);
            break;
        case AV_HD_TIMOUT_ON:
            av3xx_hd_timer_on(ptr->num);
            break;            
        case AV_HD_TIMOUT_STATE:
            ptr->val=av3xx_hd_timer_state(ptr->num);
            break;            
        case AV_HD_STOP:
            av_ata_stop_HD();
            break;
            
        case AV_HALT_GET_TIMOUT:
            ptr->val=halt_freq_rep[ptr->num];
            break;
        case AV_HALT_SET_TIMOUT:
            halt_freq_rep[ptr->num]=ptr->val;
            av3xx_halt_launchTimer();
            break;
        case AV_HALT_TIMOUT_OFF:
            del_timer(&av3xx_halt_timer);            
            av3xx_halt_timer_used[ptr->num]=0;
            break;
        case AV_HALT_TIMOUT_ON:
            av3xx_halt_timer_used[ptr->num]=1;
            av3xx_halt_launchTimer();
            break;
        case AV_HALT_TIMOUT_STATE:
            ptr->val=av3xx_halt_timer_used[ptr->num];
            break;
        case AV_HALT_DEVICE :
            av_halt_system();
            break;
        default:            
            return -ENOTTY;
    }
    return 0;
}

struct file_operations av_state_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_state_sate_ioctl,
    open:       av_state_sate_open,
    release:    av_state_sate_release
};

/** FM functions **/

int av_FM_ctl_open(struct inode *inode, struct file *filp)
{
	spin_lock(&av_dev_lock[AV_STATE_FM_CTL_MINOR]);
	if (av_dev_count[AV_STATE_FM_CTL_MINOR]) {
		spin_unlock(&av_dev_lock[AV_STATE_FM_CTL_MINOR]);
		return -EBUSY; 
	}
	av_dev_count[AV_STATE_FM_CTL_MINOR]++;
	spin_unlock(&av_dev_lock[AV_STATE_FM_CTL_MINOR]);	
	return 0;
}

int av_FM_ctl_release(struct inode *inode, struct file *filp)
{
	av_dev_count[AV_STATE_FM_CTL_MINOR]--;
	return 0;
}

int av_FM_ctl_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    int * ptr=(int*)arg;
    char * str=(char*)arg;
    
    if(_IOC_TYPE(cmd) != AV_OP_IOC_MAGIC_FM) return -ENOTTY;
    if(_IOC_NR(cmd)<1 || _IOC_NR(cmd) > AV_OP_IOC_MAXNR_FM) return -ENOTTY;
    
    switch(cmd) {
        case AV_FM_SET_TXT:
            av3xx_FM_putText(str);
            break;
        case AV_FM_GET_TXT:
            av3xx_FM_getText(str);
            break;
        case AV_FM_SCROLL_ON:
            av3xx_FM_setScroll(1);
            break;
        case AV_FM_SCROLL_OFF:
            av3xx_FM_setScroll(0);
            break;
        case AV_FM_GET_SCROLL:
            *ptr=av3xx_FM_getScroll();
            break;
        case AV_FM_ON_ICONS:
                if(*ptr!=FM_BAT && *ptr!=FM_VOL)
                    av3xx_FM_setIcon(*ptr,1);
            break;
        case AV_FM_OFF_ICONS:
                if(*ptr!=FM_BAT && *ptr!=FM_VOL)
                   av3xx_FM_setIcon(*ptr,0);
            break;
        case AV_FM_GET_ICONS:
            if(*ptr!=FM_BAT && *ptr!=FM_VOL)
                   *ptr=av3xx_FM_getIcon(*ptr);
            break;
        case AV_FM_SET_BAT:
            av3xx_FM_setIcon(FM_BAT,*ptr);
            break;
        case AV_FM_GET_BAT:
            *ptr=av3xx_FM_getIcon(FM_BAT);
            break;
        case AV_FM_SET_VOL:
            av3xx_FM_setIcon(FM_VOL,*ptr);
            break;
        case AV_FM_GET_VOL:
            *ptr=av3xx_FM_getIcon(FM_VOL);
            break;
        case AV_FM_IS_CONNECTED:
            *ptr=av3xx_FM_is_connected();
            break;
        case AV_FM_INI_TXT:
            av3xx_FM_put_iniTxt();
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

struct file_operations av_FM_ctl_fops = {
    llseek:     NULL,
    read:       NULL,
    write:      NULL,
    ioctl:      av_FM_ctl_ioctl,
    open:       av_FM_ctl_open,
    release:    av_FM_ctl_release,
};

/** General functions **/

struct file_operations * av_fops_array[] = {
	&av_rtc_fops,
	&av_power_fops,	
	&av_power_fops,
	&av_usb_fops,
	&av_mouse_fops,
        &av_state_fops,
        &av_FM_ctl_fops
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

static int __init av3xx_state_init(void)
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
        
	for(i=0;i<AV_STATE_NR_MINOR;i++)
        {
            spin_lock_init(&av_dev_lock[i]);
            av_dev_count[i]=0;
        }
	
	/* usb ini*/
	forceDisableUsb();
	status=0;

	/* power ini */
	/***** nothing to do *****/
        /***** tsc ini ??    *****/

	/* time ini */
	av3xx_rtc_init();

	/* lcd on/off timer */
                
        init_timer(&av3xx_lcdOnOff_timer);
        av3xx_lcdOnOff_timer.function = av3xx_lcd_timer_action;
        
        init_timer(&av3xx_halt_timer);
        av3xx_halt_timer.function = av3xx_halt_timer_action;            
        
        av3xx_lcd_launchTimer();
        av3xx_halt_launchTimer();
        
        av3xx_gio_dir(AV3XX_GIO_LCD_BACKLIGHT,GIO_OUT);
        
        /* proc entries setup */
        avop_proc=proc_mkdir("avop",0);
        create_proc_read_entry("usb", 0, avop_proc, av3xx_usb_state_read, NULL);
        create_proc_read_entry("power", 0, avop_proc, av3xx_power_state_read, NULL);
        create_proc_read_entry("time", 0, avop_proc, av3xx_time_read, NULL);
        create_proc_read_entry("bat", 0, avop_proc, av3xx_power_state_read, NULL);
        
        /* everything is ok */
        printk("av3xx-state driver by oxygen77@free.fr\n");
    return 0;
}

static void __exit av3xx_state_exit(void)
{
	
}

module_init(av3xx_state_init);
module_exit(av3xx_state_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("General state driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
