/*
* play.c
* by midk
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
#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "font.h"
#include "colordef.h"

#include "mp3_player.h"

struct client_operations * cops;

/***********************
 * Miscellaneous DEFINEs
 **********************/
#define MP3_BUFF_SIZE (1020*2000)
#define MWINCLUDECOLORS


/******************************
 * Color order for LCD printing
 *****************************/
const char *colortext[] = {
"White     ",
"Gray      ",
"Black     ",
"Blue      ",
"Dark Blue ",
"Red       ",
"Green     ",
"Dark Green",
"Yellow    ",
"Orange    "
};

/*******************************
 * And their corresponing colors
 ******************************/
const char colortable[] = {
COLOR_WHITE,
COLOR_GRAY,
COLOR_BLACK,
COLOR_BLUE,
COLOR_DARK_BLUE,
COLOR_RED,
COLOR_GREEN,
COLOR_DARK_GREEN,
COLOR_YELLOW,
COLOR_ORANGE
};

/*************************
 * Ints for PLAYBACK/SOUND
 ************************/
/* sound settings */
int vol=70, bass = 50, treb = 50, bal = 50, loud = 0;
int oldvol;                  /* used for pause/resume */

/******************
 * Ints for WINDOWS
 *****************/
int window = 1;                   /* "1" = main, "2" = settings, "3" = sound */

/*********************
 * Miscellaneous CHARs
 ********************/
char * filename;                    /* current file location */

/*******************
 * Other
 ******************/
struct mp3_play data; /* mp3 data */

int eventHandler(int evt)
{
    if(evt==EVT_TIMER && data.finished)
    {
        cops->close_mp3_playback();
        RELEASE(cops);
    }
    
    switch(window)
    {
        case MAIN_WIN:
            handleMainWin(evt);
            break;
        case SETTINGS_WIN:
            handleSettingsWin(evt);
            break;
        case SOUND_WIN:
            handleSoundWin(evt);
            break;
    }
    return 1;
}

int main(int argc, char * * argv)
{
    REGISTER(cops,eventHandler,0);
#ifdef AV_SCREEN
   if(argc<2)
       return 0; 
    else
    {
        filename=argv[1];
    }    
#endif
    
    data.size=MP3_BUFF_SIZE;
    data.filename=filename;
    data.pos=0;
    data.finished=0;

    /* initialize mp3 playback */
    if(!cops->ini_mp3_playback(&data))  
    	return 0;    

    /* initialize the graphics and clear the lcd */
    cops->hideSBar();
    cops->disableMenu();
    cops->clearScreen(COLOR_BLACK); /* clear */

    /* set standard font */
    cops->setFont(STD6X9);
    refreshScreen(MAIN_WIN);
    apply_settings();  
    /* start mp3 */
    cops->start_playback();
        
    PACK(cops,drawPeak);    
    
    return 1;
}
