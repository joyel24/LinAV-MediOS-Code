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
#include <stdarg.h>
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

#include "colordef.h"

#define DO_DEBUG 1;
#define NB_ITEM 10;



extern FONT_ID font_table[NBFONT];

extern struct plugin cur_plugin;
extern struct plugin menu_plugin;
extern struct plugin status_bar_plugin;

int stopWM;

char * path;

void getPath(char * str)
{
	char * tmpC;
        int pos;
        tmpC=(char*)strrchr(str,'/');
        pos=tmpC-str;
        fprintf(stderr,"Pos: %d\n",pos);
        path=(char*)malloc(sizeof(char)*(pos+1));
        strncpy(path,str,pos);
        path[pos]='\0';
}

int main(int argc,char * * argv)
{
    fprintf(stderr,"Starting AvWm\n");
    
    getPath(argv[0]);    
    
    ini_graphics();    
    ini_font(STD8X13);
    set_mouseParam(6, 3);
    
    if(!ini_menu(path,&menu_plugin))
    {
    	close_graphics();
        free(path);
        return -1;
    }

    clearScreen(COLOR_WHITE);
    
    ini_status_bar(&status_bar_plugin);
       
    setTimerFreq(10);
    startTimer();    
    
    stopWM=0;
    
    showSBar();
    menu_plugin.handle_on=1;
    sendEvt(&menu_plugin,EVT_REDRAW);
        
    eventLoop(); //main loop    
    
    close_graphics();
    free(path);    
    return 0;
}

void showSBar(void)  {status_bar_plugin.handle_on=1;sendEvt(&status_bar_plugin,EVT_REDRAW);}
void hideSBar(void)  {status_bar_plugin.handle_on=0;}
int sBarStatus(void) {return status_bar_plugin.handle_on;}
