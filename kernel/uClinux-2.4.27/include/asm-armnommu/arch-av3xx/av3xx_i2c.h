/*
* asm/arch/av3xx_i2c.h
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

#ifndef __ASM_ARCH_AV3XX_I2C_H
#define __ASM_ARCH_AV3XX_I2C_H

#define AV3XX_RTC_DEVICE                        0xd0
#define AV3XX_TSC_DEVICE                        0x90
#define AV3XX_MAS_DEVICE                        0x3c

#define I2C_WRITE_DEVICE(val)                   (val & ~0x01)
#define I2C_READ_DEVICE(val)                    (val | 0x01)

#define DO_OUTB(val,func_name,errmsg)           doRetTest(av3xx_i2c_outb(val),func_name,errmsg)

void av3xx_i2c_stop(void);
void av3xx_i2c_start(void);

void av3xx_i2c_ack(void);
void av3xx_i2c_notAck(void);
int av3xx_i2c_getAck(void);

int av3xx_i2c_inb(void);
int av3xx_i2c_outb(int data);

int av3xx_i2c_iniRead(void);
int av3xx_i2c_read(int device, int address, void * buffer, int count);
int av3xx_i2c_write(int device, int address, void * buffer, int count);
int av3xx_i2c_writeRaw(int device, void * buffer, int count);


#endif
