/*
* sound.c
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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sound.h"
#include "misc.h"

int fd_dsp=-1;
int fd_mix=-1;

int ini_mp3_playback(struct mp3_play * mp3_p_data)
{    
    return execDSP_ioctl(AV_DSP_INI_MP3,(unsigned int)mp3_p_data);
}

void close_mp3_playback(void)
{
    stop_playback();
}

int start_playback(void)
{
    return execDSP_ioctl(AV_DSP_START_MP3,(unsigned int)NULL);
}

int pause_playback(void)
{
    return execDSP_ioctl(AV_DSP_PAUSE_MP3,(unsigned int)NULL);
}

int stop_playback(void)
{
    return execDSP_ioctl(AV_DSP_STOP_MP3,(unsigned int)NULL);
}

int start_peak(void)
{
    return execDSP_ioctl(AV_DSP_START_PEAK,(unsigned int)NULL);
}

int stop_peak(void)
{
    return execDSP_ioctl(AV_DSP_STOP_PEAK,(unsigned int)NULL);
}

int readPeak(struct av_peak * peak)
{
    return execDSP_ioctl(AV_DSP_OUT_PEAK_REAL,(unsigned int)peak);  
}

int readFrame(void)
{
    int ret;
    if(execDSP_ioctl(AV_DSP_FRAME_CNT,(unsigned int)&ret)<0)
        return 0;
    else
        return ret;
}

int setVolume(int val)
{
    return execMIX_ioctl(AV_SET_MIX_VOLUME,(unsigned int)&val);
}

int setBass(int val)
{
    return execMIX_ioctl(AV_SET_MIX_BASS,(unsigned int)&val);
}

int setTreble(int val)
{
    return execMIX_ioctl(AV_SET_MIX_TREBLE,(unsigned int)&val);
}

int setLoudness(int val)
{
    return execMIX_ioctl(AV_SET_MIX_LOUDNESS,(unsigned int)&val);
}

int setBalance(int val)
{
    return execMIX_ioctl(AV_SET_MIX_BALANCE,(unsigned int)&val);
}


int getVolume(void)
{
    int val;
    if(execMIX_ioctl(AV_SET_MIX_VOLUME,(unsigned int)&val))
        return val;
    else
        return 0;
}

int getBass(void)
{
    int val;
    if(execMIX_ioctl(AV_SET_MIX_BASS,(unsigned int)&val))
        return val;
    else
        return 0;
}

int getTreble(void)
{
    int val;
    if(execMIX_ioctl(AV_SET_MIX_TREBLE,(unsigned int)&val))
        return val;
    else
        return 0;
}

int getLoudness(void)
{
    int val;
    if(execMIX_ioctl(AV_SET_MIX_LOUDNESS,(unsigned int)&val))
        return val;
    else
        return 0;
}

int getBalance(void)
{
    int val;
    if(execMIX_ioctl(AV_SET_MIX_BALANCE,(unsigned int)&val))
        return val;
    else
        return 0;
}

int execDSP_ioctl(int ioctl_call,unsigned int val)
{
    /*if(fd_dsp<0 || ioctl(fd_dsp,ioctl_call,val)<0)
        return 0;
    else
       return 1;*/
    if(gen_ioctl("/dev/dsp",ioctl_call,(void*)val)<0)
    {
        printf("DSP ioctl error\n");
        return 0;
    }
    return 1;
}

int execMIX_ioctl(int ioctl_call,unsigned int val)
{
    /*if(fd_mix<0 || ioctl(fd_mix,ioctl_call,val)<0)
        return 0;
    else
       return 1;*/
    if(gen_ioctl("/dev/mixer",ioctl_call,(void*)val)<0)
    {
        printf("MIXER ioctl error\n");
        return 0;
    }
    return 1;
}