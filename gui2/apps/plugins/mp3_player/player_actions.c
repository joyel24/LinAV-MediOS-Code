#include "cops.h"
#include "colordef.h"
#include "mp3_player.h"

/* client graphical ops */
extern struct client_operations * cops;

/*extern variables */
extern int vol,oldvol;
/*******************/

int fade=1;
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
