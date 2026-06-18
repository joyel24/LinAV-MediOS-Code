#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/io.h>
#include <linux/sched.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_cpld.h>
#include <asm/arch/av3xx_module.h>
#include <asm/arch/av3xx_FM.h>

#define MOD_DELAY      HZ

char * module_name[]= {
/* 0 */    "Unknown module",
/* 1 */    "MMC",
/* 2 */    "Unknown module",
/* 3 */    "SSFDC",
/* 4 */    "Unknown module",
/* 5 */    "MMC reader",
/* 6 */    "Unknown module",
/* 7 */    "SSFDC reader",
/* 8 */    "CFC",
/* 9 */    "Unknown module",
/* A */    "Video DVR",
/* B */    "Camera 3MG",
/* C */    "CFC reader",
/* D */    "Unknown module",
/* E */    "Unknown module",
/* F */    "NONE"
};

int connected_module=0;

struct timer_list av3xx_module_timer;

struct module_actions *actions_array[]={
/* 0 */    NULL,
/* 1 */    NULL,
/* 2 */    NULL,
/* 3 */    NULL,
/* 4 */    NULL,
/* 5 */    NULL,
/* 6 */    NULL,
/* 7 */    NULL,
/* 8 */    NULL,
/* 9 */    NULL,
/* A */    NULL,
/* B */    NULL,
/* C */    NULL,
/* D */    NULL,
/* E */    NULL,
/* F */    NULL
};

int av3xx_chk_module(unsigned long ptr)
{
    int res;
    if(!av3xx_FM_is_connected())
    {
        res=av3xx_get_module();
        if(res!=connected_module && res!= 0x00)
        {
            if(res==AV_MODULE_NONE)
                do_mod_disconnect(connected_module);
            else
                do_mod_connect(res);
            connected_module=res;
        }
    }        
    
    av3xx_module_timer.expires = jiffies + MOD_DELAY;
    add_timer(&av3xx_module_timer);
    return 0;
}

int av3xx_get_connected_module(void)
{
    return connected_module;
}   

void do_mod_disconnect(int module_num)
{
    if(module_num==AV_MODULE_NONE)
    {
        printk("Error in module driver wrong disconnect\n");
    }
    else
    {
        printk("Module disconnected: %s (0x%02x)\n",module_name[module_num],module_num);
        if(actions_array[module_num])
            actions_array[module_num]->do_disconnection();
    }
}

void do_mod_connect(int module_num)
{
    if(module_num==AV_MODULE_NONE)
    {
        printk("Error in module driver wrong connect\n");
    }
    else
    {
        printk("New module detected: %s (0x%02x)\n",module_name[module_num],module_num);
        if(actions_array[module_num])
            actions_array[module_num]->do_connection();
    }
}

int av3xx_module_register_action(struct module_actions * action_struct,int module_num)
{
    if(module_num<0 || module_num>0xF)
        return 0;
    
    if(actions_array[module_num])
        printk("WARNING trying to register module 0x%02x while it is already register\n",module_num);
        
    actions_array[module_num]=action_struct;
    return 1;
}

int av3xx_get_module(void)
{
    int res,res2;
    res=cpld_read(AV3XX_CPLD0);
    while((res2=(cpld_read(AV3XX_CPLD0)&0xF))!=res) /* wait for the value to become stable */
        res=res2;
    return (res&0xff);
}

static int __init av3xx_module_init(void)
{
    /* initial state */
    if(!av3xx_FM_is_connected())
    {
        connected_module=av3xx_get_module();        
    }
    else
        connected_module=AV_MODULE_NONE;
    
    if(actions_array[connected_module])
    {
        actions_array[connected_module]->do_connection();
    }
    
    /* setting up a timer to watch module state */
    init_timer(&av3xx_module_timer);
    av3xx_module_timer.function = av3xx_chk_module;
    av3xx_module_timer.expires = jiffies + MOD_DELAY; /* 1s timer */
    add_timer(&av3xx_module_timer);    

    /* everything is ok */
    printk("av3xx-module driver by oxygen77@free.fr, detect module %s (0x%02x)\n",module_name[connected_module],connected_module);
    return 0;
}

static void __exit av3xx_module_exit(void)
{
	/* remove timer ?? */
}

module_init(av3xx_module_init);
module_exit(av3xx_module_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Module driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
