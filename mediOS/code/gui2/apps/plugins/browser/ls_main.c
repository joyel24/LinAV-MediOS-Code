/*
* ls-gui.c
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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "ls_main.h"
#include "helperMenu.h"

struct client_operations * cops;

struct helperMenu browserMenu = {
    ON_txt        : "Select",
    OFF_txt       : "Quit",
    JOY_txt       : "Nav/Sel",
    F1_txt        : "HD",
    F2_txt        : "CF",
    F3_txt        : "Menu",
    
    helperDelay   :  1,
    helperSpeed   :  300,
    
    bg_color      : COLOR_WHITE,
    border_color  : COLOR_BLUE,
    txt_color     : COLOR_BLACK,
    
    align         : ALIGN_RIGHT
};

int evt_mode=BRW_MODE;
extern struct menu_item menu1;
extern struct menu_data menu_cfg;

struct browser_data realData = {
    path            : NULL,
    
    list            : NULL,
    listused        : 0,
    listsize        : 0,
    
    scroll_pos      : LEFT_SCROLL,
    
    pos             : 0,
    nselect         : 0,
   
    show_dot_files  : 1,

    nbFile          : 0,
    nbDir           : 0,
    totSize         : 0,
    
    nb_disp_entry   : 20,
    x_start         : 0,
    y_start         : 18,
    
    width           : 320,
    height          : 202,
    entry_height    : 10,
    
    draw_bottom_status : draw_bottom_status,
    draw_file_size     : draw_file_size,
    clear_status       : clear_status
};

struct browser_data * bdata;

void printList(struct browser_data * bdata,int val)
{
    int i;
    fprintf(stderr,"%d: Files:%d Dir:%d Size:%d ListUsed:%d\n",val,bdata->nbFile,bdata->nbDir,bdata->totSize,bdata->listused);
    fprintf(stderr,"liste @:%x\n",(int)bdata->list);
    for(i=0;i<bdata->listused;i++)
        fprintf(stderr,"%d: (%s) %s\n",i,bdata->list[i].type==TYPE_FILE?"F":"D",bdata->list[i].name);
    fprintf(stderr,"---------------------------- %d\n",val);
}

void stopLs(void)
{
    cops->hideHelper();
    RELEASE(cops)
}

int eventHandler(int evt)
{
    int w = 0;
    int h = 10;
    char pwd[10];  

    cops->getStringS("M", &w, &h);

    switch(evt_mode)
    {
        case BRW_MODE:
            cops->helperEvt(evt,BTN_JOY);
            evt=browserEvt(evt,bdata);
            switch(evt)
            {
                case BTN_RIGHT:
                    if(bdata->list[bdata->pos+bdata->nselect].type==TYPE_FILE)
                    {                    
                        cops->handle_type_other(bdata->list[bdata->pos+bdata->nselect].name);                    
                    }
                    break;
                case BTN_F1:
                    if(!viewNewDir(bdata,"/mnt"))
                        stopLs();
                    break;
                case BTN_F2:
                    if(cops->CF_is_mounted())
                        if(!viewNewDir(bdata,"/cf"))
                            stopLs();
                    break;
                case BTN_F3:
                    menu_cfg.root=&menu1;
                    evt_mode=MENU_MODE;
                    cops->hideHelper();
                    cops->start_menu(&menu_cfg);
                    cops->menuEvtHandler(EVT_REDRAW);
                    break;
                case BTN_ON:
                    chgSelect(bdata,bdata->pos+bdata->nselect);
                    break;
                case BTN_OFF:
                case EVT_QUIT:
                    cops->hideHelper();
                    RELEASE(cops)
                    break;
                case EVT_REDRAW:
                    break;
                case EVT_CF_REMOVED:
                    getcwd(pwd, 10);
                    pwd[10]='\0';
                    if(pwd[0]=='/' && pwd[1]=='c' && pwd[2]=='f' && (pwd[3]=='/'||pwd[3]=='\0'))
                        if(!viewNewDir(bdata,"/mnt"))
                            stopLs();
                    break;
                case EVT_CF_ADDED:
                    if(!viewNewDir(bdata,"/cf"))
                        stopLs();
                    break;
            }
            break;
        case MENU_MODE:
            cops->menuEvtHandler(evt);
            break;
        case CP_MV_MODE:
            cp_mv_evt(evt);
            break;
    }
    return 1;
}

int main(int argc,char * * argv)
{
    REGISTER(cops,eventHandler,0);

    /*pos=0;
    nselect=0;*/
    bdata = &realData;

    iniBrowser();
    
    if(argc>0)
    {
        cops->disableMenu();
        cops->setFont(STD6X9);

        ini_menu_struct(bdata);
        cops->iniHelperMenu(&browserMenu);
        //openHelper();
        
        if(strlen(argv[1]) == 1)
        {
            if(argv[1][0] == '/')
                chdir("./"); // handle spezial case if path only / without dot
            else
                chdir(argv[1]);
        }
        else
            chdir(argv[1]);

        if(!viewNewDir(bdata,"./"))
        {
            STOPME(cops)
            return -1;
        }
        
        PACK(cops,NULL);

        cleanList(bdata);
        return 0;
    }
    STOPME(cops)
    return -1;
}

int x=320;

void draw_file_size(struct dir_entry * entry)
{
    int h,w;
    char tmpS[15];
    
    /* erase previsous drawing */

    cops->fillRect(COLOR_WHITE,x, 230,320-x,10);
    if(entry->type == TYPE_FILE)
    {
        cops->createSizeString(tmpS,entry->size);
        cops->getStringS(tmpS,&w,&h);
        x=320-w;
        cops->putS(COLOR_BLUE, COLOR_WHITE,x, 230, tmpS);
    }
}

void draw_bottom_status(struct browser_data *bdata)
{
    char tmp[100];
    char tmpS[15];
    char pwd[PATHLEN];
    int len=0;   
    
    cops->createSizeString(tmpS,bdata->totSize);
        
    cops->fillRect(COLOR_WHITE,2, 220,316,20);
        
    if (!getcwd(pwd, PATHLEN))
    {
        fprintf(stderr, "Cannot get current directory\n");        
    }
    else
    {
        len=strlen(pwd);        
        cops->putS(COLOR_BLUE, COLOR_WHITE,2, 220, pwd);  
    }

    snprintf(tmp,100,"%d %s, %d %s, %s",bdata->nbFile,bdata->nbFile>0?"files":"file",
            bdata->nbDir,bdata->nbDir>0?"folders":"folders",tmpS);
    fprintf(stderr,"%s\n",tmp);
    
    cops->putS(COLOR_BLUE, COLOR_WHITE,2, 230, tmp);    
}

void clear_status(struct browser_data *bdata)
{
    cops->fillRect(COLOR_WHITE,2, 220,316,20);
}
