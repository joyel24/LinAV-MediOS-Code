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

#include <asm/elf.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

extern void genarch_init_irq(void);

MACHINE_START(AV3XX, "AV3XX - TI TMS320DSC25")
	MAINTAINER("THOMAS Christophe oxygen77@free.fr")
	BOOT_MEM(0x03000000, 0x03000000, 0x00000000)
	INITIRQ(genarch_init_irq)
MACHINE_END
