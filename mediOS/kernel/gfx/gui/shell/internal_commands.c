/*
* kernel/gui/gfx/shell/internal_commands.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/internal_commands.h>

#include <kernel/shell.h>

#include <gui/file_browser.h>
#include <gui/msgBox.h>
#include <kernel/exit.h>
#include <kernel/usb_fw.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/ata.h>
#include <kernel/disk.h>
#include <kernel/vfs.h>
#include <kernel/delay.h>
#include <kernel/graphics.h>
#include <kernel/lcd.h>
#include <kernel/buttons.h>

#include <sys_def/stddef.h>
#include <sys_def/types.h>
#include <sys_def/string.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

static bool intCmd_doBrowser(char * param);
static bool intCmd_doReloadFirmware(char * param);
static bool intCmd_doUsbMode(char * param);

typedef struct{
    char * command;
    bool (*function)(char * param);
} INTERNAL_COMMAND;

INTERNAL_COMMAND intCmd_commands[] = {
    {
        command:  "browser",
        function: intCmd_doBrowser
    },
    {
        command:  "reload_firmware",
        function: intCmd_doReloadFirmware
    },
    {
        command:  "usb_mode",
        function: intCmd_doUsbMode
    },
    /* should always be the last entry */
    {
        command:  NULL,
        function: NULL
    }
};

bool intCmd_execute(char * command,char * param){
    bool retval;
    int i;

    printk("[shell] executing internal command '%s', param='%s'\n",command,(param)?param:"");

    i=0;
    retval=false;
    for(;;){
        if(intCmd_commands[i].command==NULL){
            printk("[shell] error: internal command '%s' not found\n",command);
            retval=false;
            break;

        }else if(!strcmp(intCmd_commands[i].command,command)){
            retval=intCmd_commands[i].function(param);
            break;
        }
        ++i;
    }

    return retval;
}

/******************************************************************************/
/* INTERNAL COMMANDS                                                          */
/******************************************************************************/

static bool intCmd_doBrowser(char * param){
    struct browser_data * browser;

    browser=browser_NewBrowse();

    if(!browser){
        return false;
    }

    browser_browse(browser,param,NULL);

    browser_disposeBrowse(browser);

    return true;
}

static bool intCmd_doReloadFirmware(char * param){

    reload_firmware();

    return true;
}

int usbMode=0;

static bool intCmd_doUsbMode(char * param){
    if(usbMode)
    {
        // in usb mode => disable usb
        printk( "Warning should not be here\n");
        return false;
#warning need some processing to see what to do here
    }
    else
    {
        int eh=evt_getHandler(ALL_CLASS);
        int evt;

        // not in usb mode => enable usb if cable present
        if(usb_isConnected() || FW_isConnected())
        {

            msgBox_show("MediOS - USB mode","Switching to USB mode",MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,-1);

            if(disk_rmAll()!=MED_OK)
            {
                printk("can't umount\n");

                evt_freeHandler(eh);
                return false;
            }

            enableUsbFw();
            usbMode=1;
            mdelay(10);

            msgBox_show("MediOS - USB mode","Press F3 or unplug cable to exit",MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,-1);

            do{
                evt=evt_getStatus(eh);
            }while(evt!=BTN_F3 && evt!=EVT_USB_OUT);

            msgBox_show("MediOS - USB mode","Leaving USB mode",MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,-1);

            disableUsbFw();
            usbMode=0;

            ata_reset();
            vfs_init();
            disk_addAll();

#warning we should reload menu.cfg or other menu related files
        }
        else
        {
            msgBox_show("MediOS - USB mode","USB cable is not connected",MSGBOX_TYPE_OK,MSGBOX_ICON_EXCLAMATION,eh);
        }

        evt_freeHandler(eh);
    }

    return true;
}
