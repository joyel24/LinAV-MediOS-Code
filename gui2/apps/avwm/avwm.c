/*
* avwm.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "graphics.h"
#include "misc.h"
#include "events.h"
#include "avwm.h"
#include "parse_cfg.h"
#include "menu.h"
#include "cops.h"
#include "avevents.h"
#include "font.h"
#include "plugin.h"
#include "avstring.h"
#include "settings.h"
#include "msgBox.h"
#include "editBox.h"
#include "avwm-menu.h"
#include "status_line.h"
#include "CF_handler.h"
#include "fm.h"
#include "helperMenu.h"
#include "bg_loader.h"
#include "icons.h"
#include "browser.h"

#include "osd.h"

#include "colordef.h"

#define DO_DEBUG 1;
#define NB_ITEM 10;

extern FONT_ID font_table[NBFONT];

extern struct plugin cur_plugin;
extern struct plugin menu_plugin;
extern struct plugin status_bar_plugin;

int stopWM;
int maxChar;

char * path;

char * getPath(char * str)
{
    char * tmpC;
    char * res;
    int pos;

    tmpC=(char*)strrchr(str,'/');
    pos=tmpC-str;
    res=(char*)malloc(sizeof(char)*(pos+1));
    strncpy(res,str,pos);
    res[pos]='\0';

    return res;
}

int main(int argc,char * * argv)
{
    fprintf(stderr,"Starting AvWm\n");

    path=getPath(argv[0]);

    ini_graphics();
    ini_font(STD6X9);
    set_mouseParam(6, 3);
    ini_settings();
    
    iniIcon();
    
    ini_CF_mount();
    ini_FM();
       
    iniMsgBox();
    iniEditBox();
    
    iniBrowser();

    if(!ini_menu(path,&menu_plugin))
    {
        close_graphics();
        free(path);
        return -1;
    }
    
    //load_bg("/mnt/avlo.img");

    clearScreen(COLOR_WHITE);
    

    
    ini_status_bar(&status_bar_plugin);

    setTimerFreq(10);
    startTimer();

    stopWM=0;

    showSBar();
    menu_plugin.handle_on=1;
    enableMenu();
    sendEvt(&menu_plugin,EVT_REDRAW);

    eventLoop(); //main loop

    close_graphics();
    free(path);
    
    return 0;
}
