/*
* asm/arch/av3xx_mas.c
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

#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_i2c.h>

#include <asm/arch/av3xx_mas.h>

int in_16_val(void)
{
	int ret=av3xx_i2c_inb() << 8;
	av3xx_i2c_ack();	
	return ret|av3xx_i2c_inb();
}

int in_32_val(void)
{
	int ret=av3xx_i2c_inb() << 24;
	av3xx_i2c_ack();
	ret|=av3xx_i2c_inb() << 16;
	av3xx_i2c_ack();
	ret|=av3xx_i2c_inb() << 8;
	av3xx_i2c_ack();	
	return ret|av3xx_i2c_inb();
}

#define OUT_WRITE_DEVICE           DO_OUTB(I2C_WRITE_DEVICE(AV3XX_MAS_DEVICE),"I2C - mas","Error, sending write device")
#define OUT_READ_DEVICE            DO_OUTB(I2C_READ_DEVICE(AV3XX_MAS_DEVICE),"I2C - mas","Error, sending read device")
#define OUT_SUBADDRESS(addr)       DO_OUTB(addr,"I2C - mas","Error, sending write device")
#define OUT_16_VAL(val)            DO_OUTB(val>>8,"I2C - mas","Error, sending val [15-8]") \
                               DO_OUTB(val,"I2C - mas","Error, sending val [7-0]")
#define OUT_32_VAL(val)            OUT_16_VAL(val >> 16) OUT_16_VAL(val) 
#define IN_16_VAL                  in_16_val()
#define IN_32_VAL                  in_32_val()

/********************* init mas                    ***************************/
int av3xx_mas_reset(void)
{
	int i;
	outw(0x1,AV3XX_GIO_BITCLEAR1);
	for(i=0;i<0x40000;i++) /* NOTHING */ ;
	outw(0x1,AV3XX_GIO_BITSET1);
	for(i=0;i<0x40000;i++) /* NOTHING */ ;
	av3xx_mas_write_direct_config(MAS_CONTROL,0x8c00);
	return 0;
}

int av3xx_mas_gio_init(void)
{
	outw(((inw(AV3XX_GIO_DIRECTION0) & 0x00FF)|0xF5),AV3XX_GIO_DIRECTION0); // removed | 0xF5
	outw(((inw(AV3XX_GIO_DIRECTION1) & 0x3FFF)|0x4000),AV3XX_GIO_DIRECTION1);
	outw((inw(AV3XX_GIO_INVERT0) & 0xFFEF) | 0x10,AV3XX_GIO_INVERT0); // EOD inverted
	outb((inb(AV3XX_GIO_ENABLE_IRQ) & 0xEF) | 0x10,AV3XX_GIO_ENABLE_IRQ); // EOD irq enabled
	return 0;
}

/********************* app functions                ***************************/
int av3xx_mas_app_select(int app)
{
	return av3xx_mas_set_D0(MAS_APP_SELECT,app);
}

int av3xx_mas_app_running(int app)
{
	unsigned int val=av3xx_mas_get_D0(MAS_APP_SELECT);	
	if(val<0)
		return -1;
	return (val & app);
}

int av3xx_mas_set_clk_speed(int spd)
{
	return av3xx_mas_set_D0(MAS_OSC_FREQ,spd);
}

int av3xx_mas_get_frame_count(void)
{
	return (av3xx_mas_get_D0(MAS_MPEG_FRAME_COUNT)&0xFFFFF);
}

/********************* PIO read/write               ***************************/
int av3xx_mas_pio_read(void * buffer,int maxSize)
{
	return 0;
}

int av3xx_mas_pio_write(void * buffer,int size)
{
	int i,data;
	char * buf=(char*)buffer;
	if(size<=0)
		return 0;
	for(i=0;i<size;i++)
	{
		if(inw(AV3XX_GIO_BITSET0) & 0x0010) // EOD set => exit
		{
			break;
		}
		data=(buf[i] << 8) & 0xFF00;
		if(data!=0)
			outw(data,AV3XX_GIO_BITSET0);
		data ^= 0xFF00;
		if(data!=0)
			outw(data,AV3XX_GIO_BITCLEAR0);
		outw(0x8000,AV3XX_GIO_BITSET1);     // try to latch data (raise PR)
		while(!(inw(AV3XX_GIO_BITSET1) & 0x4000)) /* NOTHING */; // wait for RTR to be set
		outw(0x8000,AV3XX_GIO_BITCLEAR1);   // clear latch (lower raise PR)
	}
	return i;
}

/********************* Direct config i2c read/write ***************************/

int av3xx_mas_read_direct_config(int reg)
{
	int ret=0;	
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(reg)	
	av3xx_i2c_start();
	OUT_READ_DEVICE
	ret=IN_16_VAL;
	av3xx_i2c_notAck();	
	av3xx_i2c_stop();	
	return ret;
}

int av3xx_mas_write_direct_config(int reg,int val)
{
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(reg)
	OUT_16_VAL(val)
	av3xx_i2c_stop();	
	return 0;
}


/********************* Register i2c read/write ***************************/

int av3xx_mas_read_register(int reg)
{
	int ret=0;	
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(0xa000 | ((reg << 4) & 0x0FF0))
	av3xx_i2c_stop();	
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	av3xx_i2c_start();	
	OUT_READ_DEVICE	
	ret= IN_32_VAL & 0x000FFFFF;
	av3xx_i2c_notAck();	
	av3xx_i2c_stop();	
	return ret;
}

int av3xx_mas_write_register(int reg,int val)
{
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_32_VAL(0xb0000000 | ((reg << 20) & 0x0FF00000) | (val & 0x000FFFFF))
	av3xx_i2c_stop();	
	return 0;
}

int av3xx_mas_set_D0(int addr,int val)
{
	return av3xx_mas_write_Di_register(MAS_REGISTER_D0,addr,&val,1);
}

int av3xx_mas_get_D0(int addr)
{
	int ret;
	if( av3xx_mas_read_Di_register(MAS_REGISTER_D0,addr,&ret,1)<0)
		return -1;
	return ret;
}


int av3xx_mas_read_Di_register(int i,int addr,void * buf,int size) // !!! 20 bit values stored as 32 bit
{
	int ret=0;
	int j;
	char * buffer = (char*) buf;
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_READ_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)	
	av3xx_i2c_stop();	
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	av3xx_i2c_start();	
	OUT_READ_DEVICE
	for(j=0;j<size*4;j+=4)
	{
		ret=IN_32_VAL;
		buffer[j+3]=(ret>>24)&0xFF;
		buffer[j+2]=(ret>>16)&0xFF;
		buffer[j+1]=(ret>>8)&0xFF;
		buffer[j]=ret&0xFF;
		if(j+4!=size)
			av3xx_i2c_ack();
	}		
	av3xx_i2c_notAck();	
	av3xx_i2c_stop();	
	return 0;
}

int av3xx_mas_shortRead_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
	int ret=0;
	int j;
	char * buffer = (char*) buf;
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	
	OUT_16_VAL(MAS_SHORTREAD_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)	
	av3xx_i2c_stop();	
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	av3xx_i2c_start();
	OUT_READ_DEVICE
	for(j=0;j<size*2;j+=2)
	{
		ret=IN_16_VAL;
		buffer[j+1]=(ret>>8)&0xFF;
		buffer[j]=(ret)&0xFF;
		if(j+1!=size)
			av3xx_i2c_ack();
	}
	av3xx_i2c_notAck();	
	av3xx_i2c_stop();	
	return 0;
}

int av3xx_mas_write_Di_register(int i,int addr,void * buf,int size) // !!! 20 bit values read as 32 bit
{
	char * buffer = (char*) buf;
	int j;
	unsigned outval;
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_WRITE_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)
	for(j=0;j<size*4;j+=4)
	{
		outval=((buffer[j+3]<<24) & 0xFF000000) 
		          | ((buffer[j+2]<<16) & 0x00FF0000) 
			  | ((buffer[j+1]<<8) & 0x0000FF00) 
			  | ((buffer[j]) & 0x000000FF);	  
		OUT_32_VAL(outval)
	}
	av3xx_i2c_stop();	
	return 0;
}

int av3xx_mas_shortWrite_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
	char * buffer = (char*) buf;
	int j;
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_SHORTWRITE_Di(i))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)
	for(j=0;j<size*2;j+=2)
		OUT_32_VAL(((buffer[j+1]<<8) & 0x0000FF00) | ((buffer[j]) & 0x000000FF))
	av3xx_i2c_stop();	
	return 0;
}

int av3xx_mas_clear_sync(void)
{
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_CLEAR_SYNC)
	av3xx_i2c_stop();	
	return 0;	
}

int av3xx_mas_read_version(struct mas_version * ptr)
{
	int ret;
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_IC_VERSION)
	av3xx_i2c_stop();
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_READ)
	av3xx_i2c_start();
	OUT_READ_DEVICE
	ptr->major_number=IN_16_VAL;
	av3xx_i2c_ack();
	ret=IN_16_VAL;
	av3xx_i2c_notAck();	
	av3xx_i2c_stop();
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

int av3xx_mas_control_config(int action,int control,int val)
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
			return av3xx_mas_control_write(control,ret);
		case MAS_GET:
			ret=av3xx_mas_control_read(control);
			if(ret<0)
				return -1;
			return convertVal(ret,control,action);
		default:
			printk("[mas_codec_config_bit] Wrong action\n");
	}
	return -1;
}

int av3xx_mas_control_write(int control,int val)
{
	int tmpVal;
	switch(control)
	{
		case MAS_LOUDNESS:
			if((tmpVal=av3xx_mas_read_codec(control))<0)
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
			if((tmpVal=av3xx_mas_read_codec(control))<0)
				return -1;
			val=((val&0xF)<<12)  | (tmpVal & 0x0FFF);
			break;
		case MAS_ADC_L_GAIN:
			if((tmpVal=av3xx_mas_read_codec(control))<0)
				return -1;
			val=((val&0xF)<<8)  | (tmpVal & 0xF0FF);
			break;
		case MAS_ADC_R_GAIN:
			if((tmpVal=av3xx_mas_read_codec(control))<0)
				return -1;
			val=((val&0xF)<<4)  | (tmpVal & 0xFF0F);
			break;
		default:
			return -1;
	}	
	return av3xx_mas_write_codec(reg_addr[control],val);
}

int av3xx_mas_control_read(int control)
{
	int ret;
	switch(control)
	{
		case MAS_VOLUME:
		case MAS_BASS:
		case MAS_TREBLE:		
		case MAS_BALANCE:
		case MAS_LOUDNESS:
			if((ret=av3xx_mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>8)&0xFF;
			break;
		case MAS_MICRO_GAIN:
			if((ret=av3xx_mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>12)&0xF;
			break;
		case MAS_ADC_L_GAIN:
			if((ret=av3xx_mas_read_codec(reg_addr[control]))<0)
				return -1;
			ret=(ret>>8)&0xF;
			break;
		case MAS_ADC_R_GAIN:
			if((ret=av3xx_mas_read_codec(reg_addr[control]))<0)
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

int av3xx_mas_read_codec(int reg)
{
	int ret=0;	
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_CODEC_WRITE)
	OUT_16_VAL(reg)		
	av3xx_i2c_stop();		
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_CODEC_READ)
	av3xx_i2c_start();
	OUT_READ_DEVICE
	ret=IN_16_VAL;
	av3xx_i2c_notAck();	
	av3xx_i2c_stop();	
	return ret;
}

int av3xx_mas_write_codec(int reg,int val)
{
	av3xx_i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_CODEC_WRITE)
	OUT_16_VAL(reg)
	OUT_16_VAL(val)
	av3xx_i2c_stop();	
	return 0;
}
