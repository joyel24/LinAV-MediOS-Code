/* 
*   kernel/driver/mas.c
*
*   AMOS project
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

#define OUT_WRITE_DEVICE           i2c_outb(I2C_WRITE_DEVICE(MAS_DEVICE));
#define OUT_READ_DEVICE            i2c_outb(I2C_READ_DEVICE(MAS_DEVICE));
#define OUT_SUBADDRESS(addr)       i2c_outb(addr);
#define OUT_16_VAL(val)            i2c_outb(val>>8); \
                                       i2c_outb(val);
#define OUT_32_VAL(val)            OUT_16_VAL(val >> 16) OUT_16_VAL(val); 
#define IN_16_VAL                  in_16_val()
#define IN_32_VAL                  in_32_val()

/********************* init mas                    ***************************/
int mas_reset(void)
{
    //int i;
    gio_clear(GIO_MAS_PWR);      // stop the MAS
    //for(i=0;i<0x40000;i++) /* NOTHING */ ;
    mdelay(100);
    gio_set(GIO_MAS_PWR);       // start the MAS
    //for(i=0;i<0x40000;i++) /* NOTHING */ ;
    mdelay(10);
    mas_write_direct_config(MAS_CONTROL,0x8c00);
    //mdelay(100);
    return 0;
}

int mas_gio_init(void)
{
    int i;
    gio_dir(GIO_MAS_EOD,GIO_IN);
    for(i=0;i<8;i++)
        gio_dir((GIO_MAS_D0+i),GIO_OUT);
    gio_dir(GIO_MAS_PWR,GIO_OUT);   
    gio_dir(GIO_MAS_RTR,GIO_IN);
    gio_dir(GIO_MAS_PR,GIO_OUT);
    
    gio_inv(GIO_MAS_EOD,GIO_INVERTED);   
    gio_IRQ(GIO_MAS_EOD,GIO_IRQ);

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

/********************* Direct config i2c read/write ***************************/

int mas_read_direct_config(int reg)
{
	int ret=0;	
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
	ret= IN_32_VAL & 0x000FFFFF;
	i2c_notAck();	
	i2c_stop();	
	return ret;
}

int mas_write_register(int reg,int val)
{
	i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_32_VAL(0xb0000000 | ((reg << 20) & 0x0FF00000) | (val & 0x000FFFFF))
	i2c_stop();	
	return 0;
}

int mas_freeze(void)
{
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0x0000)
    i2c_stop();
    return 0;
}

int mas_run(void)
{
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
		buffer[j+3]=(ret>>24)&0xFF;
		buffer[j+2]=(ret>>16)&0xFF;
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
	i2c_start();
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
	i2c_stop();	
	return 0;
}

int mas_shortWrite_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
	char * buffer = (char*) buf;
	int j;
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
int tst_write(int dest,int addr,int size,char * buffer)
{
        int j;
        i2c_start();
	OUT_WRITE_DEVICE
	OUT_SUBADDRESS(MAS_DATA_WRITE)
	OUT_16_VAL(MAS_WRITE_Di(dest))
	OUT_16_VAL(size)
	OUT_16_VAL(addr)

	for(j=0;j<size*4;j++)
	{
            i2c_outb(buffer[j]&0xff);
 	}
	i2c_stop();
        return 0;
}

int tst_read(int dest,int addr,int size,char * buffer)     	
{
    int j,ret;
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(MAS_READ_Di(dest))
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
            buffer[j]=(ret>>24)&0xFF;
            buffer[j+1]=(ret>>16)&0xFF;
            buffer[j+2]=(ret>>8)&0xFF;
            buffer[j+3]=ret&0xFF;
            if(j+4!=size)
                    i2c_ack();
    }		
    i2c_notAck();	
    i2c_stop();	
}

int do_write(char *buffer,int size)
{
    int i,j;
    i2c_start();
    i2c_outb(0x3c);
    i2c_outb(0x68);
    for(j=0;j<size;j++)
    {
        if(j<8)
            printk("%02x",buffer[j]);
        if(i2c_outb(buffer[j])==1)
            printk("Err ");
    }
    i2c_stop();
    printk("\nsend: %x s=%x\n",j,size);
}

int mas_load_PCM_code(void)
{
    int i,j,dest,addr,size,err;
    char * buffer;
    char tmp[2000];
    for(i=0;i<nb_wav_chunk;i++)
    {
        buffer=wav_chunks[i];        
#if 0
        if(buffer[0]==0xE0)
            dest=0;
        else
            dest=1;
        addr=(buffer[4]&0xFF)<<8 | (buffer[5]&0xFF);
        size=(buffer[2]&0xFF)<<8 | (buffer[3]&0xFF);
        if(i==0)
        {
            size=0x600;
            i2c_start();
            OUT_WRITE_DEVICE
            OUT_SUBADDRESS(MAS_DATA_WRITE)            
            OUT_16_VAL(MAS_WRITE_Di(0))
           i2c_outb(0xE0);
            //OUT_16_VAL(size)
            //OUT_16_VAL(0)
            i2c_stop();
            printk("Special Chunk 0\n");
            
        }
        else
        {
            printk("Chunk %d: dest:D%d addr:%04x size:%04x \n",i,dest,addr,size);
            buffer=buffer+6;
            tst_write(dest,addr,size,buffer);
            //mas_write_Di_register(dest,addr,buffer,size);
        }
#else
        size=wav_chunk_size[i];
        do_write(buffer,size);
#endif
        
       
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

char stop_data1[] = {0x00,0x00 };
char stop_data2[] = {0xB3,0xB0,0x03,0x18 };
char stop_data3[] = {0xB4,0x30,0x03,0x00 };
char stop_data4[] = {0xB4,0xB0,0x00,0x00 };
char stop_data5[] = {0xB5,0x30,0x03,0x18 };
char stop_data6[] = {0xB6,0xB0,0x00,0x00 };
char stop_data7[] = {0xBB,0xB0,0x03,0x18 };
char stop_data8[] = {0xBC,0x30,0x03,0x00 };
char stop_data9[] = {0xB0,0x60,0x00,0x00 };

void mas_stop_app(void)
{
    /*int i;
    mas_freeze();
    for(i=0;i<8;i++)
    {
        mas_write_register(mas_stop_data[i][0],mas_stop_data[i][1]);        
    }*/
    do_write(stop_data1,2);
    do_write(stop_data2,4);
    do_write(stop_data3,4);
    do_write(stop_data4,4);
    do_write(stop_data5,4);
    do_write(stop_data6,4);
    do_write(stop_data7,4);
    do_write(stop_data8,4);
    do_write(stop_data9,4);
    
}

void iniData(char * data,int val)
{
    int i;
    for(i=3;i>=0;i--)
    {
        data[i]= val & 0xFF;
        val=val >> 8;
        
    }
    /*for(i=0;i<4;i++)
        printk("%02x",data[i]);*/
    printk("\n");
}

int getData(char * data)
{
    return ((data[3]<<24) & 0xFF000000) | ((data[2]<<16) & 0x00FF0000) | ((data[1]<<8) & 0x0000FF00) | (data[0]& 0x000000FF);
}

char run_data1[]={0xB6,0xBC,0x00,0x00};
char run_data2[]={0x10,0x00};

void mas_run_app(void)
{
    char data[4];
    printk("stp 1 ");
    //mas_write_register(0x6B,0xC0000);
    do_write(run_data1,4);
    printk("2 ");
    do_write(run_data2,2);
    //mas_run();
    printk("3 ");
    iniData(data,0x125);
    //tst_write(0,0x661,0x1,data);
    mas_write_Di_register(0,0x661,data,0x1);
    printk("4 ");
    iniData(data,0x0);
    //tst_write(0,0x347,0x1,data);
    mas_write_Di_register(0,0x347,data,0x1);
    printk("5 ");
    iniData(data,0x4E5E);
    //tst_write(0,0x348,0x1,data);
    mas_write_Di_register(0,0x348,data,0x1);
    printk("6 ");
    iniData(data,0x01A5);
    //tst_write(0,0x344,0x1,data);
    mas_write_Di_register(0,0x344,data,0x1);
}

void mas_run_PCM(void)
{
    int res;
    char data[4];
    
    int i;
    //while(1)
//     {
        iniData(data,0x0125);
        //tst_write(0,0x661,0x1,data);
        mas_write_Di_register(0,0x661,data,0x1);
        for(i=0;i<5;i++)
        {
            iniData(data,0x0);
            //tst_read(0,0x666,0x1,data);
            mas_read_Di_register(0,0x666,data,0x1);
            printk("res=%x\n",getData(data));
           if(getData(data)==0)
                break;
        }
//         if(getData(data)==0)
//                 break;
//         printk("[MAS RUN] Retry stopping\n");
//      }
     
     printk("[MAS RUN] mas stopped\n");
     
     iniData(data,0x61);
     //tst_write(0,0x66B,0x1,data);
     mas_write_Di_register(0,0x66B,data,0x1);
     
     iniData(data,0x1125);
     //tst_write(0,0x661,0x1,data);
     mas_write_Di_register(0,0x661,data,0x1);
     
     iniData(data,0x0);
     //tst_read(0,0x661,0x1,data);
     mas_read_Di_register(0,0x661,data,0x1);
     printk("IOC %x\n",getData(data));
     for(i=0;i<0xff;i++)
     {
     iniData(data,0x0);
     //tst_read(0,0x666,0x1,data);
     mas_read_Di_register(0,0x666,data,0x1);
     printk("APP run %x ",getData(data));
     }
     printk("\n");
}

int mas_test_PCM(void)
{
    int i;
    
    
    printk("\t\tStopping\n");    
    mas_stop_app();
    printk("\t\tDownloading\n");
    mas_load_PCM_code();
    printk("\t\tRuning\n");
    mas_run_app();
    printk("\t\tLaunching\n");
    mas_run_PCM();
}
#endif
