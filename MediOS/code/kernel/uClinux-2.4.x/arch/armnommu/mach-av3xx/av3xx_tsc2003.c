#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_i2c.h>
#include <asm/arch/av3xx_tsc2003.h>


int av3xx_tsc2003getVal(char cmdN)
{
	unsigned char batVal[2]={0x00,0x00};
	int i;
	/*if(av3xx_i2c_read(AV3XX_TSC_DEVICE, cmdN, cmd, 2)<0)
	{
		printk("[I2C - tsc] Error, reading");
		return -1;
	}*/	

	/*if(av3xx_i2c_iniRead()<0)
	{
		printk("[I2C - tsc] Error, in iniRead(1)");
		return -1;
	}*/

	av3xx_i2c_start();
	
	DO_OUTB(I2C_WRITE_DEVICE(AV3XX_TSC_DEVICE),"I2C - tsc","Error, sending write device")
	
	DO_OUTB(cmdN,"I2C - tsc","Error, sending cmd")

	for(i=0;i<10;i++) /* nothing*/ ;

	av3xx_i2c_stop();

	for(i=0;i<10;i++) /* nothing*/ ;

	/*if(av3xx_i2c_iniRead()<0)
	{
		printk("[I2C - tsc] Error, in iniRead(2)");
		return -1;
	}*/

	av3xx_i2c_start();

	DO_OUTB(I2C_READ_DEVICE(AV3XX_TSC_DEVICE),"I2C - tsc","Error, sending read device")
	
	batVal[0]=av3xx_i2c_inb();
	av3xx_i2c_ack();
	batVal[1]=av3xx_i2c_inb();
	av3xx_i2c_notAck();
	av3xx_i2c_stop();

	return (((batVal[0] << 4)&0x0FF0) | ((batVal[1]>>4)&0x000F));
}



