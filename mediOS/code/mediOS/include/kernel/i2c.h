/* 
*   include/kernel/i2c.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_H
#define __I2C_H

#define RTC_DEVICE                        0xd0
#define TSC_DEVICE                        0x90
#define MAS_DEVICE                        0x3c
#define DVR_DEVICE                        0x4a

#define I2C_WRITE_DEVICE(val)                   (val & ~0x01)
#define I2C_READ_DEVICE(val)                    (val | 0x01)

void i2c_ini_xfer(void);

void wait_i2c(void);
void mas_i2c_outb(char data);
int i2c_getAck(void);

void i2c_stop(void);
void i2c_start(void);

void i2c_ack(void);
void i2c_notAck(void);
int i2c_getAck(void);

char i2c_inb(void);
int  i2c_outb(char data);

int i2c_read(int device, int address, void * buffer, int count);
int i2c_write(int device, int address, void * buffer, int count);
int i2c_writeRaw(int device, void * buffer, int count);


#endif
