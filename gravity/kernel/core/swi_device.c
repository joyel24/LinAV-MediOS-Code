/* 
*   kernel/swi_device.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/usb_fw.h>
#include <kernel/rtc.h>
#include <kernel/bat_power.h>

extern int lcd_bright;

__IRAM_CODE int swi_device_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
        case nAPI_TIME:
            switch((int)nParam1) {
                case 0x000:
                    return rtc_getTime((struct av_tm *)nParam2);
                case 0x001:
                    return rtc_setTime((struct av_tm *)nParam2);
                default:
                    printk("time swi %d not implemented\n",(int)nParam1);
            }
            return 0;

        case nAPI_POWER:
            switch((int)nParam1) {
                case 0x000:
                    *((int*)nParam2)=kusbIsConnected();
                    break;
                case 0x001:
                    *((int*)nParam2)=kFWIsConnected();
                    break;
                case 0x002:
                    *((int*)nParam2)=kpowerConnected();
                    break;
                case 0x003:
                    *((int*)nParam2)=kgetBatLevel();
                    break;
                default:
                    printk("power swi %d not implemented\n",(int)nParam1);
            }
            return 0;

        
		case nAPI_SET_LCD_BRIGHTNESS:
			lcd_bright = nParam1;
			return 0;

		case nAPI_GET_LCD_BRIGHTNESS:
			*((int*)nParam1) = lcd_bright;
			return 0;
	}

	return 0;
}
