/*
* events.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __EVENTS_H
#define __EVENTS_H

#ifdef AV_SCREEN
#include "av3xx_common.h"
#else
struct mouseParam {
};
#endif

#define        NO_EVENT    0x0000
#define        BTN_UP      0x0001
#define        BTN_LEFT    0x0002
#define        BTN_RIGHT   0x0003
#define        BTN_DOWN    0x0004
#define        BTN_F3      0x0005
#define        BTN_F1      0x0006
#define        BTN_F2      0x0007
#define        BTN_JOY     0x0008
#define        BTN_ON      0x0009
#define        BTN_OFF     0x000a
#define        EVT_TIMER   0x000b
#define        EVT_WKUP    0x000c
#define        EVT_USB     0x000d
#define        EVT_PWR     0x000e


int  iniEvent(void);
int  nxtEvent(void);
int  waitEvent(void);
int  clearEventQueue(void);
int  wakeUP(void);
int  halt_device(void);
int  setRepeate(int val);
int  getRepeate(void);
int  setFreq(int val);
int  getFreq(void);
int  setSettings(struct mouseParam * param);
int  getSettings(struct mouseParam * param);
int  setTimerFreq(int val);
int  startTimer();
int  stopTimer();
int  timerState();
int  pause_app(void);
int  release_app(void);

#endif
