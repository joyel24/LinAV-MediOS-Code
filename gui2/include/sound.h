/*
* sound.h
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

#ifndef __SOUND_H
#define __SOUND_H

#include "av3xx_common.h"


int  ini_sound_connection(void);
int  ini_mp3_playback   (struct mp3_play * mp3_p_data);
void close_mp3_playback (void);
int  start_playback     (void);
int  pause_playback     (void);
int  stop_playback      (void);

int  readPeak           (struct av_peak * peak);
int  readFrame          (void);


int  setVolume          (int val);
int  setBass            (int val);
int  setTreble          (int val);
int  setLoudness        (int val);
int  setBalance         (int val);

int  getVolume          (void);
int  getBass            (void);
int  getTreble          (void);
int  getLoudness        (void);
int  getBalance         (void);

int  start_peak         (void);
int  stop_peak          (void);

int  execDSP_ioctl      (int ioctl_call,unsigned int val);
int  execMIX_ioctl      (int ioctl_call,unsigned int val);


#endif
