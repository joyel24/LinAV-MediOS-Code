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

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_buttons.h>
#include <asm/arch/av3xx_ir.h>
#include <asm/arch/av3xx_module.h>
#include <asm/arch/av3xx_cpld.h>
#include <asm/arch/av3xx_gio.h>

int state=0;
int dataPos=0; // current pos in data
int IRdata=0;
int lastTmrVal=0;

#define NB_CODE 10

int nb_code=NB_CODE;

unsigned int IR_code[NB_CODE][2] = {
    { BUTTONS_AV300_MENU1,     0x00ff6a95 },
    { BUTTONS_AV300_MENU2,     0x00ffaa55 },
    { BUTTONS_AV300_MENU3,     0x00ffea15 },
    { BUTTONS_AV300_ON,        0x00ff4ab5 },
    { BUTTONS_AV300_LEFT,      0x00ff728d },
    { BUTTONS_AV300_RIGHT,     0x00fff20d },
    { BUTTONS_AV300_UP,        0x00ff926d },
    { BUTTONS_AV300_DOWN,      0x00ff52ad },
    { BUTTONS_AV300_JOYPRESS,  0x00ffb24d },
    { BUTTONS_AV300_OFF,       0x00ffd22d }
};

int last_code=-1;
int repeat_code=0;


void av3xx_ir_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    int val,delta;
    switch(state)
    {
        case 0:
            restartTimer();
            state=1;
            lastTmrVal=0;
            break;
        case 1:
            val=inw(AV3XX_TIMER2_BASE+AV3XX_TIMER_CNT)&0xFFFF;
            delta=val-lastTmrVal;
            lastTmrVal=val;
            if(delta<0x7d0)
            {
                if(delta>0x514 && delta<0x578)
                {
                    /* let's start a new code */
                    state=2;
                    dataPos=0;
                    IRdata=0;
                }
                else if(delta>0x44c && delta<0x4b0)
                {
                    /* last code repeated */
                    processCode(IRdata);
                }
            }
            else
            {
                restartTimer();
                lastTmrVal=0;
            }
            break;
        case 2:
            val=inw(AV3XX_TIMER2_BASE+AV3XX_TIMER_CNT)&0xFFFF;
            delta=val-lastTmrVal;
            lastTmrVal=val;
            if (delta>0x64 && delta<0x82)
            {
                dataPos++;
                IRdata = (IRdata << 1);
            }
            else if (delta>0xC8 && delta<0xF0)
            {
                dataPos++;
                IRdata = (IRdata << 1) | 1;
            }
            else
            {
                dataPos++;
                IRdata = (IRdata << 1);
            }
            
            if(dataPos==32)
            {
                state=0;
                processCode(IRdata);                         
            }
    }
}


void processCode(int code)
{
    int i;
    for(i=0;i<nb_code;i++)
        if(IR_code[i][1] == code)
        {            
            if(last_code==code)
            {
                if(repeat_code >= 2)
                {
                    av3xx_add_event(IR_code[i][0]);
                    repeat_code=0;
                }
                else
                    repeat_code++;
            }
            else
            {
                last_code=code;
                repeat_code=1;
            }
            break;
        }
}

void restartTimer(void)
{
    outw(AV3XX_TMR_SEL_EXT, AV3XX_TIMER2_BASE+AV3XX_TIMER_SEL);           /* Ext clk */    
    outw(0x10D, AV3XX_TIMER2_BASE+AV3XX_TIMER_SCAL);                      /* prescale  */    
    outw(0xFFFF, AV3XX_TIMER2_BASE+AV3XX_TIMER_DIV);                      /* div  */    
    outw(AV3XX_TMR_MODE_FREERUN, AV3XX_TIMER2_BASE+AV3XX_TIMER_MODE);     /* freerun */
}

void av3xx_ir_tmr_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    /* timeout in reception => stop current code */
    outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER2_BASE+AV3XX_TIMER_MODE);
    state=0;
    
}

void av3xx_ir_dvr_connected(void)
{
    cpld_set_port_0(AV3XX_CPLD_IR);
    cpld_set_port_3(AV3XX_CPLD3_IR);
    cpld_set_port_2(AV3XX_CPLD2_IR);
    av3xx_gio_dir(AV3XX_GIO_DVR_IR,GIO_IN);
    av3xx_gio_IRQ(AV3XX_GIO_DVR_IR,GIO_IRQ);
    enable_irq(AV3XX_IRQ_IR_VDR);
    outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER2_BASE+AV3XX_TIMER_MODE);
    enable_irq(AV3XX_IRQ_TMR2);    
    state=0;
    dataPos=0;
    IRdata=0;
    printk("DVR-IR connect done\n");
}

void av3xx_ir_dvr_disconnected(void)
{
    cpld_clear_port_0(AV3XX_CPLD_IR);
    cpld_clear_port_3(AV3XX_CPLD3_IR);
    cpld_clear_port_2(AV3XX_CPLD2_IR);
    outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER2_BASE+AV3XX_TIMER_MODE);
    disable_irq(AV3XX_IRQ_IR_VDR);
    disable_irq(AV3XX_IRQ_TMR2);
    printk("DVR-IR disconnect done\n");
}

struct module_actions ir_module_actions = {
    do_connection:av3xx_ir_dvr_connected,
    do_disconnection:av3xx_ir_dvr_disconnected
};

static int __init av3xx_ir_init(void)
{
    int ret;

    disable_irq(AV3XX_IRQ_IR_VDR); 
    disable_irq(AV3XX_IRQ_TMR2); 

        
    /* setting up GIO */   
    av3xx_gio_dir(AV3XX_GIO_DVR_IR,GIO_IN);
    av3xx_gio_IRQ(AV3XX_GIO_DVR_IR,GIO_IRQ);
    
    /* setting up IRQ handler */    
    if((ret=request_irq(AV3XX_IRQ_IR_VDR,av3xx_ir_interrupt,0,"av3xx_ir",NULL))<0)
    {
        printk(KERN_WARNING "Unable to set our irq handler for IR (irq=%d)\n",AV3XX_IRQ_IR_VDR);
        return ret;
    }
    
    if((ret=request_irq(AV3XX_IRQ_TMR2,av3xx_ir_tmr_interrupt,0,"av3xx_tmr_ir",NULL))<0)
    {
        printk(KERN_WARNING "Unable to set our irq handler for IR (irq=%d)\n",AV3XX_IRQ_TMR2);
        return ret;
    }
    
    /* register to module driver */    
    if(!av3xx_module_register_action(&ir_module_actions,AV_MODULE_DVR))
    {
        printk("[av3xx-IR] error registering to module driver\n");
    }

    disable_irq(AV3XX_IRQ_IR_VDR); 
    disable_irq(AV3XX_IRQ_TMR2); 
    
   
    printk("av3xx-IR driver by oxygen77@free.fr\n");   
    
    return 0;
}

static void __exit av3xx_ir_exit(void)
{
    free_irq(AV3XX_IRQ_IR_VDR,NULL);
    free_irq(AV3XX_IRQ_TMR2,NULL);
}

module_init(av3xx_ir_init);
module_exit(av3xx_ir_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("InfraRed driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
