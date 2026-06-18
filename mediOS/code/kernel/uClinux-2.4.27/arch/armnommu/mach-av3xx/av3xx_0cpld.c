#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/io.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_cpld.h>

spinlock_t av3xx_cpld_lock,av3xx_cpldState_lock;

static cpld_port_state[4]=
{
    0x0,
    0x0,
    0x4,
    0x8,
};

int cpld_port_array[4] = {
    AV3XX_CPLD_PORT0,
    AV3XX_CPLD_PORT1,
    AV3XX_CPLD_PORT2,
    AV3XX_CPLD_PORT3
};

void cpld_chg_state(int cpld_port,int bit_num,int direction)
{
    if(direction)
        cpld_port_state[cpld_port] |= (0x1 << bit_num);
    else
        cpld_port_state[cpld_port] &= ~(0x1 << bit_num);
        
    outw(cpld_port_state[cpld_port],cpld_port_array[cpld_port]);
}

int cpld_read(int cpld_port)
{
    int val;
    spin_lock(&av3xx_cpld_lock);
    val=inw(cpld_port_array[cpld_port]);
    spin_unlock(&av3xx_cpld_lock);
    return val;
}

void cpld_select(int bit_num,int direction)
{
    int val;
    spin_lock(&av3xx_cpldState_lock);
    if(direction)
        val = cpld_port_state[AV3XX_CPLD0] | (0x1 << bit_num);
    else
        val = cpld_port_state[AV3XX_CPLD0] & ~(0x1 << bit_num);
    if(val != cpld_port_state[AV3XX_CPLD0])
    {
        cpld_port_state[AV3XX_CPLD0]=val;
        cpld_do_select();
    }
    spin_unlock(&av3xx_cpldState_lock);
}

void cpld_do_select(void)
{  
    int res,res2;
    spin_lock(&av3xx_cpld_lock);
    //printk("changing state : %d\n",cpld_port_state[AV3XX_CPLD0]);
    outw(cpld_port_state[AV3XX_CPLD0],AV3XX_CPLD_PORT0);
    outw(cpld_port_state[AV3XX_CPLD0],AV3XX_CPLD_PORT0);
    outw(cpld_port_state[AV3XX_CPLD0],AV3XX_CPLD_PORT0);
    outw(cpld_port_state[AV3XX_CPLD0],AV3XX_CPLD_PORT0);
    outw(cpld_port_state[AV3XX_CPLD0],AV3XX_CPLD_PORT0);
    
    res=inw(cpld_port_array[AV3XX_CPLD0]);
    while((res2=inw(cpld_port_array[AV3XX_CPLD0]))!=res) /* wait for the value to become stable */
        res=res2;
    spin_unlock(&av3xx_cpld_lock);
}

static int __init av3xx_cpld_init(void)
{
    int version;
    
    spin_lock_init(&av3xx_cpld_lock);
    spin_lock_init(&av3xx_cpldState_lock);
    
    cpld_do_select(); 
    outw(cpld_port_state[AV3XX_CPLD1],AV3XX_CPLD_PORT1);
    outw(cpld_port_state[AV3XX_CPLD2],AV3XX_CPLD_PORT2);
    outw(cpld_port_state[AV3XX_CPLD3],AV3XX_CPLD_PORT3);
    
    version=(cpld_read(AV3XX_CPLD2) & 0x000F) | ((cpld_read(AV3XX_CPLD3) & 0x0007)<<4);
    
    /* everything is ok */
    printk("av3xx-cpld driver by oxygen77@free.fr CPLD Ver:0x%x\n",version);
    return 0;
}

static void __exit av3xx_cpld_exit(void)
{	
}

module_init(av3xx_cpld_init);
module_exit(av3xx_cpld_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Cpld driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
