#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>

#include "av3xx_common.h"
#include "graphics.h"
#include "misc.h"
#include "colordef.h"
#include "events.h"

needFont(std8x13);

int main(int argc,char ** argv)
{
    int evt;
    ini_graphics();
    setFont(std8x13);
    
    clearScreen(COLOR_WHITE);
    
    putS(COLOR_BLACK,COLOR_WHITE,15,15,"USB CONTROL");
    fprintf(stderr,"in USB CONTROL\n");
    
    if(!getUSB())
    {
        putS(COLOR_RED,COLOR_WHITE,10,60,"USB CABLE NOT PRESENT");
        fprintf(stderr,"USB cable not connected\n");
        putS(COLOR_RED,COLOR_WHITE,10,75,"When plugged, press a key");
        putS(COLOR_BLUE,COLOR_WHITE,10,90,"Press OFF to exit");
        while(1)
        {
            evt=waitEvent();
            if(evt==BTN_OFF)
            {
                fprintf(stderr,"get OFF key => exit\n");
                goto end;
            }
            else            
                if((evt==BTN_ON || evt==BTN_F1 || evt==BTN_F2 || evt==BTN_F3) && getUSB())
                {
                    fprintf(stderr,"get a key && usb connected => next stage\n");
                    break;
                }
        }
        fillRect(COLOR_WHITE,10,60,310,60);
    }
    
    putS(COLOR_BLACK,COLOR_WHITE,10,60,"Enabling USB");
    
    if(umount2("/mnt", MNT_FORCE)<0)
    {
        fprintf(stderr,"Error unmounting fs\n");
        goto end;
    }
    fprintf(stderr,"unmount done\n");
    if(setUSB(1)<0)
    {
        fprintf(stderr,"Error enabling USB\n");
        goto end2;
    }
    fprintf(stderr,"in usb state\n");
    
    putS(COLOR_BLACK,COLOR_WHITE,10,60,"Press a key when finished");
    while(1)
    {
        evt=waitEvent();
        if(evt==BTN_ON || evt==BTN_F1 || evt==BTN_F2 || evt==BTN_F3|| evt==BTN_OFF)
        {
            fprintf(stderr,"get a key => disabling usb\n");
            break;
        }
    }
    fillRect(COLOR_WHITE,10,60,310,40);
    putS(COLOR_BLACK,COLOR_WHITE,10,60,"Disabling USB");
    if(setUSB(0)<0)
    {
        fprintf(stderr,"Error disabling USB\n");
        goto end;
    }
    fprintf(stderr,"usb disable\n");
end2:
    if(mount("/dev/avhd1","/mnt","vfat",MS_MGC_VAL,NULL)<0)
    {
        fprintf(stderr,"Error mounting fs\n");
        goto end;
    }
    fprintf(stderr,"fs remounted\n");
end:
    close_graphics();
    return 0;

}
