/*
* mp3_player.h
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

#ifndef __MP3_PLAYER_H
#define __MP3_PLAYER_H

#define MAIN_WIN      1
#define SETTINGS_WIN  2
#define SOUND_WIN     3

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mp3_data.h"
#include "id3.h"

struct id3tag
{
    struct mp3entry id3;
    int mempos;
    bool used;
};

/* refresh.c */
void apply_settings(void);
void refreshScreen(int window);
void draw_settings_progressbar(int x, int y, int value);
void draw_soundsettings(void);
void draw_settings(void);

/* side_menu.c */
void draw_main_help_text(void);
void draw_settings_help_text(void);
void draw_soundsettings_help_text(void);

/* evtHandler.c */
void handleMainWin(int evt);
void handleSettingsWin(int evt);
void handleSoundWin(int evt);

/* osc_peak_meter.c */
void * drawPeak(void * arg);
void oscillograph(int l, int r);
void peak_meters(int l, int r);
void erase_peak(void);

/* player_actions.c */
void pause_resume(void);

/* file_reader.c */
void * mp3_read_more(void * arg);

void debug_info(char * filename,struct mp3entry * mp3);
#endif


