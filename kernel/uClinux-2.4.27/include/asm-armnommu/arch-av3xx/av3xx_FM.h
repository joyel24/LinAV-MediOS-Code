/*
 * linux/include/asm-armnommu/arch-av3xx/av3xx_FM.h
 *
 * Copyright (c) 1998 Hugo Fiennes & Nicolas Pitre
 *
 * 18-aug-2000: Cleanup by Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *              Get rid of the special ide_init_hwif_ports() functions
 *              and make a generalised function that can be used by all
 *              architectures.
 */

#ifndef __ASM_ARCH_FM_H
#define __ASM_ARCH_FM_H

/********************************************* user functions */

/* light */
#define FM_backlight_on      av3xx_FM_setLight(0x0,0x1)
#define FM_backlight_off     av3xx_FM_setLight(0x0,0x0)
#define FM_backlight_is_on   av3xx_FM_getLight(0x0)
#define FM_REC_light_on      av3xx_FM_setLight(0x1,0x0)
#define FM_REC_light_off     av3xx_FM_setLight(0x1,0x0)
#define FM_REC_light_is_on   av3xx_FM_getLight(0x1)
void av3xx_FM_lightsOFF(void);
void av3xx_FM_lightsON(void);
int av3xx_FM_lightsState(void);

/* Text */
void av3xx_FM_put_iniTxt(void);
void av3xx_FM_putText(char * txt);
void av3xx_FM_getText(char * txt);
void av3xx_FM_setScroll(int OnOff);
int av3xx_FM_getScroll(void);

/* contrast */
void av3xx_FM_setContrast(int val);
int  av3xx_FM_getContrast(void);

/* icons */
void av3xx_FM_setIcon(int icon,int val);
int  av3xx_FM_getIcon(int icon);

/* connection status */
int  av3xx_FM_is_connected(void);


/***************************************** end user functions */

/****************************************** private functions */

void processKey(int key);

void av3xx_FM_setLight(int type,int direction);
int  av3xx_FM_getLight(int type);

int  av3xx_uart_getc(char * c);
void av3xx_uart_putc(char c);
void av3xx_FM_do_setLight(void);
void av3xx_FM_do_putText(void);

#define FMSetIcon(icon,var)    {var|=(0x1<<(icon));}
#define FMClearIcon(icon,var)  {var&=~(0x1<<(icon));}
#define FMIsSetIcon(icon,var)  ((var & (0x1<<(icon))) !=0)
void av3xx_FM_do_setIcon(void);

void test_icons(void);

void av3xx_FM_do_ini_call(void);

/************************************** end private functions */

#endif
