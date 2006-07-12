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

void WelcomeScreen(){
    int y=0;

    gfx_clearScreen(COLOR_WHITE);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y,       "aoDoom v0.2");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "===========");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:  Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Square: Shoot");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Cross:  Strafe");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On:     Activate");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F1:     Change gamma");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:     Change weapon");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:     Run");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off:    Menu");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Choose the WAD file you want to");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "play using the browser.");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,REALSCREENHEIGHT-10,"Press a key to continue...");

    while(btn_readState());
    while(!btn_readState());
};

void PWADScreen(){
    gfx_clearScreen(COLOR_WHITE);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,0,       "You chose a PWAD (custom WAD)!");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,9,       "Now choose the corresponding");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,18,      "IWAD file (eg:doom2.wad)");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,REALSCREENHEIGHT-10,"Press a key to continue...");

    while(btn_readState());
    while(!btn_readState());
};

int app_main(){
    char * wadname=malloc(MAX_PATH);

    myargc = 0;
    myargv = NULL;

    osd_setEntirePalette(gui_pal,256);

    gfx_openGraphics();
    gfx_clearScreen(COLOR_WHITE);
    gfx_fontSet(STD6X9);

    icon_init();
    iniBrowser();

    WelcomeScreen();
    
    if (browser_simpleBrowse("/",wadname)!=MED_OK) app_exit(true);
    strlwr(wadname);

    if(!IsPWAD(wadname)){
        modifiedgame = false;
        MyIdentifyVersion(wadname);
        D_AddFile(wadname);
    }else{
        char * pwadname=malloc(MAX_PATH);

        strcpy(pwadname,wadname);

        PWADScreen();

        if (browser_simpleBrowse("/",wadname)!=MED_OK) app_exit(true);
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
    gfx_closeGraphics();

    if (error){
        printf("\nAbnormal exit, press # or X to continue...\n");

        while(btn_readState()&(BTMASK_BTN1|BTMASK_BTN2));
        while(!(btn_readState()&(BTMASK_BTN1|BTMASK_BTN2)));
    }

#ifdef BUILD_STDALONE
    reload_firmware();
#else
    exit();
#endif
};
