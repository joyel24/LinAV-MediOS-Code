/*
* kernel/gui/gfx/mp3Player.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/sound.h>
#include <kernel/playList.h>

#include <kernel/graphics.h>
#include <kernel/kernel.h>
#include <kernel/evt.h>

#include <evt.h>

#include <sys_def/colordef.h>
#include <sys_def/string.h>

void printMp3Gui(void)
{
    struct list_entry * cur_entry = curEntryInList();
    char str[100];
    gfx_putS(COLOR_BLACK,COLOR_WHITE,5,30,cur_entry->id3.title);
    sprintf(str,"%d",cur_entry->id3.length);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,5,40,str);
}

void mp3PlayerMain(void)
{
    int ret_val;
    int evt_hand,evt;
    int stopMp3Player=0;
    int pause_state=0;
    
    sound_initMp3();
    
    gfx_clearScreen(COLOR_WHITE);
    
    if((evt_hand=evt_getHandler(BTN_CLASS))<0)
    {
        printk("Can't get evt handler (error:%d)\n",-evt_hand);
        evt_hand=-1;
    }
     
    ret_val = addDir("/m_test/");
    if(!ret_val)
        return;
        
    printMp3Gui();   
    sound_readMore();
    sound_startMp3();    
    while(!stopMp3Player)
    {
        sound_readMore();
        if((evt=evt_getStatus(evt_hand))<0)
            printk("Bad evt (error:%d)\n",-evt);
        switch(evt)
        {
            case BTN_OFF:
                sound_pauseMp3();
                stopMp3Player=1;
                sound_freeMp3();
                break;
            case BTN_ON:
                if(pause_state)
                {
                    sound_startMp3();
                    pause_state=0;
                }
                else
                {
                    sound_pauseMp3();
                    pause_state=1;
                }
                break;
            case BTN_RIGHT:
                sound_nxtTrack();
                printMp3Gui();
                evt_purgeHandler(evt_hand);
                break;
            case BTN_LEFT:
                sound_prevTrack();
                printMp3Gui();
                evt_purgeHandler(evt_hand);
                break;
        }
    }    
}

