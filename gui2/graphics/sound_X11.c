/*
* sound_X11.c
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

#include "sound.h"

int ini_sound_connection(void)
{
   return 1;
}

int ini_mp3_playback(struct mp3_play * mp3_p_data)
{    
    return 1;
}

void close_mp3_playback(void)
{
    stop_playback();  
}

int start_playback(void)
{
    return 0;
}

int pause_playback(void)
{
    return 0;
}

int stop_playback(void)
{
    return 0;
}

int readPeak(struct av_peak * peak)
{
    return 0;  
}

int setVolume(int val)
{
    return 0;
}

int setBass(int val)
{
    return 0;
}

int setTreble(int val)
{
    return 0;
}

int setLoudness(int val)
{
    return 0;
}

int setBalance(int val)
{
    return 0;
}


int getVolume(void)
{
    return 0;
}

int getBass(void)
{
    return 0;
}

int getTreble(void)
{
    return 0;
}

int getLoudness(void)
{
     return 0;
}

int getBalance(void)
{
    return 0;
}
