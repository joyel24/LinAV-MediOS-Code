/*
* evtHandler.c
*
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
#include <pthread.h>

#include "cops.h"
#include "colordef.h"
#include "mp3_player.h"
#include "avevents.h"
#include "events.h"

/* client graphical ops */
extern struct client_operations * cops;

/*extern variables */
extern int window;
extern int sound_cursor_position;
extern int settings_cursor_position;
extern int vol,bass,treb,bal,loud;
extern int fade,peakmeters,scroll_osci,peak_decay,peak_levelcolor,peak_bgcolor,osci_levelcolor,osci_bgcolor;
extern int stopThread;
extern pthread_t read_thread;
//extern pthread_t draw_thread;
extern struct mp3_play data;
/*******************/

int eventHandler(int evt)
{
    if(evt==EVT_TIMER && data.finished)
    {
        fprintf(stderr,"file finished\n");
        stopThread=1;
        fprintf(stderr,"topThread=1\n");
        cops->close_mp3_playback();
        fprintf(stderr,"close mp3 ioctl\n");
        pthread_join(read_thread, NULL);
        fprintf(stderr,"thread join\n");
        RELEASE(cops);
        fprintf(stderr,"release\n");
        return 1;
    }
    
    if(evt==EVT_TIMER)
        mp3_read_more();
    
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

void handleMainWin(int evt)
{
    switch(evt)
    {
        case BTN_UP:  /* Vol UP */
            if(vol < 100)
                vol++;
            apply_settings();
            refreshScreen(MAIN_WIN);
            break;
        case BTN_DOWN: /* Vol DOWN */
            if(vol > 0)
                vol--;
            apply_settings();
            refreshScreen(MAIN_WIN);              
            break;
        case BTN_ON: /* Pause/Resume */
            pause_resume();
            break;
        case BTN_OFF: /* Exit player */
        case EVT_QUIT:
            stopThread=1;            
            if(fade)
                while(vol > 35)
                {
                    vol--;
                    apply_settings();
                }
            
            if(!cops->stop_playback()) fprintf(stderr,"error stopping\n");
            cops->close_mp3_playback();
            pthread_join(read_thread, NULL);
            //pthread_join(draw_thread, NULL);
            RELEASE(cops);          
            break;
        case BTN_F2: /* go settings */             
            window = SETTINGS_WIN;
            cops->stop_peak();
            cops->clearScreen(COLOR_BLACK);
            refreshScreen(SETTINGS_WIN);
            break;
        case BTN_F3: /* go sound settings */
            window = SOUND_WIN;
            cops->stop_peak();
            cops->clearScreen(COLOR_BLACK);            
            refreshScreen(SOUND_WIN);     
            break;
    }
}

void handleSettingsWin(int evt)
{
    switch(evt)
    {
        case BTN_UP: /* settings_cursor up */            
            if(settings_cursor_position > 1)
                settings_cursor_position--;
            else
                settings_cursor_position = 8;
            refreshScreen(SETTINGS_WIN);        
            break;
        case BTN_DOWN: /* settings_cursor down */            
            if(settings_cursor_position < 8)
                settings_cursor_position++;
            else
                settings_cursor_position = 1;
            refreshScreen(SETTINGS_WIN);            
            break;
        case BTN_RIGHT: /* adjust up */            
            switch(settings_cursor_position)
            {
                case 1: fade = !fade;               break;
                case 2: peakmeters = !peakmeters;   break;
                case 3: scroll_osci = !scroll_osci; break;
                case 4: if(peak_decay < 10) peak_decay++; break;
                case 5: if(peak_levelcolor < 9) peak_levelcolor++; else peak_levelcolor = 0; break;
                case 6: if(peak_bgcolor < 9) peak_bgcolor++;       else peak_bgcolor = 0;    break;
                case 7: if(osci_levelcolor < 9) osci_levelcolor++; else osci_levelcolor = 0; break;
                case 8: if(osci_bgcolor < 9) osci_bgcolor++;       else osci_bgcolor = 0;    break;
            }
            refreshScreen(SETTINGS_WIN);            
            break;
        case BTN_LEFT: /* adjust down */            
            switch(settings_cursor_position)
            {
                case 1: fade = !fade;               break;
                case 2: peakmeters = !peakmeters;   break;
                case 3: scroll_osci = !scroll_osci; break;
                case 4: if(peak_decay > 0) peak_decay--; break;
                case 5: if(peak_levelcolor > 0) peak_levelcolor--; else peak_levelcolor = 9; break;
                case 6: if(peak_bgcolor > 0) peak_bgcolor--;       else peak_bgcolor = 9;    break;
                case 7: if(osci_levelcolor > 0) osci_levelcolor--; else osci_levelcolor = 9; break;
                case 8: if(osci_bgcolor > 0) osci_bgcolor--;       else osci_bgcolor = 9;    break;
            }
            refreshScreen(SETTINGS_WIN);            
            break;
        case BTN_OFF: /* quit menu */
        case BTN_F2:
            cops->clearScreen(COLOR_BLACK);
            window = 1;
            refreshScreen(MAIN_WIN);
            cops->start_peak();
            break;
    }
}

void handleSoundWin(int evt)
{
    switch(evt)
    {
        case BTN_UP: /* settings_cursor up */            
            if(sound_cursor_position > 1)
                sound_cursor_position--;
            else
                sound_cursor_position = 5;
            refreshScreen(SOUND_WIN);            
            break;
        case BTN_DOWN: /* settings_cursor down */                    
            if(sound_cursor_position < 5)
                sound_cursor_position++;
            else
                sound_cursor_position = 1;
            refreshScreen(SOUND_WIN);                    
            break;
        case BTN_RIGHT: /* adjust up */                    
            switch(sound_cursor_position)
            {
                case 1: if(vol < 100) vol++; break;
                case 2: if(bass < 100) bass+=5; break;
                case 3: if(treb < 100) treb+=5; break;
                case 4: if(bal < 100) bal++; break;
                case 5: if(loud < 100) loud+=5; break;
            }
            refreshScreen(SOUND_WIN);
            apply_settings();
            break;
        case BTN_LEFT: /* adjust down */                    
            switch(sound_cursor_position)
            {
                case 1: if(vol > 0) vol--; break;
                case 2: if(bass > 0) bass-=5; break;
                case 3: if(treb > 0) treb-=5; break;
                case 4: if(bal > 0) bal--; break;
                case 5: if(loud > 0) loud-=5; break;
            }
            refreshScreen(SOUND_WIN);
            apply_settings();                    
            break;
        case BTN_OFF: /* exit menu */
        case BTN_F3: 
            cops->clearScreen(COLOR_BLACK);
            window = 1;  
            refreshScreen(MAIN_WIN);
            cops->start_peak();
            break;
    }
}

