/* 
*   kernel/driver/sound.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>

#include <kernel/irq.h>
#include <kernel/kernel.h>

#include <kernel/hardware.h>
#include <kernel/gio.h>

#include <kernel/mas.h>
#include <kernel/sound.h>

#define MAS_DELAY        {int __i; for(__i=0;__i<20;__i++);}
#define MIN(a, b)        (((a)<(b))?(a):(b))

/********************* DSP                    ***************************/


__IRAM_DATA int playing=0;   // 1 if we are sending data to MAS, 0 if no more data to send or Stop mode or Pause mode
__IRAM_DATA int in_pause=0;     // 1 if in Pause mode
__IRAM_DATA int nb_send;
__IRAM_DATA int nb_loop;

__IRAM_DATA struct mp3_play * data;

#define SEND_TO_MAS(BUFFER,SIZE)                              \
 ({                                                           \
    int  __i;                                                 \
    int __data;                                              \
    for(__i=0;__i<SIZE;__i++)                                 \
    {                                                         \
        if(inw(GIO_BITSET0) & (0x1<<GIO_MAS_EOD))             \
            break;  /* EOD set => exit */                     \
        __data=(BUFFER[__i] << 8) & 0xFF00;                   \
        if(__data!=0)                                         \
            outw(__data,GIO_BITSET0);                         \
        __data ^= 0xFF00;                                     \
        if(__data!=0)                                         \
            outw(__data,GIO_BITCLEAR0);                       \
        /* try to latch data (raise PR) */                    \
        outw(0x1<<(GIO_MAS_PR-16),GIO_BITSET1);               \
        /* wait for RTR to be set */                          \
        while(!(inw(GIO_BITSET1) & (0x1<<(GIO_MAS_RTR-16))))  \
            /*nothing*/;                                        \
        /* clear latch (lower raise PR) */                    \
        outw(0x1<<(GIO_MAS_PR-16),GIO_BITCLEAR1);             \
    }                                                         \
    __i;                                                      \
  })

__IRAM_CODE void dsp_interrupt(int irq)
{
   int toSend;
   int send;
   char * buffer;
   if(playing)
   {       
       /*if(data->buffer_write<data->buffer_read)*/
            toSend=data->buffer_len-data->buffer_read;
       /* else
            toSend=data->buffer_write-data->buffer_read;*/
        buffer=data->buffer+data->buffer_read;    
        send=SEND_TO_MAS(buffer,toSend);
        /*if(send>0)
            nb_send++;*/
        data->buffer_read+=send;
        
        if(data->buffer_read>= data->buffer_len)
        {
            /*data->buffer_read=0;
            toSend=data->buffer_write-data->buffer_read;
            buffer=data->buffer;
            data->buffer_read+=SEND_TO_MAS(buffer,toSend); */
            disable_irq(IRQ_MAS_DATA);
            printk("end of playback send=%d loop=%d\n",nb_send,nb_loop);
            playing=0;
        }
    }
}

__IRAM_CODE void dsp_ctl(unsigned int cmd, void * arg)
{	
    int * val;
    struct av_peak * av_p;
    switch(cmd)
    {
        case DSP_INI_MP3:
            ini_mas_for_mp3();
            data=(struct mp3_play *)arg;    
            playing=1;
            in_pause=0;
            nb_send=0;
            nb_loop=0;
            break;
        case DSP_START_MP3:
            if(in_pause)
            {
                playing=1;
                in_pause=0;
                dsp_interrupt(IRQ_MAS_DATA);
            }
            else
            {
                playing=1;
                enable_irq(IRQ_MAS_DATA);
                dsp_interrupt(IRQ_MAS_DATA);                
            }
            printk("MP3 playing\n");
            break;
        case DSP_STOP_MP3:
            playing=0;
            disable_irq(IRQ_MAS_DATA);
            break;
        case DSP_PAUSE_MP3:
            playing=0;
            in_pause=1;
            break;
        case DSP_FRAME_CNT:
            val=(int*)arg;
            *val=mas_get_frame_count();
            break;
        case DSP_IN_PEAK:
            av_p=(struct av_peak *)arg;
            av_p->left=(mas_read_codec(MAS_REG_INPEAK_LEFT)*100)/0x7FFF;
            av_p->right=(mas_read_codec(MAS_REG_INPEAK_RIGHT)*100)/0x7FFF;
            break;
        case DSP_OUT_PEAK:
            av_p=(struct av_peak *)arg;
            av_p->left=(mas_read_codec(MAS_REG_OUTPEAK_LEFT)*100)/0x7FFF;
            av_p->right=(mas_read_codec(MAS_REG_OUTPEAK_RIGHT)*100)/0x7FFF;
            break;
        case DSP_IN_PEAK_REAL:
            av_p=(struct av_peak *)arg;
            av_p->left=mas_read_codec(MAS_REG_INPEAK_LEFT);
            av_p->right=mas_read_codec(MAS_REG_INPEAK_RIGHT);
            break;
        case DSP_OUT_PEAK_REAL:
            av_p=(struct av_peak *)arg;
            av_p->left=mas_read_codec(MAS_REG_OUTPEAK_LEFT);
            av_p->right=mas_read_codec(MAS_REG_OUTPEAK_RIGHT);
            break;
    }	

}

int mas_stop_mp3_app(void)
{
    int i;
    mas_app_select(MASS_APP_NONE);
    while(1)
    {
        i=mas_app_running(MASS_APP_ANY);
        if(i<0)
        {
            printk("error getting app status (trying to stop)\n");
            return 0;
        }
        if(i==0)
            break;
    }
    return 1;
}

int mas_start_mp3_app(void)
{
    int i;
    mas_app_select(MASS_APP_MPEG3_DEC | MASS_APP_MPEG2_DEC);	
    while(1)
    {
        i=mas_get_D0(MAS_APP_SELECT);
        if(i<0)
        {
            printk("error getting app status (trying to start)\n");
            return 0;
        }
        if((i & MASS_APP_MPEG3_DEC) && (i & MASS_APP_MPEG2_DEC))
            break;
    }
    return 1;
}

void mas_line_in_on(void)
{
    mas_write_codec(MAS_REG_AUDIO_CONF, MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
                        | 0x0  << 4 // mic gain
                        | 0xf << 8  // adc gain right
                        | 0xf <<12  // adc gain left
                        );
    mas_write_codec(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_STEREO);
    mas_write_codec(MAS_REG_MIX_ADC_SCALE,0x40 << 8);
    mas_write_codec(MAS_REG_MIX_DSP_SCALE,0x00 << 8);
    mas_write_codec(MAS_REG_DA_OUTPUT_MODE,0x0);
    mas_control_config(MAS_SET,MAS_BALANCE,50);
    mas_control_config(MAS_SET,MAS_VOLUME,70);
}

void mas_line_in_off(void)
{
    mas_write_codec(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
    mas_control_config(MAS_SET,MAS_VOLUME,0x0);
}

int ini_mas_for_mp3(void)
{
    mas_write_codec(MAS_REG_AUDIO_CONF,MAS_INPUT_AD | MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
                        | 0xf  << 4 // mic gain
                        | 0xf << 8  // adc gain right
                        | 0xf <<12  // adc gain left
                        );
    mas_write_codec(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_MONO);
    mas_write_codec(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
    mas_write_codec(MAS_REG_MIX_DSP_SCALE,0x40 << 8);
    mas_write_codec(MAS_REG_DA_OUTPUT_MODE,0x0);
    mas_control_config(MAS_SET,MAS_BALANCE,50);
    mas_control_config(MAS_SET,MAS_VOLUME,70);
    
    MAS_DELAY
        
    if(!mas_stop_mp3_app())
        return -1;        
    
    mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
    mas_set_clk_speed(0x4800);
    mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);
    
    MAS_DELAY
        
    if(!mas_start_mp3_app())
        return -1;	
            
    printk("MAS configured for mp3 playing, waiting for start\n");
    
    return 0;
}

/********************* MIXER                 ***************************/

int oldVol=0;

void mixer_ctl(unsigned int cmd, int dir, void * arg)
{
    int * val=(int*)arg;
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
    }
}


/********************* OSS init               ***************************/

void init_sound(void)
{
    struct mas_version version;
    
    printk("[init] Loading av3xx sound driver: ");
    mas_gio_init();
    printk("M");
    mas_reset();
    printk("A");
    mas_read_version(&version);
    printk("S%x:%d:%c%d ",version.major_number,version.derivate,version.char_order_version,version.digit_order_version);
    oldVol=mas_control_config(MAS_GET,MAS_VOLUME,0);
    playing=0;
    in_pause=0;

    disable_irq(IRQ_MAS_DATA);
    
    add_irq_handler(IRQ_MAS_DATA,dsp_interrupt,"MAS");        

    disable_irq(IRQ_MAS_DATA);
    
            
    /*************************************************************/
    #if 0
    /* wave testing */
    mas_write_codec(MAS_REG_AUDIO_CONF,MAS_INPUT_AD | MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
                                | 0xf  << 4 // mic gain
                                | 0xf << 8  // adc gain right
                                | 0xf <<12  // adc gain left
                                );
    mas_write_codec(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_MONO);
    mas_write_codec(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
    mas_write_codec(MAS_REG_MIX_DSP_SCALE,0x40 << 8);
    mas_write_codec(MAS_REG_DA_OUTPUT_MODE,0x0);
    mas_control_config(MAS_SET,MAS_BALANCE,50);
    mas_control_config(MAS_SET,MAS_VOLUME,70);
    
    
    mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
    mas_set_clk_speed(0x4800);
    mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);
    
    mas_test_PCM();
    
    int cnt,mp3ptr=0;
    
    while(1)
    {
        cnt = mas_pio_write(mp3Buff + mp3ptr, 2000); 
        printk("%d ",cnt); 
        //mp3ptr+=cnt;
        //if (mp3ptr>=65000){mp3ptr=0; printk("loop ");}
    }
    #endif
    /*************************************************************/
    printk(" done\n");

}

