/* 
*   kernel/driver/i2c.c
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
#include <kernel/irq.h>
#include <kernel/kernel.h>

#define DELAY_1              { int _x; for(_x=0;_x<10;_x++); };
#define DELAY_2              { int _x; for(_x=0;_x<10;_x++); };

#define SDA_MASK           0x8
#define SDA_N_MASK         0xfff7

#define SDA_HI             outw(SDA_MASK,GIO_BITSET1);
#define SDA_LO             outw(SDA_MASK,GIO_BITCLEAR1);
#define SDA_IN             outw(inw(GIO_DIRECTION1) | SDA_MASK,GIO_DIRECTION1);
#define SDA_OUT            outw(inw(GIO_DIRECTION1) & SDA_N_MASK,GIO_DIRECTION1);
#define SDA                inw(GIO_BITSET1)&SDA_MASK

#define SCL_MASK           0x4
#define SCL_N_MASK         0xfffb

#define SCL_HI             outw(SCL_MASK,GIO_BITSET1);
#define SCL_LO             outw(SCL_MASK,GIO_BITCLEAR1); 
#define SCL_IN             outw(inw(GIO_DIRECTION1) | SCL_MASK,GIO_DIRECTION1);
#define SCL_OUT            outw(inw(GIO_DIRECTION1) & SCL_N_MASK,GIO_DIRECTION1);
#define SCL                inw(GIO_BITSET1)&SCL_MASK

#define ENA_INT            
//sti();
#define DIS_INT            
//cli();

#define DA_1               DIS_INT SDA_IN ENA_INT
#define CL_1               DIS_INT SCL_IN ENA_INT 
#define DA_0               DIS_INT SDA_LO SDA_OUT ENA_INT
#define CL_0               DIS_INT SCL_LO SCL_OUT ENA_INT

#define i2c_ini_xfer()     { int _val; DELAY_1 CL_1 DELAY_1 DA_1 DELAY_1 \
                                _val=inw(GIO_BITSET1);\
                                if(!(_val & SCL_MASK) || !(_val & SDA_MASK)) return -1; \
                           }

#define WAIT_I2C           while(!SCL) /*nothing*/;

__IRAM_CODE void i2c_start(void)
{
    DELAY_1        
    CL_1  
    DELAY_1
    DA_0    
    DELAY_1    
    CL_0
    DELAY_1
}

__IRAM_CODE void i2c_stop(void)
{
    CL_0
    DELAY_1
    DA_0
    DELAY_1
    CL_1
    DELAY_1
    DA_1
    DELAY_1
}

__IRAM_CODE void i2c_ack(void)
{
    CL_0
    DELAY_1
    DA_0
    DELAY_1
    CL_1
    DELAY_1
    while(!SCL) /*nothing*/;
    DELAY_1
    CL_0
    DELAY_1
    DA_1
    DELAY_1
}

__IRAM_CODE void i2c_notAck(void)
{
    CL_0
    DELAY_1
    DA_1
    DELAY_1
    CL_1
    DELAY_1
    WAIT_I2C
    DELAY_1
    CL_0
    DELAY_1
}

__IRAM_CODE int i2c_getAck(void)
{
    int ret=0;
    
    CL_0
    DELAY_1
    DA_1
    DELAY_1
    CL_1
    DELAY_1
    WAIT_I2C
    DELAY_1
    if(SDA)
        ret=1;
    DELAY_1
    CL_0
    DELAY_1
    return ret;
}

__IRAM_CODE char i2c_inb(void)
{
    char i;
    char ret=0;
    for(i=0x80;i;i=i>>1)
    {
        CL_0
        DELAY_1
        CL_1
        DELAY_1
        WAIT_I2C
        if(SDA)
            ret |= i; 
         DELAY_1     
    }
    CL_0
    DELAY_1    
    return ret;
}

__IRAM_CODE int i2c_outb(char data)
{
    char i;    
    for(i=0x80;i;i=i>>1)
    {
        CL_0
        DELAY_1
        if(i&data)
        {
            DA_1
        }
        else
        {
            DA_0
        }
        DELAY_1
        CL_1
        DELAY_1        
    }
    CL_0
    DELAY_1
    return i2c_getAck();
}

__IRAM_CODE int i2c_read(int device, int address, void * buffer, int count)
{
    int i;
    char * buff=(char*) buffer;

    i2c_ini_xfer();
    
    i2c_start();

    if(i2c_outb(((device<<24)>>25)<<1) != 0)
        return -2;

    if(i2c_outb((address<<24)>>24) != 0)
        return -3;

    i2c_start();

    if(i2c_outb(((device<<24)>>24)|1) != 0)
        return -4;
    
    for(i=0;i<count-1;i++)
    {
        buff[i]=i2c_inb();
        i2c_ack();
    }
    
    buff[i]=i2c_inb();

    i2c_notAck();
    i2c_stop();
    return 0;
}

__IRAM_CODE int i2c_write(int device, int address, void * buffer, int count)
{
    int i;
    char * buff=(char*) buffer;

    i2c_ini_xfer();
    
    i2c_start();

    if(i2c_outb(((device<<24)>>25)<<1) != 0)
        return -1;

    if(i2c_outb((address<<24)>>24) != 0)
        return -2;

    for(i=0;i<count;i++)
        if(i2c_outb(buff[i])!=0)
            return -3;

    i2c_stop();
    return 0;
}

__IRAM_CODE int i2c_writeRaw(int device, void * buffer, int count)
{
    int i;
    char * buff=(char*) buffer;
    
    i2c_ini_xfer();
    
    i2c_start();

    if(i2c_outb(((device<<24)>>25)<<1) != 0)
        return -1;

    for(i=0;i<count;i++)
        if(i2c_outb(buff[i])==0)
            break;

    i2c_stop();
    return 0;
}
