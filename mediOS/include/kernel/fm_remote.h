/* 
*   include/kernel/fm_remote.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __FM_REMOTE_H
#define __FM_REMOTE_H


#include <kernel/uart.h>
#define FM_UART 1

#define MAX_PING      3
#define MAX_NON_GET   5
#define NB_KEY        8

/********************************************* user functions */

/* remote FM icons */
#define FM_PLAY           0x0
#define FM_REC            0x1
#define FM_PAUSE          0x2
#define FM_LOOP           0x3
#define FM_1PLAY          0x4
#define FM_BAT            0x5
#define FM_VOL            0x6

/* light */
#define FM_backlight_on      FM_setLight(0x0,0x1)
#define FM_backlight_off     FM_setLight(0x0,0x0)
#define FM_backlight_is_on   FM_getLight(0x0)
#define FM_REC_light_on      FM_setLight(0x1,0x1)
#define FM_REC_light_off     FM_setLight(0x1,0x0)
#define FM_REC_light_is_on   FM_getLight(0x1)
void FM_lightsOFF(void);
void FM_lightsON(void);
int  FM_lightsState(void);

/* Text */
void FM_put_iniTxt(void);
void FM_putText(char * txt);
void FM_putTmpText(char * str,int iter);
void FM_savText(char * str,int num);
void FM_getText(char * txt);
void FM_setScroll(int OnOff);
int  FM_getScroll(void);

/* contrast */
void FM_setContrast(int val);
int  FM_getContrast(void);

/* icons */
void FM_setIcon(int icon,int val);
int  FM_getIcon(int icon);

/* connection status */
int  FM_is_connected(void);

/* radio send */
#define FM_IN_SEARCH_MODE 1
#define FM_IN_SET_MODE    0
#define FM_SEARCH_UP      1
#define FM_SEARCH_DOWN    0
#define FM_STOP_HIGH      3
#define FM_STOP_MID       2
#define FM_STOP_LOW       1
#define FM_HIGH_INJECTION 1
#define FM_LOW_INJECTION  0
#define FM_STEREO         0
#define FM_FORCE_MONO     1
#define FM_EXT_PORT_SET   1
#define FM_EXT_PORT_CLEAR 0
#define FM_JPN_BAND       1
#define FM_US_EU_BAND     0
#define FM_XTAL0          0
#define FM_XTAL1          1
#define FM_DTC_75         1
#define FM_DTC_50         0

#define FM_ON             1
#define FM_OFF            0

void FM_do_turn_on_radio(void);
void FM_do_turn_off_radio(void);
void FM_do_turn_on_mic(void);
void FM_do_turn_off_mic(void);

/***************************************** end user functions */

/*****************************************    timer functions */

extern int FM_connected;
extern int nbPingSend;
extern int inHold;

#define FM_REMOTE_CHK {               \
    if(FM_connected)                  \
    {                                 \
        nbPingSend++;                 \
        if(nbPingSend>MAX_PING)       \
        {                             \
            FM_connected=0;           \
            nbPingSend=0;             \
            inHold=0;                 \
            printk("[FM Remote] disconnected\n"); \
        }                             \
        uart_out('v',UART_1);          \
    }                                 \
}


/**************************************** end timer functions */

/****************************************** private functions */

void processKey(int key);

void FM_setLight(int type,int direction);
int  FM_getLight(int type);

int fm_uart_getc(char *c);
void fm_uart_putc(char c);

void FM_do_setLight(void);
void FM_do_putText(void);

#define FMSetIcon(icon,var)    {var|=(0x1<<(icon));}
#define FMClearIcon(icon,var)  {var&=~(0x1<<(icon));}
#define FMIsSetIcon(icon,var)  ((var & (0x1<<(icon))) !=0)
void FM_do_setIcon(void);

void test_icons(void);

void FM_do_setRadio(void);

void FM_send_data(char cmd,char * data,int size);
void FM_do_ini_call(void);

void init_fm_remote(void);

/************************************** end private functions */

#endif
