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

#include <kernel/threads.h>

#define MAS_DELAY        {int ___i; for(___i=0;___i<20;___i++);}
#define MIN(a, b)        (((a)<(b))?(a):(b))
#define MAX(a, b)        (((a)<(b))?(b):(a))

/********************* DSP                    ***************************/


//__IRAM_DATA int playing_sound=0;   // 1 if we are sending data to MAS, 0 if no more data to send or Stop mode or Pause mode
__IRAM_DATA long g_nPaused = 0;     // 1 if in Pause mode

__IRAM_DATA sound_buffer_s * g_CurrentBuffer;

CRITSEC_INFO* g_pCS_SOUND = 0;

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
    char * data_buff=BUFFER->data+BUFFER->bytes_played;       \
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
		GIO_SEND_MAS_DELAY\
    }                                                         \
    __i;                                                      \
  })

__IRAM_CODE void dsp_interrupt(int irq)
{
	int bytes_sent, to_send;

	if (!g_CurrentBuffer)
	{
//		disable_irq(IRQ_MAS_DATA);
		return;
	}

	if (!g_nPaused)
	{
//		printk ("WR\n");

		to_send = g_CurrentBuffer->size - g_CurrentBuffer->bytes_played;
		bytes_sent = SEND_TO_MAS (g_CurrentBuffer, to_send);
		g_CurrentBuffer->bytes_played += bytes_sent;

		if (g_CurrentBuffer->bytes_played >= g_CurrentBuffer->size)
		{
			g_CurrentBuffer->loops_played ++;

			if (g_CurrentBuffer->loops_played >= g_CurrentBuffer->loop_counter)
			{
				g_CurrentBuffer = g_CurrentBuffer->next_buffer;
				g_CurrentBuffer->loops_played = 0;
			}

			if (g_CurrentBuffer)
				g_CurrentBuffer->bytes_played = 0;

//			printk ("SW\n");
			dsp_interrupt (IRQ_MAS_DATA);
		}
	}


#if 0
    int toSend;
    int send;
    char * buffer;
    if(playing_sound && current_buffer!=NULL)
    {
        if(current_buffer->read==current_buffer->write) /* nothing in buffer */
        {
            disable_irq(IRQ_MAS_DATA);
            printf("read == write => no more data to play\n");
            return;
        }
        
        if(current_buffer->read>current_buffer->write)   /* write before read => count only size till the end of buffer*/
            toSend=current_buffer->size-current_buffer->read;
        else                                             /* we have less than size-read in buffer */
            toSend=current_buffer->write-current_buffer->read; 
        
        send=SEND_TO_MAS(current_buffer,toSend);
        
        current_buffer->read+=send;
        
        if(current_buffer->read >= current_buffer->size)  /* we reached end of buffer => go back to start */
        {
//            printf("Roll\n");
            current_buffer->read=0;
            dsp_interrupt(IRQ_MAS_DATA);             /* retry to send data */
        }
    }
    else
    {
        disable_irq(IRQ_MAS_DATA);
    }
#endif
}


#if 0
__IRAM_CODE int sound_buff_write(sound_buffer_s * sound_buffer, int (*reader_fct)(char * data,int count,void* param),
            int count,void * param)
{
    int need_wake_up=0;
    int toSend,send;
    int totSend=0;
    
    if(current_buffer->read==current_buffer->write)
        need_wake_up=1;
        
    if(count>0)
    {
        
        if(sound_buffer->write > sound_buffer->read)
        {
            toSend=sound_buffer->size-sound_buffer->write;
            toSend=MIN(count,toSend);
            send=reader_fct(sound_buffer->data+sound_buffer->write,toSend,param);
            count-=send;
            totSend+=send;
            
            if(send<toSend)     /* haven't write enough => go out */
            {
                sound_buffer->write+=send;
                return totSend;
            }
            
            if(count>0)
            {
                sound_buffer->write=0;
                toSend=sound_buffer->read-sound_buffer->write;
                toSend=MIN(count,toSend);
                send=reader_fct(sound_buffer->data+sound_buffer->write,toSend,param);
                totSend+=send;
            }
            
            sound_buffer->write+=send;
        }
        else
        {
            toSend=sound_buffer->size-sound_buffer->read;
            toSend=MIN(count,toSend);
            send=reader_fct(sound_buffer->data+sound_buffer->write,toSend,param);
            totSend+=send;
            sound_buffer->write+=send;
        }
        
        if(need_wake_up && !(inw(GIO_BITSET0) & (0x1<<GIO_MAS_EOD)))
        {
            enable_irq(IRQ_MAS_DATA);
            dsp_interrupt(IRQ_MAS_DATA);  
        }
        
    }
    
    return totSend;
}
#endif

__IRAM_CODE void dsp_ctl(unsigned int cmd, void * arg)
{
    int * val;
    struct av_peak * av_p;

	API_CRITSEC_ENTER ((HCRITSEC)g_pCS_SOUND);

    switch(cmd)
    {
        case DSP_INI_MP3:
            ini_mas_for_mp3();
//            current_buffer=(sound_buffer_s *)arg;
//            playing_sound=1;
//            in_pause=0;
	enable_irq(IRQ_MAS_DATA);
            break;

		case DSP_SETCURR_MP3_BUFFER:
			g_CurrentBuffer = (sound_buffer_s *)arg;
			break;

		case DSP_GETCURR_MP3_BUFFER:
			break;

		case DSP_ADD_MP3_BUFFER:
			break;

		case DSP_REMOVE_MP3_BUFFER:
			break;

		case DSP_START_MP3:
/*
            if(g_nPaused)
            {
//                playing_sound=1;
                g_nPaused = 0;
                dsp_interrupt(IRQ_MAS_DATA);
            }
            else
            {
//                playing_sound=1;
                enable_irq(IRQ_MAS_DATA);
                dsp_interrupt(IRQ_MAS_DATA);                
            }

            }
*/

			g_nPaused = 0;
			enable_irq(IRQ_MAS_DATA);

//			mas_write_direct_config (MAS_CONTROL,0x8C00);
//			mas_write_direct_config (MAS_CONTROL,0x0C00);

//			mas_start_mp3_app ();

//			printk("MP3 playing\n");

			__cli ();
			dsp_interrupt (IRQ_MAS_DATA);
			__sti ();


			break;

        case DSP_STOP_MP3:
            g_nPaused = 1;
            disable_irq (IRQ_MAS_DATA);
            break;

        case DSP_PAUSE_MP3:
            g_nPaused = 1;
//            in_pause=1;
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

	API_CRITSEC_LEAVE ((HCRITSEC)g_pCS_SOUND);
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

	API_CRITSEC_ENTER ((HCRITSEC)g_pCS_SOUND);

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

	API_CRITSEC_LEAVE ((HCRITSEC)g_pCS_SOUND);
}


/********************* OSS init               ***************************/

#include <kernel/kfile.h>

void init_sound (void)
{
    struct mas_version version;

    printk("[init] Loading av3xx sound driver: ");

    HCRITSEC hSec = 0;
    API_CRITSEC_CREATE (&hSec);
    g_pCS_SOUND = (CRITSEC_INFO*)hSec;

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
    g_CurrentBuffer = NULL;

    disable_irq (IRQ_MAS_DATA);
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
    
    
    /*mas_set_D0(MAS_INTERFACE_CONTROL,0x04);
    mas_set_clk_speed(0x4800);
    mas_set_D0(MAS_MAIN_IO_CONTROL,0x125);*/
    
    mas_test_PCM();
    
    
    int mp3ptr=0;
    int fd = kfopen("/data.bin",O_RDONLY);
    if(fd<0)
        printk("Error loading file\n");
    else
    {
        char * mp3Buff = kmalloc(1024*1024*7);
        int cnt=1;
        int size=kfilesize(fd);
        /*
        while(cnt>0)
        {
            cnt = kfread(fd,mp3Buff+size,1024);
            size += cnt;            
        }*/
        size = kfread(fd,mp3Buff,size);
        printk("Read from file: %x\n",size);
        kfclose(fd);
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
                if ((data_buff-mp3Buff)>=size) {data_buff=mp3Buff; printk("loop\n");}
                
                outw(0x1<<(GIO_MAS_PR-16),GIO_BITCLEAR1);
            }
        }
        cnt = mas_pio_write(mp3Buff + mp3ptr, 2000);
        printk("%d ",cnt); 
        //mp3ptr+=cnt;
        //if (mp3ptr>=65000){mp3ptr=0; printk("loop ");}
#endif

}
