#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_i2c.h>
#include <asm/arch/av3xx_rtc.h>

struct tm_pv {
    unsigned char tm_ms;
    unsigned char tm_sec;
    unsigned char tm_min;
    unsigned char tm_hour;
    unsigned char tm_wday;
    unsigned char tm_mday;
    unsigned char tm_mon;
    unsigned char tm_year;
};

int pvToUser(unsigned char val)
{
	return ((((val>>4)&0xF)*10)+(val&0xF));
}

unsigned char userToPv(int val)
{
	return ((((val/10)<<4)&0xf0)+(val&0xF));
}

int av3xx_rtc_getTime(struct tm * valTime)
{
	struct tm_pv pv_dt;
        int retVal;
        
        retVal=av3xx_i2c_read(AV3XX_RTC_DEVICE, 0, (void*)(&pv_dt), 8);
		
	if(retVal<0)
	{
		printk("[I2C - rtc] Error, reading (err:%d)\n",retVal);
		return retVal;
	}
	
	pv_dt.tm_wday&=0x7;
    
	valTime->tm_ms=pvToUser(pv_dt.tm_ms);
	valTime->tm_sec=pvToUser(pv_dt.tm_sec);
	valTime->tm_min=pvToUser(pv_dt.tm_min);
	valTime->tm_hour=pvToUser(pv_dt.tm_hour);
	valTime->tm_wday=pvToUser(pv_dt.tm_wday);
	valTime->tm_mday=pvToUser(pv_dt.tm_mday);
	valTime->tm_mon=pvToUser(pv_dt.tm_mon);
	valTime->tm_year=2000+pvToUser(pv_dt.tm_year);
    
	return 0;
}

int av3xx_rtc_setTime(struct tm *newTime)
{
	struct tm_pv pv_dt;
	
	pv_dt.tm_ms=userToPv(newTime->tm_ms);
	pv_dt.tm_sec=userToPv(newTime->tm_sec);
	pv_dt.tm_min=userToPv(newTime->tm_min);
	pv_dt.tm_hour=userToPv(newTime->tm_hour);
	pv_dt.tm_wday=userToPv(newTime->tm_wday);
	pv_dt.tm_mday=userToPv(newTime->tm_mday);
	pv_dt.tm_mon=userToPv(newTime->tm_mon);
	pv_dt.tm_year=userToPv(newTime->tm_year-2000);
	
	if(av3xx_i2c_write(AV3XX_RTC_DEVICE, 0, (void*)(&pv_dt), 8)<0)
	{
		printk("[I2C - rtc] Error, writting");
		return -1;
	}
	return 0;
}

int av3xx_rtc_init(void)
{
	unsigned char cb;
	
	if(av3xx_i2c_read(AV3XX_RTC_DEVICE, 0x0c, (void*)&cb, 1)<0)
	{
		printk("[I2C - rtc] Error, reading");
		return -1;
	}
	cb &= ~0x40;                         // Make sure HT bit is not set
	if(av3xx_i2c_write(AV3XX_RTC_DEVICE, 0x0c, (void*)&cb, 1)<0)
	{
		printk("[I2C - rtc] Error, writting");
		return -1;
	}
	return 0;
}
