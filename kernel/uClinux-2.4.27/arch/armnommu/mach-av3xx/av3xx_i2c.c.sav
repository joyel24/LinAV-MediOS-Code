#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_i2c.h>
#include <asm/arch/av3xx_gio.h>

#define DELAY              { int _x; for(_x=0;_x<10;_x++); };

#if 0
#define WAIT_IN            while(!(inw(AV3XX_I2C_REG_IN)&0x4)) /* NOTHING */ ;

#define I2C_IO_8           outw(0x8,AV3XX_I2C_REG_IO);
#define I2C_IO_4           outw(0x4,AV3XX_I2C_REG_IO);

#define I2C_DR_OR_4        outw(inw(AV3XX_I2C_REG_DR) | 0x4,AV3XX_I2C_REG_DR);
#define I2C_DR_OR_8        outw(inw(AV3XX_I2C_REG_DR) | 0x8,AV3XX_I2C_REG_DR);
#define I2C_DR_AND_NOT_4   outw(inw(AV3XX_I2C_REG_DR) & 0xfffb,AV3XX_I2C_REG_DR);
#define I2C_DR_AND_NOT_8   outw(inw(AV3XX_I2C_REG_DR) & 0xfff7,AV3XX_I2C_REG_DR);

#define I2C_DATA_OUTPUT    outw(0x4,AV3XX_I2C_REG_IO);


#define SDA_MASK           0x8
#define SCL_MASK           0x4

#define SDA_CLS            outw(SDA_MASK,AV3XX_I2C_REG_IO);
#define SDA_HI             outb(SDA_MASK,AV3XX_I2C_REG_IN);
#define SDA_LO             SDA_CLS 
#define SDA_INPUT          outb(inb(AV3XX_I2C_REG_DR) | SDA_MASK,AV3XX_I2C_REG_DR);
#define SDA_OUTPUT         outb(inb(AV3XX_I2C_REG_DR) & ~SDA_MASK,AV3XX_I2C_REG_DR);
#define SDA                SDA_MASK & inb(AV3XX_I2C_REG_IN)

#define SCL_CLS            outw(SCL_MASK,AV3XX_I2C_REG_IO);
#define SCL_HI             outb(SCL_MASK,AV3XX_I2C_REG_IN);
#define SCL_LO             SCL_CLS
#define SCL_INPUT          outb(inb(AV3XX_I2C_REG_DR) | SCL_MASK,AV3XX_I2C_REG_DR);
#define SCL_OUTPUT         outb(inb(AV3XX_I2C_REG_DR) & ~SCL_MASK,AV3XX_I2C_REG_DR);
#define SCL                SCL_MASK & inb(AV3XX_I2C_REG_IN)
#endif

#define SDA_CLS            av3xx_gio_clear(AV3XX_GIO_I2C_DATA);
#define SDA_HI             av3xx_gio_set(AV3XX_GIO_I2C_DATA);
#define SDA_LO             SDA_CLS 
#define SDA_INPUT          av3xx_gio_dir(AV3XX_GIO_I2C_DATA,GIO_IN);
#define SDA_OUTPUT         av3xx_gio_dir(AV3XX_GIO_I2C_DATA,GIO_OUT);
#define SDA                av3xx_gio_is_set(AV3XX_GIO_I2C_DATA)

#define SCL_CLS            av3xx_gio_clear(AV3XX_GIO_I2C_CLOCK);
#define SCL_HI             av3xx_gio_set(AV3XX_GIO_I2C_CLOCK);
#define SCL_LO             SCL_CLS 
#define SCL_INPUT          av3xx_gio_dir(AV3XX_GIO_I2C_CLOCK,GIO_IN);
#define SCL_OUTPUT         av3xx_gio_dir(AV3XX_GIO_I2C_CLOCK,GIO_OUT);
#define SCL                av3xx_gio_is_set(AV3XX_GIO_I2C_CLOCK)


void av3xx_i2c_stop(void)
{
	SDA_LO
        SCL_HI
        DELAY
        SDA_HI        

}

void av3xx_i2c_start(void)
{
	SDA_OUTPUT
        SCL_OUTPUT
        SDA_HI
        SCL_HI
        SDA_LO
        DELAY
        SCL_LO
}

void av3xx_i2c_ack(void)
{
	SCL_LO
        SDA_LO
        SCL_INPUT
        while(!SCL) /*nothing*/;
        DELAY
        SCL_OUTPUT
        SCL_LO
        SDA_HI
}

void av3xx_i2c_notAck(void)
{
	SCL_LO
        SDA_HI
        SCL_INPUT
        while(!SCL) /*nothing*/;
        DELAY
        SCL_OUTPUT
        SCL_LO
}

int av3xx_i2c_getAck(void)
{
	int ret=0;
        
        //SDA_LO
        SCL_HI
        SDA_INPUT
        SCL_INPUT
        while(!SCL) /*nothing*/;
        
        DELAY
        
        if(SDA)
            ret=1;
        
        SCL_OUTPUT
        SCL_LO
        SDA_HI
        SDA_OUTPUT

	return ret;
}

int av3xx_i2c_inb(void)
{
	int i;
	int ret=0;
        SCL_LO
	for(i=0x80;i;i=i>>1)
	{
            //SDA_LO
            SDA_INPUT
            SCL_HI
            DELAY
            if(SDA)
                ret |= i;
            SCL_LO
            SDA_OUTPUT
	}
        
	return ret;
}

int av3xx_i2c_outb(int data)
{
    int i;
    SCL_LO
    for(i=0x80;i;i=i>>1)
    {
        if(i&data)
            SDA_HI
        else
            SDA_LO
        SCL_HI
        DELAY
        SCL_LO
    }
    //SDA_HI
    return av3xx_i2c_getAck();
}

int av3xx_i2c_iniRead(void)
{
	int i;
	SDA_OUTPUT
        SCL_OUTPUT
        
        SDA_HI
        SCL_LO
        
        for(i=0;i<3;i++)
            av3xx_i2c_stop();
	
	return 0;
}

int av3xx_i2c_read(int device, int address, void * buffer, int count)
{
	int i;
	char * buff=(char*) buffer;

        av3xx_i2c_iniRead();
        
	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>25)<<1) != 0)
		return -2;

	if(av3xx_i2c_outb((address<<24)>>24) != 0)
		return -3;

	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>24)|1) != 0)
		return -4;
        
        for(i=0;i<count-1;i++)
        {
                buff[i]=av3xx_i2c_inb();
                av3xx_i2c_ack();
        }
        
	buff[i]=av3xx_i2c_inb();

	av3xx_i2c_notAck();
	av3xx_i2c_stop();
	return 0;
}

int av3xx_i2c_write(int device, int address, void * buffer, int count)
{
	int i;
	char * buff=(char*) buffer;

        av3xx_i2c_iniRead();
        
	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>25)<<1) != 0)
		return -1;

	if(av3xx_i2c_outb((address<<24)>>24) != 0)
		return -2;

	for(i=0;i<count;i++)
		if(av3xx_i2c_outb(buff[i])!=0)
                    return -3;

	av3xx_i2c_stop();
	return 0;
}

int av3xx_i2c_writeRaw(int device, void * buffer, int count)
{
	int i;
	char * buff=(char*) buffer;
        
        av3xx_i2c_iniRead();
	
	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>25)<<1) != 0)
		return -1;

	for(i=0;i<count;i++)
		if(av3xx_i2c_outb(buff[i])==0)
			break;

	av3xx_i2c_stop();
	return 0;
}
