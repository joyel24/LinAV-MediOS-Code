// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log$
// Revision 1.10  2006/10/28 10:54:49  sfxgligli
// doom: added cheats, fixed AV300 build, sound improvements, killed warnings
//
// Revision 1.9  2006/10/27 16:18:47  sfxgligli
// doom: added ingame menu with tv out & oc options
//
// Revision 1.8  2006/10/11 21:48:27  opeos
// improvement to aones config now explorer goes directly to /aones/roms
// config to use-screen_resize for av4xx
// bug correction for explorer goes directly to /doom
// bug correction to ifdef in medos.h for doom
// config button in aoboy for av4 similar to aones
// thanks to GliGli and oxygen77 for the info.
//
// Revision 1.8  2006/10/11 03:42:30  opeos
// Now the WAD files was stored in /doom
// the explorer goes here automatically
//thanks to GliGli for the solution

// Revision 1.7  2006/08/19 08:42:30  oxygen77
// small bugfix in Makefiles
// adding 1 missing fction in api
//
// Revision 1.6  2006/07/12 17:13:01  sfxgligli
// - widget system
// - new cfg file parser
// - uidemo
// - enabled random dos name generation in fat code
// - some function name standardisation
//
// Revision 1.5  2006/06/19 16:36:30  sfxgligli
// aoDoom update
//
// Revision 1.4  2006/05/28 17:08:45  sfxgligli
// aoDoom update (adding browser, PWADs support, optimisations,...)
//
// Revision 1.3  2006/02/08 17:45:40  oxygen77
// big cleanup in makefiles
//
// Revision 1.2  2006/02/06 22:45:48  oxygen77
// make doom work with new api, we lack of exit() now
//
// Revision 1.1  2005/12/20 19:11:56  sfxgligli
// - added Doom port
// - Gmini400 buttons fix
//
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id$";

#include "medios.h"
#include "doomdef.h"
#include "doomstat.h"

#include "m_argv.h"
#include "d_main.h"

extern void snd_close();
extern void display_getRealSize();

bool IsPWAD(char * name){
    char id[5];
    int f;

    memset(id,'\0',5);

    f=open(name,O_RDONLY);
    if(f<0) return false;
    read(f,id,4);
    close(f);

    return !strncmp(id,"PWAD",4);
}

void MyIdentifyVersion(char * name){
    gamemode = indetermined;

    if(strstr(name,"doom1.wad")!=NULL){
        gamemode = shareware;
        return;
    }
    if(strstr(name,"doom.wad")!=NULL){
        gamemode = registered;
        return;
    }
    if(strstr(name,"doomu.wad")!=NULL){
        gamemode = retail;
        return;
    }
    if(strstr(name,"doom2.wad")!=NULL){
        gamemode = commercial;
        return;
    }
    if(strstr(name,"tnt.wad")!=NULL){
        gamemode = commercial;
        return;
    }
    if(strstr(name,"plutonia.wad")!=NULL){
        gamemode = commercial;
        return;
    }
}

int app_main(){
    char * wadname=malloc(MAX_PATH);

    myargc = 0;
    myargv = NULL;

    // disable LCD & halt timer
    set_timer_status(LCD_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(LCD_TIMER,TIMER_MODE_DC,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_DC,MODE_DISABLE);

    // create dir if it doesn't exist
    mkdir("/doom",-1);

    gfx_openGraphics();
    gfx_clearScreen(COLOR_WHITE);
    gfx_fontSet(STD6X9);

    //needed to get real width&hheight right before launching doom code
    display_getRealSize();

    icon_init();
    iniBrowser();

    gui_welcomeScreen();

    if (browser_simpleBrowse("/doom",wadname)!=MED_OK) app_exit(true);
    strlwr(wadname);

    if(!IsPWAD(wadname)){
        modifiedgame = false;
        MyIdentifyVersion(wadname);
        D_AddFile(wadname);
    }else{
        char * pwadname=malloc(MAX_PATH);

        strcpy(pwadname,wadname);

        gui_PWADScreen();

        if (browser_simpleBrowse("/doom",wadname)!=MED_OK) app_exit(true);
        strlwr(wadname);

        modifiedgame = true;
        MyIdentifyVersion(wadname);
        D_AddFile(wadname);
        D_AddFile(pwadname);

        free(pwadname);
    }

    free(wadname);

    gfx_closeGraphics();

    D_DoomMain ();
    return 0;
}


void app_exit(bool error){
    snd_close();
    gfx_closeGraphics();

    if (error){
        printf("\nAbnormal exit, press # or X to continue...\n");

        while(btn_readState()&(BTMASK_BTN1|BTMASK_BTN2));
        while(!(btn_readState()&(BTMASK_BTN1|BTMASK_BTN2)));
    }

#ifdef BUILD_STDALONE
    reload_firmware();
#else
    exit(0);
#endif
};
