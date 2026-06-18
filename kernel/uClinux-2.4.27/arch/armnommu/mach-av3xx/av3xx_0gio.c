#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/io.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_gio.h>

void av3xx_gio_chg(int gio_num,int direction,int base_addr_0,int base_addr_1,int max_gpio,int sav)
{
    int addr;
    if(gio_num>=0 && gio_num<max_gpio)
    {       
        if(gio_num < 16)
        {
            addr = base_addr_0;
        }
        else
        {
            addr = base_addr_1; 
            gio_num -= 16;  
        }
        
        if(sav)
            outw(direction?inw(addr)| (0x1 << gio_num) : inw(addr) & ~(0x1 << gio_num),addr);
        else
            outw((0x1 << gio_num),addr); 
    }
}

void av3xx_gio_dir(int gio_num,int direction)
{
    av3xx_gio_chg(gio_num,direction,AV3XX_GIO_DIRECTION0,AV3XX_GIO_DIRECTION1,32,1);        
}

void av3xx_gio_inv(int gio_num,int direction)
{
    av3xx_gio_chg(gio_num,direction,AV3XX_GIO_INVERT0,AV3XX_GIO_INVERT1,32,1);        
}

void av3xx_gio_set(int gio_num)
{
    av3xx_gio_chg(gio_num,0x1,AV3XX_GIO_BITSET0,AV3XX_GIO_BITSET1,32,0);        
}

void av3xx_gio_raw(int data,int bank)
{
    if(data!=0)
        outw(data,bank==0?AV3XX_GIO_BITSET0:AV3XX_GIO_BITSET1);
    data ^= 0xFF00;
    if(data!=0)
        outw(data,bank==0?AV3XX_GIO_BITCLEAR0:AV3XX_GIO_BITCLEAR1);
}

void av3xx_gio_clear(int gio_num)
{
    av3xx_gio_chg(gio_num,0x1,AV3XX_GIO_BITCLEAR0,AV3XX_GIO_BITCLEAR1,32,0);        
}

void av3xx_gio_IRQ(int gio_num,int set)
{
    av3xx_gio_chg(gio_num,set,AV3XX_GIO_ENABLE_IRQ,AV3XX_GIO_ENABLE_IRQ,8,1);        
}

int av3xx_gio_is_set(int gio_num)
{
    int addr;
    
    if(gio_num>=0 && gio_num<32)
    {
        if(gio_num < 16)
        {
            addr = AV3XX_GIO_BITSET0;
        }
        else
        {
            addr = AV3XX_GIO_BITSET1; 
            gio_num -= 16;  
        }
        
        return ((inw(addr) & (0x1 << gio_num)) != 0);
    }
//     printk("Error wrong GIO number: 0x%x\n",gio_num);
    return 0;
}


static int __init av3xx_gio_init(void)
{
    /*outw(0xFFFF,AV3XX_GIO_DIRECTION0);
    outw(0xFFFF,AV3XX_GIO_DIRECTION1);
    outw(0x0000,AV3XX_GIO_INVERT0);
    outw(0x0000,AV3XX_GIO_INVERT1);
    outw(0xFFFF,AV3XX_GIO_BITCLEAR0);
    outw(0xFFFF,AV3XX_GIO_BITCLEAR1);
    outw(0x0000,AV3XX_GIO_BITSET0);
    outw(0x0000,AV3XX_GIO_BITSET1);*/
    
    /* keep the screen ON */
    /*av3xx_gio_dir(AV3XX_GIO_LCD_BACKLIGHT,GIO_OUT);
    av3xx_gio_set(AV3XX_GIO_LCD_BACKLIGHT);*/
    

    /* everything is ok */
    printk("av3xx-gio driver by oxygen77@free.fr\n");
    return 0;
}

static void __exit av3xx_gio_exit(void)
{
	
}

module_init(av3xx_gio_init);
module_exit(av3xx_gio_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Gio driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
