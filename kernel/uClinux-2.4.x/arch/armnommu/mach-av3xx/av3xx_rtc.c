#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_i2c.h>
#include <asm/arch/av3xx_rtc.h>


int av3xx_rtc_getTime(struct tm * valTime) {
    if(av3xx_i2c_read(AV3XX_RTC_DEVICE, 0, (void*)(valTime), 8)<0)
	{
		printk("[I2C - rtc] Error, reading");
		return -1;
	}
    valTime->tm_wday&=0x7;
	return 0;
}

int av3xx_rtc_setTime(struct tm *newTime) {
    if(av3xx_i2c_write(AV3XX_RTC_DEVICE, 0, (void*)(newTime), 8)<0)
	{
		printk("[I2C - rtc] Error, writting");
		return -1;
	}
	return 0;
}

int av3xx_rtc_init(void) {
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
