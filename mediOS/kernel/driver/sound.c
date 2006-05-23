/* 
*   kernel/driver/sound.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/kernel.h>

#include <kernel/mas.h>
#include <kernel/aic23.h>

#include <kernel/sound.h>
#include <sys_def/sound.h>

/********************* MIXER                 ***************************/

void sound_mixerCtl(unsigned int cmd, int dir, void * arg)
{
    /*int * val=(int*)arg;
    int tmp;


    switch(cmd)
    {
        case MIXER_VOLUME:
            if(dir==MAS_SET)
            {
                mas_control_config(MAS_SET,MAS_VOLUME,*val);
                oldVol=*val;
            }
            else
            {
                *val=mas_control_config(MAS_GET,MAS_VOLUME,*val);
            }
            break;
        case MIXER_BALANCE:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_BALANCE,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_BALANCE,*val);
            break;
        case MIXER_MUTE:
            if(dir==MAS_SET)
            {
                if(*val)
                    mas_control_config(MAS_SET,MAS_VOLUME,0);
                else
                    mas_control_config(MAS_SET,MAS_VOLUME,oldVol);
            }
            else
            {
                tmp=mas_control_config(MAS_GET,MAS_VOLUME,*val);
                if(tmp==0)
                    *val=1;
                else
                    *val=0;
            }
            break;
        case MIXER_BASS:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_BASS,*val);
            else            
                *val=mas_control_config(MAS_GET,MAS_BASS,*val);
            break;
        case MIXER_TREBLE:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_TREBLE,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_TREBLE,*val);
            break;
        case MIXER_LOUDNESS:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_LOUDNESS,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_LOUDNESS,*val);
            break;
        case MIXER_MIC_GAIN:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_MICRO_GAIN,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_MICRO_GAIN,*val);
            break;
        case MIXER_ADC_L_GAIN:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_ADC_L_GAIN,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_ADC_L_GAIN,*val);                
            break;
        case MIXER_ADC_R_GAIN:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_ADC_R_GAIN,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_ADC_R_GAIN,*val);
            break;
        default:
                printk("[ctl MIX] bad ctl\n");
    }*/

}


/********************* sound init               ***************************/

void sound_init(void)
{

#ifdef HAVE_MAS_SOUND
    mas_init();
#endif

#ifdef HAVE_AIC23_SOUND
    aic23_init();
#endif
   
    printk("[init] sound done\n");
}
