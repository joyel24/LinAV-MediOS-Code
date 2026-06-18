/*
 *  arch/arm/mach-av3xx/arch.c
 *
 *   Derived from arch/armnommu/mach-atmel/arch.c
 *
 *   Copyright (C) 2004 InnoMedia Pte Ltd. All rights reserved.
 *   cheetim_loh@innomedia.com.sg  <www.innomedia.com>
 *
 *  Architecture specific fixups.  This is where any
 *  parameters in the params struct are fixed up, or
 *  any additional architecture specific information
 *  is pulled from the params struct.
 */
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/ide.h>

#include <asm/io.h>
#include <asm/elf.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/arch/av3xx_osd.h>
#include <asm/arch/av3xx_FM.h>

extern void genarch_init_irq(void);

MACHINE_START(AV3XX, "AV3XX - TI TMS320DSC25")
	MAINTAINER("THOMAS Christophe oxygen77@free.fr")
	BOOT_MEM(0x03000000, 0x03000000, 0x00000000)
	INITIRQ(genarch_init_irq)
MACHINE_END

void av_halt_system(void)
{
	int ret;
	printk("let's go to halt\n");
        display_halt_screen();
        if(av3xx_FM_is_connected())
            av3xx_FM_putText("HALTING");
        sys_umount("/mnt", MNT_FORCE);
        sys_umount("/cf", MNT_FORCE);
        av3xx_ide_halt();
	//ret=sys_reboot(LINUX_REBOOT_MAGIC1,LINUX_REBOOT_MAGIC2,LINUX_REBOOT_CMD_RESTART,NULL);
	clf();
	while(1)
		outw(0,0x30a1a);
}

#define AV3XX_LCD_WIDTH   320
#define AV3XX_LCD_HEIGHT  240

#define AV3XX_TXT_WIDTH   220
#define AV3XX_TXT_HEIGHT  35
#define AV3XX_TXT_X       5
#define AV3XX_TXT_Y       68

extern char av3xx_startup_scr[AV3XX_LCD_HEIGHT * AV3XX_LCD_WIDTH * 4];
//extern char av3xx_start_scr[AV3XX_TXT_HEIGHT * AV3XX_TXT_WIDTH * 4];
extern char av3xx_halt_scr[AV3XX_LCD_HEIGHT * AV3XX_LCD_WIDTH * 4];

void display_startup_screen(void)
{
    osdInit();
    
    /* reset everything */
    osdSetComponentConfig(AV3XX_OSD_VIDEO1,  0);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2,  0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR1, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR2, 0);
    
    osdSetComponentSize(AV3XX_OSD_VIDEO1, AV3XX_LCD_WIDTH*2, AV3XX_LCD_HEIGHT);
    osdSetComponentPosition(AV3XX_OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffset(AV3XX_OSD_VIDEO1, (int)av3xx_startup_scr);
    osdSetComponentSourceWidth(AV3XX_OSD_VIDEO1, 0x28);
    osdSetComponentConfig(AV3XX_OSD_VIDEO1, AV3XX_OSD_COMPONENT_ENABLE);
    
    /*osdSetComponentSize(AV3XX_OSD_VIDEO2, 220*2, AV3XX_TXT_HEIGHT);
    osdSetComponentPosition(AV3XX_OSD_VIDEO2, 0x14+AV3XX_TXT_X, 0x12+AV3XX_TXT_Y);
    osdSetComponentOffset(AV3XX_OSD_VIDEO2, (int)av3xx_start_scr);
    osdSetComponentSourceWidth(AV3XX_OSD_VIDEO2, 224/8);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2, AV3XX_OSD_COMPONENT_ENABLE);*/
    
   
    
}

void display_halt_screen(void)
{
    osdInit();
    
    /* reset everything */
    osdSetComponentConfig(AV3XX_OSD_VIDEO1,  0);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2,  0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR1, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR2, 0);
    
    osdSetComponentSize(AV3XX_OSD_VIDEO1, AV3XX_LCD_WIDTH*2, AV3XX_LCD_HEIGHT);
    osdSetComponentPosition(AV3XX_OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffset(AV3XX_OSD_VIDEO1, (int)av3xx_halt_scr);
    osdSetComponentSourceWidth(AV3XX_OSD_VIDEO1, 0x28);
    osdSetComponentConfig(AV3XX_OSD_VIDEO1, AV3XX_OSD_COMPONENT_ENABLE);
    
    /*osdSetComponentSize(AV3XX_OSD_VIDEO2, 220*2, AV3XX_TXT_HEIGHT);
    osdSetComponentPosition(AV3XX_OSD_VIDEO2, 0x14+AV3XX_TXT_X, 0x12+AV3XX_TXT_Y);
    osdSetComponentOffset(AV3XX_OSD_VIDEO2, (int)av3xx_halt_scr);
    osdSetComponentSourceWidth(AV3XX_OSD_VIDEO2, 224/8);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2, AV3XX_OSD_COMPONENT_ENABLE);*/
    
   
    
}

#if 0
void av3xx_ide_stop_device(ide_drive_t * drive)
{
    ide_hwif_t *hwif = HWIF(drive);
    int j;	
	
    /* Spin down the drive */
    SELECT_DRIVE(drive);
    SELECT_MASK(drive, 0);
    hwif->OUTB(drive->select.all, IDE_SELECT_REG);
    (void) hwif->INB(IDE_SELECT_REG);
    udelay(100);
    hwif->OUTB(0x00, IDE_SECTOR_REG);
    hwif->OUTB(0x00, IDE_NSECTOR_REG);
    hwif->OUTB(0x00, IDE_LCYL_REG);
    hwif->OUTB(0x00, IDE_HCYL_REG);
    hwif->OUTB(drive->ctl|2, IDE_CONTROL_REG);   
    hwif->OUTB(WIN_STANDBYNOW1, IDE_COMMAND_REG);
    for (j = 0; j < 10; j++) {
            u8 status;
            mdelay(100);
            status = hwif->INB(IDE_STATUS_REG);
            if (!(status & BUSY_STAT) && (status & DRQ_STAT))
                    break;
    }
    
    cpld_clear_port_3(AV3XX_CPLD_HD_POWER);
    
    udelay(100);
    printk("\n%s stoped\n",drive->name);
}
#endif

