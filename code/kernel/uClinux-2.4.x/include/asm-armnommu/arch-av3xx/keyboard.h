/*
 *  linux/include/asm-arm/arch-ti926/keyboard.h
 *
 *  Copyright (C) 2000 RidgeRun, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Keyboard driver definitions for ARM
 */
#ifndef __ASM_ARM_ARCH_TI925_KEYBOARD_H
#define __ASM_ARM_ARCH_TI925_KEYBOARD_H

/*
 * Required by drivers/char/keyboard.c. 
 */
#define kbd_setkeycode(sc,kc) (-EINVAL)
#define kbd_getkeycode(sc) (-EINVAL)
#define kbd_translate(sc,kcp,rm) ({ *(kcp) = (sc); 1; })
#define kbd_unexpected_up(kc) (0200)
#define kbd_leds(leds)
#define kbd_init_hw()
#define kbd_enable_irq()
#define kbd_disable_irq()


#endif /* __ASM_ARM_ARCH_TI925_KEYBOARD_H */
