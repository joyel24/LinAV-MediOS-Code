/*
*   kernel/driver/rtc.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/hardware.h>
#include <kernel/i2c.h>
#include <kernel/rtc.h>
#include <kernel/kernel.h>
#include <kernel/errors.h>

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

#define pvToUser(val)     ((((val>>4)&0xF)*10)+(val&0xF))
#define userToPv(val)     ((((val/10)<<4)&0xf0)+((val%10)&0x0F))

int is_leap_year(int yr)
{
    return ((yr) <= 1752 ? !((yr) % 4) : \
    (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))  ? 1:0 ;
}

int month_code[12] = {6,2,2,5,0,3,5,1,4,6,2,4};

int getDayOfWeek(int day,int month,int year)
{
    int result;
    int y;
    
    if(month>12) month=12;
    
    y=year%100;
    result=(5*y)/4;
    result+=month_code[month-1];
    result+=day;
    if(is_leap_year(year) && (month==1 || month==2))
        result-=1;
    while(result<=0)
        result+=7;
    result%=7;
    if(result==0)
        result=7;
    //printk("[getDayOfWeek]: %02d/%02d/%04d => %d\n",day,month,year,result);
    return result;
}

MED_RET_T rtc_getTime(struct med_tm * valTime)
{
    struct tm_pv pv_dt;
    int retVal;
    
    //  printk("[I2C - rtc] before read\n");
    
    retVal=i2c_read(RTC_DEVICE, 0, (void*)(&pv_dt), 8);
            
    if(retVal<0)
    {
            printk("[I2C - rtc] Error, reading (err:%d)\n",retVal);
            return retVal;
    }
    
    //  printk("[I2C - rtc] read success\n");
    
    pv_dt.tm_wday&=0x7;

    valTime->tm_ms=pvToUser(pv_dt.tm_ms);
    valTime->tm_sec=pvToUser(pv_dt.tm_sec);
    valTime->tm_min=pvToUser(pv_dt.tm_min);
    valTime->tm_hour=pvToUser(pv_dt.tm_hour);
    valTime->tm_wday=pvToUser(pv_dt.tm_wday);
    valTime->tm_mday=pvToUser(pv_dt.tm_mday);
    valTime->tm_mon=pvToUser(pv_dt.tm_mon);
    valTime->tm_year=2000+pvToUser(pv_dt.tm_year);
    return MED_OK;
}

#define sendRTC(addr,val)        {int __ret;int __val=val; __ret=i2c_write(RTC_DEVICE, addr, (void*)(&__val), 1); \
                                        if(retVal<0) printk("[I2C - rtc] Error, writting (err:%d)\n",retVal);}

MED_RET_T rtc_setTime(struct med_tm *newTime)
{
    struct tm_pv pv_dt;
    struct tm_pv pv_dt_ini;
    int year;
    int wday;
    int retVal;
    
    char tmp[0x14];
    int i;
    
    /* print out everything */
    printk("[set rtc] 0| ");
    i2c_read(RTC_DEVICE, 0, (void*)tmp, 0x14);
    for(i=0;i<0x14;i++)
        printk("%02x ",tmp[i]);
    printk("\n");
    
    retVal=i2c_read(RTC_DEVICE, 0, (void*)(&pv_dt_ini), 8);
            
    if(retVal<0)
    {
            printk("[I2C - rtc] Error, reading (err:%d) can't save the value\n",retVal);
            return retVal;
    }
    
    year=newTime->tm_year-2000;
    
    pv_dt.tm_ms=userToPv(newTime->tm_ms);
    pv_dt.tm_sec=userToPv(newTime->tm_sec);
    pv_dt.tm_min=userToPv(newTime->tm_min);
    pv_dt.tm_hour=userToPv(newTime->tm_hour);
    pv_dt.tm_wday=userToPv(newTime->tm_wday);
    pv_dt.tm_mday=userToPv(newTime->tm_mday);
    pv_dt.tm_mon=userToPv(newTime->tm_mon);
    pv_dt.tm_year=userToPv(year);
    
    /*printk("Setting time: old= %02x:%02x:%02x newUS= %02d:%02d:%02d newPV= %02x:%02x:%02x new2= %02x:%02x:%02x\n",
                                pv_dt_ini.tm_hour,pv_dt_ini.tm_min,pv_dt_ini.tm_sec,
                                newTime->tm_hour,newTime->tm_min,newTime->tm_sec,
                                pv_dt.tm_hour,pv_dt.tm_min,pv_dt.tm_sec,
                                    RTC_MK_H(pv_dt_ini.tm_hour,pv_dt.tm_hour),
                                    RTC_MK_M(pv_dt_ini.tm_min,pv_dt.tm_min),
                                    RTC_MK_S(pv_dt_ini.tm_sec,pv_dt.tm_sec));*/
    
    /* only writting h,m,s and d,m,y */
    
    wday=getDayOfWeek(newTime->tm_mday,newTime->tm_mon,newTime->tm_year);
    
    /*sendRTC(RTC_S,RTC_MK_S(pv_dt_ini.tm_sec,pv_dt.tm_sec));
    sendRTC(RTC_M,RTC_MK_M(pv_dt_ini.tm_min,pv_dt.tm_min));
    sendRTC(RTC_H,RTC_MK_H(pv_dt_ini.tm_hour,pv_dt.tm_hour));
    
    sendRTC(RTC_DM,RTC_MK_DM(pv_dt_ini.tm_mday,pv_dt.tm_mday));
    sendRTC(RTC_MO,RTC_MK_MO(pv_dt_ini.tm_mon,pv_dt.tm_mon));
    sendRTC(RTC_Y,RTC_MK_Y(pv_dt_ini.tm_year,pv_dt.tm_year));*/
    
    pv_dt.tm_ms=0;
    pv_dt.tm_sec=RTC_MK_S(pv_dt_ini.tm_sec,pv_dt.tm_sec);
    pv_dt.tm_min=RTC_MK_M(pv_dt_ini.tm_min,pv_dt.tm_min);
    pv_dt.tm_hour=RTC_MK_H(pv_dt_ini.tm_hour,pv_dt.tm_hour);
    pv_dt.tm_wday=(wday&0x7);
    pv_dt.tm_mday=RTC_MK_DM(pv_dt_ini.tm_mday,pv_dt.tm_mday);
    pv_dt.tm_mon=RTC_MK_MO(pv_dt_ini.tm_mon,pv_dt.tm_mon);
    pv_dt.tm_year=RTC_MK_Y(pv_dt_ini.tm_year,pv_dt.tm_year);
    
    retVal=i2c_write(RTC_DEVICE, 0, (void*)(&pv_dt), 8);

    if(retVal<0)
    {
        printk("[I2C - rtc] Error, writting (err:%d)\n",retVal);
        return retVal;
    }
    
    /* print out everything */
    printk("[set rtc] 1| ");
    i2c_read(RTC_DEVICE, 0, (void*)tmp, 0x14);
    for(i=0;i<0x14;i++)
        printk("%02x ",tmp[i]);
    printk("\n");
    //wday=getDayOfWeek(newTime->tm_mday,newTime->tm_mon,newTime->tm_year);
    
    
    //sendRTC(RTC_DW,((wday&0x7) | 0x80));
    /*if(newTime->tm_mday == 27 && newTime->tm_mon == 11)
        sendRTC(RTC_DW,RTC_MK_DW(pv_dt_ini.tm_wday,5))*/
    
    return MED_OK;
}

MED_RET_T init_rtc(void)
{
    unsigned char cb;
    int retVal;
    int cwd;
    
    struct med_tm valTime;
        
    /*cb=0x10;
    if((retVal=av3xx_i2c_write(AV3XX_RTC_DEVICE, 0x13, (void*)&cb, 1))<0)
    {
            printk("[I2C-rtc-ini] Step 1: Error, writting (err:%d)\n",retVal);
            return -1;
    } */ 
    
    if((retVal=i2c_read(RTC_DEVICE, 0x04, (void*)&cb, 1))<0)
    {
        printk("[I2C-rtc-ini] Step 0: Error, reading (err:%d)\n",retVal);
        return -1;
    }
    if(cb & 0x80)
    {
        cb&= ~0x80;        
        if((retVal=i2c_write(RTC_DEVICE, 0x04, (void*)&cb, 1))<0)
        {
            printk("[I2C-rtc-ini] Step 1: Error, writting (err:%d)\n",retVal);
            return -1;
        }
    }

    /* Clear the Stop bit if it is set */
    if((retVal=i2c_read(RTC_DEVICE, 0x01, (void*)&cb, 1))<0)
    {
        printk("[I2C-rtc-ini] Step 2: Error, reading (err:%d)\n",retVal);
        return -1;
    }        
    if(cb & 0x80)
    {
        cb&= ~0x80;
        if((retVal=i2c_write(RTC_DEVICE, 0x01, (void*)&cb, 1))<0)
        {
            printk("[I2C-rtc-ini] Step 3: Error, writting (err:%d)\n",retVal);
            return -1;
        }
    }
        
    if((retVal=i2c_read(RTC_DEVICE, 0x0c, (void*)&cb, 1))<0)
    {
        printk("[I2C-rtc-ini] Step 4: Error, reading (err:%d)\n",retVal);
        return -1;
    }
    if(cb & 0x40)
    {
        cb &= ~0x40;                         // Make sure HT bit is not set
        if((retVal=i2c_write(RTC_DEVICE, 0x0c, (void*)&cb, 1))<0)
        {
            printk("[I2C-rtc-ini] Step 5: Error, writting (err:%d)\n",retVal);
            return -1;
        }
    }
    
    rtc_getTime(&valTime); 

    cwd=getDayOfWeek(valTime.tm_mday,valTime.tm_mon,valTime.tm_year);
    if(cwd!=valTime.tm_wday)
    {
        //printk("setting DofW: %d (old %d)\n",cwd,valTime.tm_wday);
        sendRTC(RTC_DW,((cwd&0x7) | 0x80));
    }
    
    rtc_getTime(&valTime);
    
    printk("[init] rtc :%02d/%02d/%04d %02d:%02d:%02d\n",valTime.tm_mday,valTime.tm_mon,valTime.tm_year
                ,valTime.tm_hour,valTime.tm_min,valTime.tm_sec);
    return MED_OK;
}

