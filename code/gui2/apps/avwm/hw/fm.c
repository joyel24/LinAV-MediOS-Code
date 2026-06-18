/*
* fm.c
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

#include "misc.h"
#include "events.h"
#include "avevents.h"
#include "plugin.h"

#define disp_time   100

struct plugin FM_plugin;
int FMisON=0;
float fm_freq=98.50;

void FMEvtHandler(int evt)
{
    char tmp[50];
    switch (evt)
    {
        case BTN_FM:
            FMisON=~FMisON;
            fprintf(stderr,"[FM-AVWM] radio %s\n",FMisON?"enable":"disable");
            fmSetRecLight(FMisON);
            fmSetTmpTxt(FMisON?"FM is on":"FM is off",disp_time);
            if(FMisON)
                fmOnRadio();
            else
                fmOffRadio();
            break;
        case BTN_REC:
            if(FMisON)
            {
                fm_freq=fmGetFreq();
                sprintf(tmp,"FM %.02f",fm_freq);
                fmSetTmpTxt(tmp,disp_time);
            }
            break;
    }          
}

void tunr_OFF_FM(void)
{
}

void turn_ON_FM(void)
{
}

void ini_FM(void)
{
    /* check if FM is on ?? */
    FMisON=0;
    doRegisterPlugin(&FM_plugin,FMEvtHandler,0);
    FM_plugin.handle_on=1;
}
