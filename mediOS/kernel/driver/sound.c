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

#include <kernel/io.h>

#include <kernel/irq.h>
#include <kernel/kernel.h>

#include <kernel/hardware.h>
#include <kernel/gio.h>

#include <kernel/mas.h>
#include <kernel/sound.h>
#include <sys_def/sound.h>

#define MAS_DELAY        {int ___i; for(___i=0;___i<20;___i++);}
#define MIN(a, b)        (((a)<(b))?(a):(b))
#define MAX(a, b)        (((a)<(b))?(b):(a))

/********************* DSP                    ***************************/


//__IRAM_DATA int playing_sound=0;   // 1 if we are sending data to MAS, 0 if no more data to send or Stop mode or Pause mode
__IRAM_DATA long g_nPaused = 0;     // 1 if in Pause mode

__IRAM_DATA sound_buffer_s * current_buffer;


// For 32kbps [unstable]
//#define GIO_SEND_MAS_DELAY {int ___i; for(___i=0;___i<20;___i++);}

// For 32kbps [stable]
//#define GIO_SEND_MAS_DELAY {int ___i; for(___i=0;___i<40;___i++);}

// For 128kbps [unstable]
//#define GIO_SEND_MAS_DELAY {int ___i; for(___i=0;___i<5;___i++);}

// For 128kbps [stable]
//#define GIO_SEND_MAS_DELAY {int ___i; for(___i=0;___i<20;___i++);}

// For 128kbps [stable]
#define GIO_SEND_MAS_DELAY {int ___i; for(___i=0;___i<30;___i++);}

#define SEND_TO_MAS(BUFFER,SIZE)                              \
 ({                                                           \
    int  __i;                                                 \
    int __data;                                               \
    char * data_buff=BUFFER->data+BUFFER->read;                                   \
    for(__i=0;__i<SIZE;__i++)                                 \
    {                                                         \
        if(inw(GIO_BITSET0) & (0x1<<GIO_MAS_EOD))             \
            break;  /* EOD set => exit */                     \
        __data=(data_buff[__i] << 8) & 0xFF00;                \
        if(__data!=0)                                         \
            outw(__data,GIO_BITSET0);                         \
        __data ^= 0xFF00;                                     \
        if(__data!=0)                                         \
            outw(__data,GIO_BITCLEAR0);                       \
        /* try to latch data (raise PR) */                    \
        outw(0x1<<(GIO_MAS_PR-16),GIO_BITSET1);               \
        /* wait for RTR to be set */                          \
        while(!(inw(GIO_BITSET1) & (0x1<<(GIO_MAS_RTR-16))))  \
            /*nothing*/;                                      \
        /* clear latch (lower raise PR) */                    \
        outw(0x1<<(GIO_MAS_PR-16),GIO_BITCLEAR1);             \
		GIO_SEND_MAS_DELAY                            \
    }                                                         \
    __i;                                                      \
  })

__IRAM_CODE void dsp_interrupt(int irq,struct pt_regs * regs)
{
    int toSend;
    int send;
    if(current_buffer!=NULL)
    {
        irq_disable(IRQ_MAS_DATA);
        //printk("INT r:%d,w:%d\n",current_buffer->read,current_buffer->write);

        if(current_buffer->read==current_buffer->write) /* nothing in buffer */
        {
            printk("read == write => no more data to play\n");
            return;
        }

        //printk("INT2 r:%d,w:%d\n",current_buffer->read,current_buffer->write);

        if(current_buffer->read>current_buffer->write)   /* write before read => count only size till the end of buffer*/
            toSend=current_buffer->size-current_buffer->read;
        else                                             /* we have less than size-read in buffer */
            toSend=current_buffer->write-current_buffer->read;

        send=SEND_TO_MAS(current_buffer,toSend);

        current_buffer->read+=send;

        //printk("send: %d/%d r:%d,w:%d\n",send,toSend,current_buffer->read,current_buffer->write);


        if(current_buffer->read >= current_buffer->size)  /* we reached end of buffer => go back to start */
        {
           // printk("loop -> read\n");
            current_buffer->read=0;
            dsp_interrupt(IRQ_MAS_DATA,NULL);             /* retry to send data */
        }
        irq_enable(IRQ_MAS_DATA);
    }
    else
    {
        irq_disable(IRQ_MAS_DATA);
    }
}

int mas_stop_mp3_app(void)
{
    int i;
    mas_app_select(MASS_APP_NONE);
    while(1)
    {
        i=mas_app_running(MASS_APP_ANY);
        /*if(i<0)
        {
            printk("error getting app status (trying to stop)\n");
            return 0;
        }*/
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
        /*if(i<0)
        {
            printk("error getting app status (trying to start)\n");
            
            return 0;
        }*/
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
    mas_control_config(MAS_SET,MAS_VOLUME,/*70*/80);

    MAS_DELAY

    printk("mas_stop_mp3_app\n");
    if(!mas_stop_mp3_app())
        return -1;

    mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
    mas_set_clk_speed(0x4800);
    mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);

    MAS_DELAY

    printk("mas_start_mp3_app\n");
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

#include <kernel/stdfs.h>

void init_sound (void)
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
//    playing_sound=0;
//    in_pause=0;

    g_nPaused = 0;
    current_buffer = NULL;

    irq_disable(IRQ_MAS_DATA);
    printk(" done\n");

    /*************************************************************/
    
    //ini_mas_for_mp3();
#if 0   
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
    
    mas_stop_mp3_app();
    
    mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
    mas_set_clk_speed(0x4800);
    mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);
    
    mas_test_PCM();
    
#if 1    
    int mp3ptr=0;
    int fd = open("/out.wav",O_RDONLY);
    if(fd<0)
        printk("Error loading file\n");
    else
    {
        char * mp3Buff = malloc(1024*1024*7);
        int cnt=1;
        int size=filesize(fd);
        /*
        while(cnt>0)
        {
            cnt = read(fd,mp3Buff+size,1024);
            size += cnt;            
        }*/
        size = read(fd,mp3Buff,size);
        printk("Read from file: %x\n",size);
        close(fd);
        //int size=0x46500;
        char * data_buff = mp3Buff;
        int data;
        if(size>0)
        {
            while(1)
            {
                if(inw(GIO_BITSET0) & (0x1<<GIO_MAS_EOD))
                    continue;
                
                data = (data_buff[0] & 0xFF) | ((data_buff[1] & 0xFF)<<8);
                
                data++;
                
                outw(0xFF00,GIO_BITCLEAR0);
                outw(((data & 0xFF)<<8),GIO_BITSET0);
                /* try to latch data (raise PR) */
                outw(0x1<<(GIO_MAS_PR-16),GIO_BITSET1);
                outw(0x1<<(GIO_MAS_PR-16),GIO_BITSET1);
                outw(0x1<<(GIO_MAS_PR-16),GIO_BITCLEAR1);
                
                outw(0xFF00,GIO_BITCLEAR0);
                outw((data & 0xFF00),GIO_BITSET0);
                outw(0x1<<(GIO_MAS_PR-16),GIO_BITSET1);
                
                data_buff+=2;
                if ((data_buff-mp3Buff)>=size) {data_buff=mp3Buff; printk("loop\n"); break;}
                
                outw(0x1<<(GIO_MAS_PR-16),GIO_BITCLEAR1);
            }
        }
        //cnt = mas_pio_write(mp3Buff + mp3ptr, 2000);
        //printk("%d ",cnt); 
        //mp3ptr+=cnt;
        //if (mp3ptr>=65000){mp3ptr=0; printk("loop ");}
        }
#endif
#endif

}
