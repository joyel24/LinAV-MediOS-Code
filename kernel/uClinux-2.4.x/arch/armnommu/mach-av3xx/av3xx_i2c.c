#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_i2c.h>

#define DELAY              do { int _x; for(_x=0;_x<10;_x++); } while(0);
#define WAIT_IN            while(!(inw(AV3XX_I2C_REG_IN)&0x4)) /* NOTHING */ ;

#define I2C_IO_8           outw(0x8,AV3XX_I2C_REG_IO);
#define I2C_IO_4           outw(0x4,AV3XX_I2C_REG_IO);

#define I2C_DR_OR_4        outw(inw(AV3XX_I2C_REG_DR) | 0x4,AV3XX_I2C_REG_DR);
#define I2C_DR_OR_8        outw(inw(AV3XX_I2C_REG_DR) | 0x8,AV3XX_I2C_REG_DR);
#define I2C_DR_AND_NOT_4   outw(inw(AV3XX_I2C_REG_DR) & 0xfffb,AV3XX_I2C_REG_DR);
#define I2C_DR_AND_NOT_8   outw(inw(AV3XX_I2C_REG_DR) & 0xfff7,AV3XX_I2C_REG_DR);

#define I2C_DATA_OUTPUT    outw(0x4,AV3XX_I2C_REG_IO);
/*
#define SDA_MASK           0x8
#define SCL_MASK           0x4

#define SDA_CLS            outw(SDA_MASK,AV3XX_I2C_REG_IO);
#define SDA_HI             outb(inb(AV3XX_I2C_REG_DR) | SDA_MASK,AV3XX_I2C_REG_DR);
#define SDA_LO             SDA_CLS outb(inb(AV3XX_I2C_REG_DR) & ~SDA_MASK,AV3XX_I2C_REG_DR);
#define SDA_OUPUT          outb(inb(AV3XX_I2C_REG_IO) | SDA_MASK,AV3XX_I2C_REG_IO);
#define SDA_INPUT          outb(inb(AV3XX_I2C_REG_IO) & ~SDA_MASK,AV3XX_I2C_REG_IO);

#define SCL_CLS            outw(SCL_MASK,AV3XX_I2C_REG_IO);
#define SCL_HI             outb(inb(AV3XX_I2C_REG_DR) | SCL_MASK,AV3XX_I2C_REG_DR);
#define SCL_LO             SCL_CLS outb(inb(AV3XX_I2C_REG_DR) & ~SCL_MASK,AV3XX_I2C_REG_DR);
#define SCL_OUPUT          outb(inb(AV3XX_I2C_REG_IO) | SCL_MASK,AV3XX_I2C_REG_IO);
#define SCL_INPUT          outb(inb(AV3XX_I2C_REG_IO) & ~SCL_MASK,AV3XX_I2C_REG_IO);*/

void av3xx_i2c_stop(void)
{
	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY
	I2C_IO_8
	I2C_DR_AND_NOT_8
	DELAY
	I2C_DR_OR_4
	DELAY
	I2C_DR_OR_8

}

void av3xx_i2c_start(void)
{
	DELAY
	I2C_DR_OR_4
	DELAY
	I2C_IO_8
	I2C_DR_AND_NOT_8
	DELAY
	I2C_IO_8
	I2C_DR_AND_NOT_4
}

void av3xx_i2c_ack(void)
{
	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY
	I2C_IO_8
	I2C_DR_AND_NOT_8
	DELAY
	I2C_DR_OR_4
	DELAY
	WAIT_IN
	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY
	I2C_DR_OR_8

}

void av3xx_i2c_notAck(void)
{
	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY
	I2C_DR_OR_8
	DELAY
	I2C_DR_OR_4
	DELAY
	WAIT_IN
	DELAY
	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY
}

int av3xx_i2c_getAck(void)
{
	int ret;

	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY
	I2C_DR_OR_8
	DELAY
	I2C_DR_OR_4
	DELAY
	WAIT_IN
	DELAY

	ret=(inw(AV3XX_I2C_REG_IN)<<28)>>31;

	DELAY
	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY

	return ret;
}

int av3xx_i2c_inb(void)
{
	int i;
	int ret=0;

	for(i=0x80;i;i=i>>1)
	{
		I2C_IO_4
		I2C_DR_AND_NOT_4
		DELAY
		I2C_DR_OR_4
		DELAY
		//WAIT_IN
		DELAY
		if(inw(AV3XX_I2C_REG_IN)&0x8)
			ret |= i;

		DELAY
	}

	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY

	return ret;
}

int av3xx_i2c_outb(int data)
{
	int i;
	for(i=0x80;i;i=i>>1)
	{
		I2C_IO_4
		I2C_DR_AND_NOT_4
		DELAY


		if(i&data)
		{
			I2C_DR_OR_8
		}
		else
		{
			I2C_IO_8
			I2C_DR_AND_NOT_8
		}

		DELAY
		I2C_DR_OR_4
		DELAY
		//WAIT_IN
		DELAY
	}

	I2C_IO_4
	I2C_DR_AND_NOT_4
	DELAY

	return av3xx_i2c_getAck();
}

int av3xx_i2c_iniRead()
{
	int val;
	I2C_DR_OR_4
	I2C_DR_OR_8

	val=inw(AV3XX_I2C_REG_IN);

	if(!(val&0x4) || !(val&0x8))
		return -1;
	return 0;
}

int av3xx_i2c_read(int device, int address, void * buffer, int count)
{
	int val,i;
	char * buff=(char*) buffer;

	I2C_DR_OR_4
	I2C_DR_OR_8

	val=inw(AV3XX_I2C_REG_IN);

	if(!(val&0x4) || !(val&0x8))
		return -1;

	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>25)<<1) != 0)
		return -1;

	if(av3xx_i2c_outb((address<<24)>>24) != 0)
		return -1;

	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>24)|1) != 0)
		return -1;

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
	int val,i;
	char * buff=(char*) buffer;

	I2C_DR_OR_4
	I2C_DR_OR_8

	val=inw(AV3XX_I2C_REG_IN);

	if(!(val&0x4) || !(val&0x8))
		return -1;

	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>25)<<1) != 0)
		return -1;

	if(av3xx_i2c_outb((address<<24)>>24) != 0)
		return -1;

	for(i=0;i<count;i++)
		if(av3xx_i2c_outb(buff[i])==0)
			break;

	av3xx_i2c_stop();
	return 0;
}

int av3xx_i2c_writeRaw(int device, void * buffer, int count)
{
	int val,i;
	char * buff=(char*) buffer;

	I2C_DR_OR_4
	I2C_DR_OR_8

	val=inw(AV3XX_I2C_REG_IN);

	if(!(val&0x4) || !(val&0x8))
		return -1;

	av3xx_i2c_start();

	if(av3xx_i2c_outb(((device<<24)>>25)<<1) != 0)
		return -1;

	for(i=0;i<count;i++)
		if(av3xx_i2c_outb(buff[i])==0)
			break;

	av3xx_i2c_stop();
	return 0;
}
