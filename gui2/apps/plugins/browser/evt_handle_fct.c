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

/*int pos,nselect;*/

/*extern variables */
extern struct client_operations * cops;
extern int menuOff;
/*extern struct dir_entry * list;
extern int listused;*/
extern struct menu_data menu_cfg;
extern struct menu_item menu1;

extern struct browser_data * bdata;
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
                bdata->nselect--;

                if(bdata->nselect<0)
                {
                    bdata->nselect=0;
                    bdata->pos--;
                    if(bdata->pos<0) // we are at the beg => do wrapping
                    {
                        bdata->pos=bdata->listused-bdata->nb_disp_entry;//-1;
                        if(bdata->pos<0)
                        {
                            bdata->pos=0;
                            printAName(bdata,0,0,0,0);
                            bdata->nselect=bdata->listused-1;
                            printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                        }
                        else
                        {
                            bdata->nselect=bdata->listused-bdata->pos-1;
                            printAllName(bdata);
                        }
                    }
                    else // not going up, scrolling
                    {
                        cops->scrollWindowVert(COLOR_WHITE, 0, 1  + h+6+MENU_SHADOW, 
                                    320, (h+1)*bdata->nb_disp_entry, h+1,0);
                        printAName(bdata,bdata->pos+bdata->nselect+1,bdata->nselect+1,1,0);
                        printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,1,1);
                    }
                }
                else // just going up
                {
                    printAName(bdata,bdata->pos+bdata->nselect+1,bdata->nselect+1,0,0);
                    printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                }

                if( (bdata->listused>bdata->nb_disp_entry) &&
                    (bdata->pos+bdata->nb_disp_entry < bdata->listused) )
                    showArrow(DOWN_ARROW,bdata->nb_disp_entry);
                else
                    hideArrow(DOWN_ARROW,bdata->nb_disp_entry);

                if(bdata->pos == 0)
                    hideArrow(UP_ARROW,bdata->nb_disp_entry);
                else
                    showArrow(UP_ARROW,bdata->nb_disp_entry);

                break;

            case BTN_DOWN:
                bdata->nselect++;
                if(bdata->nselect+bdata->pos>=bdata->listused)       // jump to beginning
                {
                    if(bdata->listused<=bdata->nb_disp_entry)
                    {
                        printAName(bdata,bdata->pos+bdata->nselect-1,bdata->nselect-1,0,0);
                        bdata->pos=0;
                        bdata->nselect=0;
                        printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                    }
                    else
                    {
                        bdata->pos=0;
                        bdata->nselect=0;
                        printAllName(bdata);
                    }
                }
                else
                {
                    if(bdata->nselect>=bdata->nb_disp_entry)
                    {
                        bdata->nselect=bdata->nb_disp_entry-1;
                        bdata->pos++;
                        if(bdata->pos+bdata->nb_disp_entry>bdata->listused) // we are at the end => do wrapping
                        {
                            bdata->pos=0;
                            bdata->nselect=0;
                            printAllName(bdata);
                        }
                        else // not going down, scrolling
                        {
                            cops->scrollWindowVert(COLOR_WHITE, 0, 1  + h+6+MENU_SHADOW, 
                                    320, (h+1)*bdata->nb_disp_entry, h+1,1);
                            printAName(bdata,bdata->pos+bdata->nselect-1,bdata->nselect-1,1,0);
                            printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,1,1);
                        }
                    }
                    else
                    {
                        printAName(bdata,bdata->pos+bdata->nselect-1,bdata->nselect-1,0,0);
                        printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                    }
                }
               if(bdata->pos>0)
                    showArrow(UP_ARROW,bdata->nb_disp_entry);
                else
                    hideArrow(UP_ARROW,bdata->nb_disp_entry);

                if( (bdata->listused>bdata->nb_disp_entry) &&
                    (bdata->pos+bdata->nb_disp_entry < bdata->listused) )
                    showArrow(DOWN_ARROW,bdata->nb_disp_entry);
                else
                    hideArrow(DOWN_ARROW,bdata->nb_disp_entry);

                break;

            case BTN_RIGHT:
                switch(bdata->list[bdata->pos+bdata->nselect].type)
                {
                    case TYPE_BACK:
                        viewNewDir(bdata,"../");
                        break;
                    case TYPE_DIR:
                        viewNewDir(bdata,bdata->list[bdata->pos+bdata->nselect].name);
                        break;
                    case TYPE_FILE:
                        handle_type_other(bdata->list[bdata->pos+bdata->nselect].name);
                        //viewNewDir("./");
                        break;
                }
                break;
            case BTN_LEFT:
                viewNewDir(bdata,"../");
                break;
            case BTN_F1:
                viewNewDir(bdata,"/mnt");                  
                break;
            case BTN_F2:
                if(cops->CF_is_mounted())
                    viewNewDir(bdata,"/cf");
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
                printAllName(bdata);
                break;
            case EVT_CF_REMOVED:
                getcwd(pwd, 10);
                pwd[10]='\0';
                if(pwd[0]=='/' && pwd[1]=='c' && pwd[2]=='f' && (pwd[3]=='/'||pwd[3]=='\0'))
                    viewNewDir(bdata,"/mnt");
                break;
            case EVT_CF_ADDED:
                viewNewDir(bdata,"/cf");
                break;
        }
    }
    else
    {
        cops->menuEvtHandler(evt);
    }
    return 1;
}