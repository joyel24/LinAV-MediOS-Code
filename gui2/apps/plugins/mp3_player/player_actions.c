/*
* player_actions.c
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

/* client graphical ops */
extern struct client_operations * cops;

/*extern variables */
extern int vol,oldvol;
/*******************/

int fade=0;    /* not using fade by default */
int pause=0;

/**********************
 * Pause or resume song
 *********************/
void pause_resume(void)
{
    if(pause) /* let's resume */
    {
        cops->start_playback();
        pause=0;

        if(fade) /* fade in */
        {
            oldvol = vol;
            vol = 30;
            while(vol < oldvol)
            {
                vol++;
                apply_settings();
            }
        }
    }
    else /* let's pause */
    {
        if(fade) /* fade out */
        {
            oldvol = vol;
            while(vol > 30)
            {
                vol--;
                apply_settings();
            }
            vol = oldvol;
        }

        cops->pause_playback();
        pause=1;
    }
    refreshScreen(MAIN_WIN);
}
