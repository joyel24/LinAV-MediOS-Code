/*
 * linux/include/asm-armnommu/arch-av3xx/av3xx_module.h
 *
 * Copyright (c) 1998 Hugo Fiennes & Nicolas Pitre
 *
 * 18-aug-2000: Cleanup by Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *              Get rid of the special ide_init_hwif_ports() functions
 *              and make a generalised function that can be used by all
 *              architectures.
 */

#ifndef __ASM_ARCH_MODULE_H
#define __ASM_ARCH_MODULE_H

/********************************************* user functions */

#define AV_MODULE_NONE      0x0f
#define AV_MODULE_CFC_R     0x0c
#define AV_MODULE_CAM       0x0b
#define AV_MODULE_DVR       0x0a
#define AV_MODULE_CFC       0x08
#define AV_MODULE_SSFDC_R   0x07
#define AV_MODULE_MMC_R     0x05
#define AV_MODULE_SSFDC     0x03
#define AV_MODULE_MMC       0x01

int  av3xx_get_connected_module(void);

struct module_actions {
    void (*do_connection)(void);
    void (*do_disconnection)(void);
};

int  av3xx_module_register_action(struct module_actions * action_struct,int module_num);

/****************************************** end user functions */

/****************************************** private functions */

void do_mod_connect(int module_num);
void do_mod_disconnect(int module_num);
int  av3xx_get_module(void);

/************************************** end private functions */

#endif
