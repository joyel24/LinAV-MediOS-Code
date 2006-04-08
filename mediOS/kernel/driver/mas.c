/* 
*   kernel/driver/mas.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/i2c.h>
#include <kernel/gio.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/mas.h>

int in_16_val(void)
{
	int ret=i2c_inb() << 8;
	i2c_ack();	
	return ret|i2c_inb();
}

int in_32_val(void)
{
	int ret=i2c_inb() << 24;
	i2c_ack();
	ret|=i2c_inb() << 16;
	i2c_ack();
	ret|=i2c_inb() << 8;
	i2c_ack();	
	return ret|i2c_inb();
}

#define OUT_WRITE_DEVICE           i2c_outb((I2C_WRITE_DEVICE(MAS_DEVICE)));
#define OUT_READ_DEVICE            i2c_outb((I2C_READ_DEVICE(MAS_DEVICE)));
#define OUT_SUBADDRESS(addr)       i2c_outb((addr));
#define OUT_16_VAL(val)            i2c_outb(((val)>>8)&0xFF); \
                                       i2c_outb((val)&0xFF);
#define OUT_32_VAL(val)            OUT_16_VAL((val) >> 16) OUT_16_VAL((val)); 
#define IN_16_VAL                  in_16_val()
#define IN_32_VAL                  in_32_val()

/********************* init mas                    ***************************/
int mas_reset(void)
{
    GIO_CLEAR(GIO_MAS_PWR);  // stop the MAS
    mdelay(100);
    GIO_SET(GIO_MAS_PWR);       // start the MAS
    mdelay(10);
    mas_write_direct_config(MAS_CONTROL,0x8c00);
    return 0;
}

int mas_gio_init(void)
{
    int i;
    GIO_DIRECTION(GIO_MAS_EOD,GIO_IN);
    for(i=0;i<8;i++)
        GIO_DIRECTION((GIO_MAS_D0+i),GIO_OUT);
    GIO_DIRECTION(GIO_MAS_PWR,GIO_OUT);   
    GIO_DIRECTION(GIO_MAS_RTR,GIO_IN);
    GIO_DIRECTION(GIO_MAS_PR,GIO_OUT);

    GIO_INVERT(GIO_MAS_EOD,GIO_INVERTED);
    GIO_IRQ_ENABLE(GIO_MAS_EOD,GIO_IRQ);

    return 0;
}

/********************* app functions                ***************************/
int mas_app_select(int app)
{
	return mas_set_D0(MAS_APP_SELECT,app);
}

int mas_app_running(int app)
{
	unsigned int val=mas_get_D0(MAS_APP_SELECT);	
	if(val<0)
		return -1;
	return (val & app);
}

int mas_set_clk_speed(int spd)
{
	return mas_set_D0(MAS_OSC_FREQ,spd);
}

int mas_get_frame_count(void)
{
	return (mas_get_D0(MAS_MPEG_FRAME_COUNT)&0xFFFFF);
}

/********************* PIO read/write               ***************************/
int mas_pio_read(void * buffer,int maxSize)
{
	return 0;
}

/********************* Direct config i2c read/write ***************************/

int mas_read_direct_config(int reg)
{
	int ret=0;
        //i2c_ini_xfer();	
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(reg)
	i2c_start();
	OUT_READ_DEVICE
	ret=IN_16_VAL;
	i2c_notAck();	
	i2c_stop();	
	return ret;
}

int mas_write_direct_config(int reg,int val)
{
    //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(reg)
	OUT_16_VAL(val)
	i2c_stop();	
	return 0;
}


/********************* Register i2c read/write ***************************/

int mas_read_register(int reg)
{
	int ret=0;
        int ret2=0;
        //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(0xa000 | ((reg<<4)  & 0x0FF0))
	i2c_stop();	
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	i2c_start();	
	OUT_READ_DEVICE	
        ret = IN_16_VAL;
        ret = (ret&0xF)<<16;
        i2c_ack();
        ret2 = IN_16_VAL;
        ret |= (ret2 & 0xFFFF);
	i2c_notAck();	
	i2c_stop();	
	return ret;
}

int mas_write_register(int reg,int val)
{
    //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0xB000 | ((reg&0xFF)<<4) | ((val>>16)&0xf))
    OUT_16_VAL(val&0xFFFF)
    i2c_stop();
    return 0;
}

int mas_freeze(void)
{
    //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0x0000)
    i2c_stop();
    return 0;
}

int mas_run(void)
{
    //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0x1000)
    i2c_stop();
    return 0;
}

int mas_set_D0(int addr,int val)
{
	return mas_write_Di_register(MAS_REGISTER_D0,addr,&val,1);
}

int mas_get_D0(int addr)
{
	unsigned int ret;
	if( mas_read_Di_register(MAS_REGISTER_D0,addr,&ret,1)<0)
		return -1;
        
	return ret;
}


int mas_read_Di_register(int i,int addr,void * buf,int size) // !!! 20 bit values stored as 32 bit
{
	int ret=0;
	int j;
	char * buffer = (char*) buf;
        //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_READ_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)	
	i2c_stop();	
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	i2c_start();	
	OUT_READ_DEVICE
	for(j=0;j<size*4;j+=4)
	{
		ret=IN_32_VAL;
		/*buffer[j+3]=(ret>>24)&0xFF;
		buffer[j+2]=(ret>>16)&0xFF;
		buffer[j+1]=(ret>>8)&0xFF;
		buffer[j]=ret&0xFF;*/
                /*buffer[j+3]=(ret>>24)&0xFF;
                buffer[j+2]=(ret>>16)&0xFF;
		buffer[j+1]=(ret>>8)&0x0F;
		buffer[j]=0;*/
                buffer[j+3]=0;
		buffer[j+2]=(ret>>16)&0x0F;
		buffer[j+1]=(ret>>8)&0xFF;
		buffer[j]=ret&0xFF;
		if(j+4!=size)
			i2c_ack();
	}		
	i2c_notAck();	
	i2c_stop();	
	return 0;
}

int mas_shortRead_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
	int ret=0;
	int j;
	char * buffer = (char*) buf;
        //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	
	OUT_16_VAL(MAS_SHORTREAD_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)	
	i2c_stop();	
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	i2c_start();
	OUT_READ_DEVICE
	for(j=0;j<size*2;j+=2)
	{
		ret=IN_16_VAL;
		buffer[j+1]=(ret>>8)&0xFF;
		buffer[j]=(ret)&0xFF;
		if(j+1!=size)
			i2c_ack();
	}
	i2c_notAck();	
	i2c_stop();	
	return 0;
}

int mas_write_Di_register(int i,int addr,void * buf,int size) // !!! 20 bit values read as 32 bit
{
	char * buffer = (char*) buf;
	int j;
	unsigned long outval;
        //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_WRITE_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)
	for(j=0;j<size*4;j+=4)
	{
		outval=((buffer[j+3]<<24) & 0x00000000) 
		          | ((buffer[j+2]<<16) & 0x000F0000) 
			  | ((buffer[j+1]<<8) & 0x0000FF00) 
			  | ((buffer[j]) & 0x000000FF);	  
		OUT_32_VAL(outval)
	}
	i2c_stop();	
	return 0;
}

int mas_shortWrite_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
	char * buffer = (char*) buf;
	int j;
        //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_SHORTWRITE_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)
	for(j=0;j<size*2;j+=2)
		OUT_32_VAL(((buffer[j+1]<<8) & 0x0000FF00) | ((buffer[j]) & 0x000000FF))
	i2c_stop();	
	return 0;
}

int mas_clear_sync(void)
{
    //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_CLEAR_SYNC)
	i2c_stop();	
	return 0;	
}

int mas_read_version(struct mas_version * ptr)
{
	int ret;
        //i2c_ini_xfer();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_IC_VERSION)
	i2c_stop();
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	i2c_start();
	OUT_READ_DEVICE
	ptr->major_number=IN_16_VAL;
	i2c_ack();
	ret=IN_16_VAL;
	i2c_notAck();	
	i2c_stop();
	ptr->derivate=(ret >> 12) & 0xF;
	ptr->char_order_version=((ret>>4) & 0xFF)+0x41; // it's a char !
	ptr->digit_order_version=ret&0xF;	
	return 0;
}

/********************* Codec function       ***************************/

int reg_addr[]={
	MAS_REG_VOLUME,
	MAS_REG_BASS,
	MAS_REG_TREBLE,
	MAS_REG_LOUDNESS,
	MAS_REG_BALANCE,
	MAS_REG_AUDIO_CONF,
	MAS_REG_AUDIO_CONF,
	MAS_REG_AUDIO_CONF
};

int mas_control_config(int action,int control,int val)
{
	int ret=0;
	switch (action)
	{
		case MAS_SET:
			if(val<0 || val >100)
				return -1;
			ret=convertVal(val,control,action);
			if(ret<0)
				return ret;
			return mas_control_write(control,ret);
		case MAS_GET:
			ret=mas_control_read(control);
			if(ret<0)
				return -1;
			return convertVal(ret,control,action);
		default:
			printk("[mas_codec_config_bit] Wrong action\n");
	}
	return -1;
}

int mas_control_write(int control,int val)
{
	int tmpVal;
	switch(control)
	{
		case MAS_LOUDNESS:
			if((tmpVal=mas_read_codec(control))<0)
				return -1;
			val=(val<<8) | (tmpVal & 0x00FF);
			break;
		case MAS_VOLUME:
		case MAS_BASS:
		case MAS_TREBLE:
		case MAS_BALANCE:
			val<<=8;
			break;
		case MAS_MICRO_GAIN:
			if((tmpVal=mas_read_codec(control))<0)
				return -1;
			val=((val&0xF)<<12)  | (tmpVal & 0x0FFF);
			break;
		case MAS_ADC_L_GAIN:
			if((tmpVal=mas_read_codec(control))<0)
				return -1;
			val=((val&0xF)<<8)  | (tmpVal & 0xF0FF);
			break;
		case MAS_ADC_R_GAIN:
			if((tmpVal=mas_read_codec(control))<0)
				return -1;
			val=((val&0xF)<<4)  | (tmpVal & 0xFF0F);
			break;
		default:
			return -1;
	}	
	return mas_write_codec(reg_addr[control],val);
}

int mas_control_read(int control)
{
	int ret;
	switch(control)
	{
		case MAS_VOLUME:
		case MAS_BASS:
		case MAS_TREBLE:		
		case MAS_BALANCE:
		case MAS_LOUDNESS:
			if((ret=mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>8)&0xFF;
			break;
		case MAS_MICRO_GAIN:
			if((ret=mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>12)&0xF;
			break;
		case MAS_ADC_L_GAIN:
			if((ret=mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>8)&0xF;
			break;
		case MAS_ADC_R_GAIN:
			if((ret=mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>4)&0xF;
			break;
		default:
			return -1;
	}
	return ret;
}


int convertVal(int val,int control,int action)
{
	int a,b;
	switch(control)
	{
		case MAS_VOLUME:
			a=0x7F;
			b=0;
			break;
		case MAS_BASS:
		case MAS_TREBLE:
			if(val <50)
			{
				a=180;
				b=160;
			}
			else
			{
				a=192;
				b=-96;
			}
			break;
		case MAS_LOUDNESS:
			a=0x44;
			b=0;
			break;
		case MAS_BALANCE:
			if(val <50)
			{
				a=260;
				b=128;
			}
			else
			{
				a=254;
				b=-127;
			}
			break;
		case MAS_MICRO_GAIN:
		case MAS_ADC_L_GAIN:
		case MAS_ADC_R_GAIN:
			a=0xF;
			b=0;
			break;
		default:
			return -1;
	}
	if(action == MAS_SET)
	{
		val=(val*a)/100+b;
	}
	else
	{
		val=((val-b)/a)*100;
		if(val>100)
			val=100;
		if(val<0)
			val=0;
	}
	
	return val;
}

/********************* Codec i2c read/write ***************************/

int mas_read_codec(int reg)
{
	int ret=0;	
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_CODEC_WRITE)
	OUT_16_VAL(reg)		
	i2c_stop();		
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_CODEC_READ)
	i2c_start();
	OUT_READ_DEVICE
	ret=IN_16_VAL;
	i2c_notAck();	
	i2c_stop();	
	return ret;
}

int mas_write_codec(int reg,int val)
{
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_CODEC_WRITE)
	OUT_16_VAL(reg)
	OUT_16_VAL(val)
	i2c_stop();	
	return 0;
}

/*********************  PCM  code       ***************************/
#if 0

#include "mas_code/mas_pcm_struct.h"
#include "mas_code/d0_640_1e.h"
#include "mas_code/d0_674_51.h"
#include "mas_code/d0_661_13.h"
#include "mas_code/d0_6e4_10.h"
#include "mas_code/d0_700_12.h"
#include "mas_code/d1_600_3f.h"
#include "mas_code/d1_640_c.h"
#include "mas_code/d1_660_1c.h"
#include "mas_code/d0_800_684.h"
#include "mas_code/d1_800_435.h"

#define NB_CHUNK 10

struct mas_pcm_struct * chunk_list[NB_CHUNK] = {
    &D0_640_1e,
    &D0_674_51,
    &D0_661_13,
    &D0_6e4_10,
    &D0_700_12,
    &D1_600_3f,
    &D1_640_c,
    &D1_660_1c,
    &D0_800_684,
    &D1_800_435
};

int mas_load_PCM_code(void)
{
    int i,val;
    for(i=0;i<NB_CHUNK;i++)
    {
        printk("loading %s(%x@%x,%x)\n",chunk_list[i]->name,chunk_list[i]->reg,chunk_list[i]->addr,chunk_list[i]->length);
        mas_write_Di_register(chunk_list[i]->reg,chunk_list[i]->addr,(void *)chunk_list[i]->buffer,chunk_list[i]->length);
        mas_read_Di_register(chunk_list[i]->reg,chunk_list[i]->addr,&val,1);
        printk("read back : @%x:%x get: %x\n",chunk_list[i]->reg,chunk_list[i]->addr,val);
    }
    return 0;
}

int mas_stop_data[8][2] = {
    { 0x3B,0x00318 },
    { 0x43,0x00300 },
    { 0x4B,0x00000 },
    { 0x53,0x00318 },
    { 0x6B,0x00000 },
    { 0xBB,0x00318 },
    { 0xC3,0x00300 },
    { 0x06,0x00000 }
};

void mas_stop_app(void)
{
    int i,val,val2;
    mas_freeze();
    for(i=0;i<8;i++)
    {
        val2 = mas_read_register(mas_stop_data[i][0]);
        mas_write_register(mas_stop_data[i][0],mas_stop_data[i][1]);
        val = mas_read_register(mas_stop_data[i][0]);
        printk("READ back reg : %x=%x was %x, should be %x\n",mas_stop_data[i][0],val,val2,mas_stop_data[i][1]);
    }
}



void mas_run_app(void)
{
    int val=0;
    int i=0;
    
    mas_write_register(0x6B,0xC0000);
    mas_run();
    
    
    val = 0x125;
    mas_write_Di_register(MAS_REGISTER_D0,0x661,&val,1);
    printk("bf loop1\n");
    while(1)
    {
        val=0;
        mas_read_Di_register(MAS_REGISTER_D0,0x666,&val,1);
        if(val==0)
            break;
        printk("APP get: %x\n",val);
    }
    printk("af loop1\n");
    val = 0x8300;
    mas_write_Di_register(MAS_REGISTER_D0,0x7f8,&val,1);
    val = 0x125;
    mas_write_Di_register(MAS_REGISTER_D0,0x661,&val,1);
    printk("bf loop2\n");
    while(1)
    {
        val=0;
        mas_read_Di_register(MAS_REGISTER_D0,0x666,&val,1);
        if(val==0)
            break;
        printk("APP get: %x\n",val);
        
    }
    printk("af loop2\n");
    val = 0x0069;
    mas_write_Di_register(MAS_REGISTER_D0,0x66b,&val,1);
    val = 0x1125;
    mas_write_Di_register(MAS_REGISTER_D0,0x661,&val,1);
    printk("bf loop2\n");
    while(1)
    {
        val=0;
        i++;
        mas_read_Di_register(MAS_REGISTER_D0,0x666,&val,1);
        if(val==0 || i>10)
            break;
        printk("APP get: %x\n",val);
    }
    if(val!=0)
        printk("APP get: bad val\n");
    else
        printk("APP get: ok\n");
        //while(1);
    printk("af loop2\n");
    printk("all ok\n");
    
}



int mas_test_PCM(void)
{
    printk("\t\tStopping\n");    
    mas_stop_app();
    printk("\t\tDownloading\n");
    mas_load_PCM_code();
    printk("\t\tRuning\n");
    mas_run_app();
    
    
    
    return 0;
}
#endif
