/*
* refresh.c
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

#include "cops.h"
#include "colordef.h"
#include "mp3_player.h"
#include "font.h"

/* client graphical ops */
extern struct client_operations * cops;

/*extern variables */
extern const char colortable[];
extern char * filename;
extern int fade;
extern int vol,bass,treb,bal,loud;
extern int fade,peakmeters,scroll_osci,peak_decay,peak_levelcolor,peak_bgcolor,osci_levelcolor,osci_bgcolor;
extern const char *colortext[];
extern struct id3tag fTag;
/*******************/

int sound_cursor_position = 1;    /* cursor position at sound settings */
int settings_cursor_position = 1; /* cursor position at settings */

void refreshScreen(int window)
{
    char tmp[60];
    struct list_entry * cur_entry;
    /* main window */
    switch(window)
    {
        case MAIN_WIN:
            /* Print the version at the top */
            sprintf(tmp,"--= MP3 Player v2.00 =--");
            cops->putS(COLOR_WHITE, COLOR_BLACK, 65, 10, tmp);
            /* What's playing? */
            cops->setFont(STD7X13);
            sprintf(tmp,"Now Playing:");
            cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 40, tmp);
            sprintf(tmp,"%s",filename);
            cur_entry=curEntryInList();
            if(!cur_entry)
                printf("No entry in the list\n");
            else
                cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 55, cur_entry->id3.path);            
            cops->setFont(STD6X9);
            /* fill in the peak meter background */
            if(peakmeters)
            {
                cops->fillRect(colortable[peak_bgcolor], 0, 208, 270, 15);
                cops->fillRect(colortable[peak_bgcolor], 0, 224, 270, 15);
            }
            draw_main_help_text();
            break;
        case SETTINGS_WIN:
            //erase_peak();
            draw_settings();
            draw_settings_help_text();
            break;
        case SOUND_WIN:
            //erase_peak();
            draw_soundsettings();
            draw_soundsettings_help_text();
            break;
    }
}

/********************
 * Apply the settings
 *******************/
void apply_settings(void)
{
    cops->setVolume(vol);
    cops->setBass(bass);
    cops->setTreble(treb);
    cops->setLoudness(loud);
    cops->setBalance(bal);
}

/***************************************
 * Draws a progress bar on a 0-100 scale
 **************************************/
void draw_settings_progressbar(int x, int y, int value)
{
    int j;
    
    for(j=y+5; j<y+7; j++)
    {
        cops->drawLine(COLOR_BLACK, x+value, j, x+value+(100-value), j);
        cops->drawLine(COLOR_BLUE, x, j, x+value, j);
    }

    cops->drawRect(COLOR_WHITE, x, y+3, 101, 6);
}

/**************************
 * Draw sound settings text
 *************************/
void draw_soundsettings(void)
{
    char tmp[60];
    sprintf(tmp,"Vol: %03d%%",vol);
    if(sound_cursor_position == 1)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 100, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 100, tmp);

    sprintf(tmp,"Bass: %03d%%",bass);
    if(sound_cursor_position == 2)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 115, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 115, tmp);

    sprintf(tmp,"Treble: %03d%%",treb);
    if(sound_cursor_position == 3)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 130, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 130, tmp);

    sprintf(tmp,"Balance: %03d%%",bal);
    if(sound_cursor_position == 4)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 145, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 145, tmp);

    sprintf(tmp,"Loudness: %03d%%",loud);
    if(sound_cursor_position == 5)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 160, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 160, tmp);
}

/********************
 * Draw settings text
 *******************/
void draw_settings(void)
{
    char tmp[60];
    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 40, "--- GENERAL OPTIONS ---");

    if(fade == 1)
        sprintf(tmp, "Fade On Stop/Pause: Yes");
    else
        sprintf(tmp, "Fade On Stop/Pause: No ");
    if(settings_cursor_position == 1)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 55, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 55, tmp);

    /*
     *VISUALIZATION CATEGORY
     */
    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 85, "--- VISUALIZATION OPTIONS ---");

    if(peakmeters == 1)
        sprintf(tmp, "Visualization: Peak Meters ");
    else
        sprintf(tmp, "Visualization: Oscillograph");
    if(settings_cursor_position == 2)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 100, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 100, tmp);

    if(scroll_osci == 1)
        sprintf(tmp, "Scrolling Oscillograph: Yes");
    else
        sprintf(tmp, "Scrolling Oscillograph: No ");
    if(settings_cursor_position == 3)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 115, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 115, tmp);

    if(peak_decay > 0)
        sprintf(tmp, "Peak Release (Peak Meter): %02d        ", peak_decay);
    else
        sprintf(tmp, "Peak Release (Peak Meter): [No Decay]");
    if(settings_cursor_position == 4)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 130, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 130, tmp);

    /*
     * COLORS CATEGORY
     */
    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 160, "--- COLOR OPTIONS ---");

    sprintf(tmp, "Peak Meter Color [Level]: %s", colortext[peak_levelcolor]);
    if(settings_cursor_position == 5)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 175, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 175, tmp);

    sprintf(tmp, "Peak Meter Color [Background]: %s", colortext[peak_bgcolor]);
    if(settings_cursor_position == 6)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 190, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 190, tmp);

    sprintf(tmp, "Oscillograph Color [Level]: %s", colortext[osci_levelcolor]);
    if(settings_cursor_position == 7)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 205, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 205, tmp);

    sprintf(tmp, "Oscillograph Color [Background]: %s", colortext[osci_bgcolor]);
    if(settings_cursor_position == 8)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 220, tmp);
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 220, tmp);

}
