/*
* misc.h
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

#ifndef __MISC_H
#define __MISC_H

#include "av3xx_common.h"

int   getTimeS          (char * timeSt);
int   getTime           (struct av_tm * date_time);
int   setTime           (struct av_tm * date_time);
int   getTick           (void);

int   set_mouseParam    (int freq, int repeat);
int   get_mouseFreq     ();
int   get_mouseRepeat   ();

int   set_lcd_TimeOutParam  (int state, int value);
int   get_lcd_TimeOutParam  (int state);

int   set_hd_TimeOutParam(int state, int value);
int   get_hd_TimeOutParam(int state);

int   set_halt_TimeOutParam(int state, int value);
int   get_halt_TimeOutParam(int state);

int   setBrightness(int val);
int   getBrightness(void);

int   getBat            (void);

int   getPwr            (void);

int   getUSB            (void);

int   getFwExt          (void);

int   setUSB            (int state);

int   fmIsConnected     (void);
int   fmSetBat          (int val);
int   fmSetVol          (int val);
int   fmSetTxt          (char * txt);
int   fmSetTmpTxt       (char * txt,int iter);
int   fmSetBackLight    (int val);
int   fmSetRecLight     (int val);
int   fmGetBackLight    (void);
int   fmGetRecLight     (void);
int   fmOnRadio         (void);
int   fmOffRadio        (void);
float fmGetFreq         (void);
int   fmSetFreq         (float freq);

int   CF_mod_is_connected (void);
int   CF_is_connected     (void);


int   gen_ioctl         (char * dev_name,int ioctl_cmd,void * param);
#endif
