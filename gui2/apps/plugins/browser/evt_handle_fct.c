/*
* evt_handle_fct.c
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

#include "ls_main.h"
#include "msgBox.h"
#include "helperMenu.h"

int pos,nselect;

/*extern variables */
extern struct client_operations * cops;
extern int menuOff;
extern struct dir_entry * list;
extern int listused;
extern struct menu_data menu_cfg;
extern struct menu_item menu1;
/************************/

int eventHandler(int evt)
{
    int w = 0;
    int h = 10;
    char pwd[10];    

    cops->getStringS("M", &w, &h);

    if(menuOff)
    {
        helperEvt(evt,BTN_JOY);
        switch(evt)
        {
            case BTN_UP:
                nselect--;

                if(nselect<0)
                {
                    nselect=0;
                    pos--;
                    if(pos<0) // we are at the beg => do wrapping
                    {
                        pos=listused-MAXPOS;//-1;
                        if(pos<0)
                        {
                            pos=0;
                            nselect=listused-1;
                        }
                        else
                            nselect=listused-pos-1;
                        printAllName(pos,nselect);
                    }
                    else // not going up, scrolling
                    {
                        cops->scrollWindowVert(COLOR_WHITE, 0, 1  + h+6+MENU_SHADOW, 320, (h+1)*MAXPOS, h+1,0);
                        printAName(pos+nselect+1,nselect+1,1,0);
                        printAName(pos+nselect,nselect,1,1);
                    }
                }
                else // just going up
                {
                    printAName(pos+nselect+1,nselect+1,0,0);
                    printAName(pos+nselect,nselect,0,1);
                }

                if( (listused>MAXPOS) &&
                    (pos+MAXPOS < listused) )
                    showArrow(DOWN_ARROW);
                else
                    hideArrow(DOWN_ARROW);

                if(pos == 0)
                    hideArrow(UP_ARROW);
                else
                    showArrow(UP_ARROW);

                break;

            case BTN_DOWN:
                nselect++;
                if(nselect+pos>=listused)       // jump to beginning
                {                    
                    pos=0;
                    nselect=0;
                    printAllName(pos,nselect);
                }
                else
                {
                    if(nselect>=MAXPOS)
                    {
                        nselect=MAXPOS-1;
                        pos++;
                        if(pos+MAXPOS>listused) // we are at the end => do wrapping
                        {
                            pos=0;
                            nselect=0;
                            printAllName(pos,nselect);
                        }
                        else // not going down, scrolling
                        {
                            cops->scrollWindowVert(COLOR_WHITE, 0, 1  + h+6+MENU_SHADOW, 320, (h+1)*MAXPOS, h+1,1);
                            printAName(pos+nselect-1,nselect-1,1,0);
                            printAName(pos+nselect,nselect,1,1);
                        }
                    }
                    else
                    {
                        printAName(pos+nselect-1,nselect-1,0,0);
                        printAName(pos+nselect,nselect,0,1);
                    }
                }
               if(pos>0)
                    showArrow(UP_ARROW);
                else
                    hideArrow(UP_ARROW);

                if( (listused>MAXPOS) &&
                    (pos+MAXPOS < listused) )
                    showArrow(DOWN_ARROW);
                else
                    hideArrow(DOWN_ARROW);

                break;

            case BTN_RIGHT:
                if(list[pos+nselect].type!=TYPE_DIR)
                    handle_type_other(list[pos+nselect].name);
                else
                    viewNewDir(list[pos+nselect].name);
                break;
            case BTN_LEFT:
                viewNewDir("../");
                break;
            case BTN_F1:
                viewNewDir("/mnt");                  
                break;
            case BTN_F2:
                if(cops->CF_is_mounted())
                    viewNewDir("/cf");
                break;
            case BTN_F3:
                menu_cfg.root=&menu1;
                menuOff=0;
                hideHelper();
                cops->start_menu(&menu_cfg);
                cops->menuEvtHandler(EVT_REDRAW);
                break;
            case BTN_OFF:
            case EVT_QUIT:
                RELEASE(cops)
                break;
            case EVT_REDRAW:
                printAllName(pos,nselect);
                break;
            case EVT_CF_REMOVED:
                getcwd(pwd, 10);
                pwd[10]='\0';
                if(pwd[0]=='/' && pwd[1]=='c' && pwd[2]=='f' && (pwd[3]=='/'||pwd[3]=='\0'))
                    viewNewDir("/mnt");
                break;
            case EVT_CF_ADDED:
                viewNewDir("/cf");
                break;
        }
    }
    else
    {
        cops->menuEvtHandler(evt);
    }
    return 1;
}