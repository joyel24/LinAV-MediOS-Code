/*
 * linux/include/asm-arm/arch-sa1100/ide.h
 *
 * Copyright (c) 1998 Hugo Fiennes & Nicolas Pitre
 *
 * 18-aug-2000: Cleanup by Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *              Get rid of the special ide_init_hwif_ports() functions
 *              and make a generalised function that can be used by all
 *              architectures.
 */

#ifndef __ASM_ARCH_IDE_H
#define __ASM_ARCH_IDE_H

#include <asm/arch/av3xx_cpld.h>

#define AV3XX_IDE_DATA                          0x000
#define AV3XX_IDE_ERROR                         0x080
#define AV3XX_IDE_NSECTOR                       0x100
#define AV3XX_IDE_SECTOR                        0x180
#define AV3XX_IDE_LCYL                          0x200
#define AV3XX_IDE_HCYL                          0x280
#define AV3XX_IDE_SELECT                        0x300
#define AV3XX_IDE_CONTROL                       0x340
#define AV3XX_IDE_STATUS                        0x380

/* we have one controller */
#undef MAX_HWIFS
#define MAX_HWIFS	1

//#define DEBUG 1

extern void ide_delay_50ms(void);

static struct av3xx_ide_defaults {
	ide_ioreg_t	base;
	int		irq;
} AV3XX_ide_defaults[MAX_HWIFS] = {
	{ ((ide_ioreg_t)AV3XX_IDE_BASE), AV3XX_IDE_IRQ },
};

static int ide_offsets[AV3XX_NR_PORTS] = {
    AV3XX_IDE_DATA, AV3XX_IDE_ERROR,  AV3XX_IDE_NSECTOR, AV3XX_IDE_SECTOR, AV3XX_IDE_LCYL,
    AV3XX_IDE_HCYL, AV3XX_IDE_SELECT, AV3XX_IDE_STATUS,  AV3XX_IDE_CONTROL, -1
};

static __inline__ ide_ioreg_t ide_default_io_base(int index)
{
	if ( index >= 0 && index < MAX_HWIFS )
		return AV3XX_ide_defaults[index].base;
	return 0;
}

static __inline__ int ide_default_irq(ide_ioreg_t base)
{
	int i;

	for ( i = 0; i < MAX_HWIFS; i++ )
		if ( AV3XX_ide_defaults[i].base == base )
			return AV3XX_ide_defaults[i].irq;
	return 0;
}

/*
 * Set up a hw structure for a specified data port, control port and IRQ.
 * This should follow whatever the default interface uses.
 */

static __inline__ void
ide_init_hwif_ports(hw_regs_t *hw, int data_port, int ctrl_port, int *irq)
{
	int i;

	for ( i = 0; i <= AV3XX_NR_PORTS; i++ ) {
		hw->io_ports[i] = data_port+ide_offsets[i];
	}
}

/*
 * This registers the standard ports for this architecture with the IDE
 * driver.
 */
static __inline__ void
ide_init_default_hwifs(void)
{
	hw_regs_t hw;
	ide_ioreg_t base;
	int index;
	int c;

	// let's start the HD

        cpld_select(AV3XX_CPLD_HD_CF,0); // select HD
        
	cpld_set_port_3(AV3XX_CPLD_HD_POWER);
        

        
	for (index = 0; index < MAX_HWIFS; index++) {

		base = ide_default_io_base(index);
		if (!base) continue;

		memset(&hw, 0, sizeof(hw));
		ide_init_hwif_ports(&hw, base, 0, NULL);
		hw.irq = ide_default_irq(base);
		ide_register_hw(&hw, NULL);
	}

	// wait for the drive to be ready
	/*while((inb(0x30a24)&0x1)!=0x1)
		printk("l ");*/
	while((inb(AV3XX_IDE_STATUS+AV3XX_IDE_BASE)&0x80)==0x80)
		ide_delay_50ms();
	/*outw(0x1,0x30a30);
	outw(0x8,AV3XX_IDE_CONTROL);*/

        
	/* 27MHz clock (not ARM clock) */
	outw(AV3XX_TMR_SEL_EXT, AV3XX_TIMER1_BASE+AV3XX_TIMER_SEL);
	/* prescale 10 */
	outw(9, AV3XX_TIMER1_BASE+AV3XX_TIMER_SCAL);
	/* div 27000 */
	outw((CONFIG_ARM_CLK/10000)-1, AV3XX_TIMER1_BASE+AV3XX_TIMER_DIV);
	/* freerun */
	outw(AV3XX_TMR_MODE_FREERUN, AV3XX_TIMER1_BASE+AV3XX_TIMER_MODE);
        

}

#define ide_request_irq(irq,hand,flg,dev,id)	request_irq((irq),(hand),(flg),(dev),(id))
#define ide_free_irq(irq,dev_id)		free_irq((irq), (dev_id))
#define ide_check_region(from,extent)		check_region((from), (extent))
#define ide_request_region(from,extent,name)	request_region((from), (extent), (name))
#define ide_release_region(from,extent)		release_region((from), (extent))

#define ide_ack_intr(hwif)              (1)

#endif
