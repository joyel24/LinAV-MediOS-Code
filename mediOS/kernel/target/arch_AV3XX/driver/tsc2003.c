/* 
*   kernel/target/arch_AV3XX/tsc2003.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/hardware.h>
#include <kernel/i2c.h>
#include <kernel/target/arch/tsc2003.h>


int tsc2003getVal(char cmdN)
{
	unsigned char batVal[2]={0x00,0x00};
	int i;
	/*if(i2c_read(TSC_DEVICE, cmdN, cmd, 2)<0)
	{
		printk("[I2C - tsc] Error, reading");
		return -1;
	}*/	

	/*if(i2c_iniRead()<0)
	{
		printk("[I2C - tsc] Error, in iniRead(1)");
		return -1;
	}*/

	i2c_start();
	
        i2c_outb(I2C_WRITE_DEVICE(TSC_DEVICE));
	
	i2c_outb(cmdN);

	for(i=0;i<10;i++) /* nothing*/ ;

	i2c_stop();

	for(i=0;i<10;i++) /* nothing*/ ;

	/*if(i2c_iniRead()<0)
	{
		printk("[I2C - tsc] Error, in iniRead(2)");
		return -1;
	}*/

	i2c_start();

	i2c_outb(I2C_READ_DEVICE(TSC_DEVICE));
	
	batVal[0]=i2c_inb();
	i2c_ack();
	batVal[1]=i2c_inb();
	i2c_notAck();
	i2c_stop();

	return (((batVal[0] << 4)&0x0FF0) | ((batVal[1]>>4)&0x000F));
}



