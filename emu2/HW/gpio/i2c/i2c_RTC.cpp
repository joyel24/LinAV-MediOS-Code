/* 
*   i2c_RTC.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdlib.h>
#include <stdio.h>

#include <time.h>

#include <emu.h>

#include <i2c_RTC.h>
#include <HW_gpio.h>

char * time_str[8] = {
    "MS",
    "Sec",
    "Min",
    "Hours",
    "Dweek",
    "Day",
    "Month",
    "Year"
};

int i2c_RTC::read(void)
{
    int val = 0;
    if(index>=0 && index<0x42)
    {
        readTime();
        val=rtc_reg[index];
        DEBUG_HW(RTC_HW_DEBUG,"I2C - RTC - R - %s : %x\n",index<0x8?time_str[index]:"conf",val);
        index++;        
    }
    else
        DEBUG_HW(RTC_HW_DEBUG,"I2C - RTC - R - Error bad address : %x\n",index);
    return val;
}

void i2c_RTC::write(int val)
{
    if(!has_address)
    {
        index = val;
        has_address = true;        
        DEBUG_HW(RTC_HW_DEBUG,"I2C -RTC - W - starting at : %x\n",index);
    }
    else
    {
        if(index>=0 && index<0x42)
        {
            rtc_reg[index]=val;
            DEBUG_HW(RTC_HW_DEBUG,"I2C - RTC - W - %s : %x\n",index<0x8?time_str[index]:"conf",val);
            index++;
        }
        else
            DEBUG_HW(RTC_HW_DEBUG,"I2C -RTC - W - Error bad address : %x\n",index);
    }
}
        
void i2c_RTC::start(int direction)
{
    
}

void i2c_RTC::stop(void)
{
    has_address=false;
}

#if 0
struct tm {
        int     tm_sec;         /* secondes           */
        int     tm_min;         /* minutes            */
        int     tm_hour;        /* heures             */
        int     tm_mday;        /* quantième du mois  */
        int     tm_mon;         /* mois (0 à  11)    */
        int     tm_year;        /* année              */
        int     tm_wday;        /* jour de la semaine */
        int     tm_yday;        /* jour de l'année    */
        int     tm_isdst;       /* décalage horaire   */
};
#endif

void i2c_RTC::readTime(void)
{
    time_t rawtime;
    time(&rawtime);   
    struct tm * curT = localtime(&rawtime);
    //printf ( "Current date and time are: %s", asctime (curT) );
    int val;
    rtc_reg[0]=0;
    rtc_reg[1]= (rtc_reg[1]&0x80) | (int2bcd(curT->tm_sec)&0x7F);
    rtc_reg[2]= int2bcd(curT->tm_min)&0x7F;
    rtc_reg[3]= (rtc_reg[3]&0xC0) | (int2bcd(curT->tm_hour)&0x3F);
    val=curT->tm_wday==0?7:curT->tm_wday;
    rtc_reg[4]= (rtc_reg[4]&0x80) | (int2bcd(val)&0x07);
    rtc_reg[5]= int2bcd(curT->tm_mday)&0x3F;
    rtc_reg[6]= int2bcd(curT->tm_mon+1)&0x1F;
    rtc_reg[7]= int2bcd(curT->tm_year%100)&0xFF;    
    //printf("converted tp : %x %x %x %x:%x:%x %x\n",rtc_reg[4],rtc_reg[6],rtc_reg[5],rtc_reg[3],rtc_reg[2],rtc_reg[1],rtc_reg[7]);
}

int i2c_RTC::int2bcd(int v)
{
        return ((v % 10) + (((int) (v / 10)) << 4));
}


int i2c_RTC::bcd2int(int v)
{
        return ((v & 0xf0) >> 4) * 10 + (v & 0xf);
}

i2c_RTC::i2c_RTC(HW_gpio * gpio):i2c_device(0xD0,"RTC",gpio)
{
    //printf("i2c_RTC constructor for %x:%s %x\n",this,name,address);
    has_address=false;
    index=0;
    for(int i=0;i<0x14;i++)
        rtc_reg[i]=0;
        
    //rtc_reg[0x0c] = 0x00;   //0x40;                                   // RTC halted

    rtc_reg[0x14] =  0x33;

    rtc_reg[0x15] = 0x00;

    rtc_reg[0x16] = 0x00;
    rtc_reg[0x17] = 0x17;
    rtc_reg[0x18] = 0x18;
    rtc_reg[0x19] = 0x19;
    rtc_reg[0x1a] = 0x1a;
    rtc_reg[0x1b] = 0x1b;

    rtc_reg[0x1c] = 0x1c;                                   // [dsc21]Dir
    rtc_reg[0x1d] = 0x1d;                                   //
    rtc_reg[0x1e] = 0x1e;                                   //
    rtc_reg[0x1f] = 0x1f;                                   //

    rtc_reg[0x20] = 0x00;                                   // ? dsc25
    rtc_reg[0x21] = 0x00;                                   // prints this
    rtc_reg[0x22] = 0x00;                                   // out
    rtc_reg[0x23] = 0x00;                                   //

    rtc_reg[0x24] = 0x24;
    rtc_reg[0x25] = 0x25;
    rtc_reg[0x26] = 0x26;
    rtc_reg[0x27] = 0x27;

    rtc_reg[0x28] = 0x00;                                   // [dsc21] LBA
    rtc_reg[0x29] = 0x00;                                   //
    rtc_reg[0x2a] = 0x00;                                   //
    rtc_reg[0x2b] = 0x00;                                   //

    rtc_reg[0x2c] = 0x2c;
    rtc_reg[0x2d] = 0x2d;
    rtc_reg[0x2e] = 0x2e;                                   // [dsc21] Lev
    rtc_reg[0x2f] = 0x2f;                                   // Checksum
    rtc_reg[0x30] = 0x30;
    rtc_reg[0x31] = 0x31;
    rtc_reg[0x32] = 0x32;
    rtc_reg[0x33] = 0x33;
    rtc_reg[0x34] = 0x34;
    rtc_reg[0x35] = 0x35;
    rtc_reg[0x36] = 0x36;
    rtc_reg[0x37] = 0x37;
    rtc_reg[0x38] = 0x38;
    rtc_reg[0x39] = 0x39;
    rtc_reg[0x3a] = 0x3a;

    int ch = 0;
    for (int i = 0x14; i < 0x3c; i++) {
        if (i != 0x2f) {
            ch += rtc_reg[i];
        }
    }

    ch = ch & 0xff;

    printf("[RTC Checksum calculated] =0x%x\n",ch);

    rtc_reg[0x2f] = ch;

    rtc_reg[0x3d] = 0x17;
    rtc_reg[0x3e] = 0x6f;                                   // Vmin | Vmax
    rtc_reg[0x3f] = 0xe1;                                   // Vavg | ~Vavg
    rtc_reg[0x40] = 0x08;    
}
