/*
* menu_fct.c
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
#include <sys/stat.h>
#include <unistd.h>

#include "ls_main.h"
#include "msgBox.h"
#include "editBox.h"

struct menu_data menu_cfg = {
    useOwnDisp     : 1,
    x:200,y:120,width:8*16,height:100,
    dx:2,dy:2,
    txt_color      : COLOR_BLACK,
    bg_color       : COLOR_LIGHT_BLUE,
    select_color   : COLOR_BLUE,
    sub_color      : COLOR_RED,
    border_color   : COLOR_BLACK,
    has_border     : 0,
    title          : NULL,
    root           : NULL,
    right_action   : do_right,
    on_action      : do_on,
    off_action     : do_off,
    f1_action      : do_F1,
    f2_action      : do_F2,
    f3_action      : do_F3,
    item_str       : mk_item_str,
    submenu_str    : mk_item_str,
    getSubIcon     : NULL,
    getItemIcon    : NULL,
    isTxtMenu      : 1
};

struct ls_item {
    char * label;
    int num;
    struct browser_data * bdata;
};

struct menu_item menu1;
struct menu_item menu2;
struct menu_item menu3;
struct menu_item menu4;
struct menu_item menu5;
struct menu_item menu6;

#define MENU_COPY      1
#define MENU_DELETE    2
#define MENU_RENAME    3
#define MENU_NEW_DIR   4
#define MENU_CF        5
#define MENU_MOVE      6


struct ls_item ls_item1 = { "Copy"    , MENU_COPY};
struct ls_item ls_item2 = { "Move"    , MENU_MOVE};
struct ls_item ls_item3 = { "New Dir" , MENU_NEW_DIR};
struct ls_item ls_item4 = { "Delete"  , MENU_DELETE};
struct ls_item ls_item5 = { "Rename"  , MENU_RENAME};
struct ls_item ls_item6 = { "CF"      , MENU_CF};

struct browser_data browser2 = {
    path            : NULL,
    
    list            : NULL,
    listused        : 0,
    listsize        : 0,
    
    pos             : 0,
    nselect         : 0,
   
    scroll_pos      : LEFT_SCROLL,
    
    show_dot_files  : 1,

    nbFile          : 0,
    nbDir           : 0,
    totSize         : 0,
    
    nb_disp_entry   : 20,
    x_start         : 20,
    y_start         : 40,
    
    width           : 300,
    height          : 200,
    entry_height    : 10,
    
    draw_bottom_status : NULL,
    draw_file_size     : NULL,
    clear_status       : NULL
};
struct browser_data * bdata2;

int copy_mode;

extern int evt_mode;

/*extern variables */
extern struct client_operations * cops;
extern int pos,nselect;
extern struct dir_entry * list;
extern int listused;
extern struct helperMenu browserMenu;

extern struct browser_data * bdata;

/************************/

void ini_menu_struct(struct browser_data * bdata)
{
    ls_item1.bdata=bdata;
    ls_item2.bdata=bdata;
    ls_item3.bdata=bdata;
    ls_item4.bdata=bdata;
    ls_item5.bdata=bdata;
    ls_item6.bdata=bdata;

    menu1.data=(void*)&ls_item1;
    menu2.data=(void*)&ls_item2;
    menu3.data=(void*)&ls_item3;
    menu4.data=(void*)&ls_item4;
    menu5.data=(void*)&ls_item5;
    menu6.data=(void*)&ls_item6;

    menu1.prev=NULL;
    menu1.nxt=&menu2;
    menu1.sub=NULL;
    menu1.up=NULL;

    menu2.prev=&menu1;
    menu2.nxt=&menu3;
    menu2.sub=NULL;
    menu2.up=NULL;

    menu3.prev=&menu2;
    menu3.nxt=&menu4;
    menu3.sub=NULL;
    menu3.up=NULL;

    menu4.prev=&menu3;
    menu4.nxt=&menu5;
    menu4.sub=NULL;
    menu4.up=NULL;
    
    menu5.prev=&menu4;
    menu5.nxt=&menu6;
    menu5.sub=NULL;
    menu5.up=NULL;
    
    menu6.prev=&menu5;
    menu6.nxt=NULL;
    menu6.sub=NULL;
    menu6.up=NULL;
    
}

void do_off(void * data)
{
    cops->stop_menu();
    evt_mode=BRW_MODE;
}

void do_on(void * data)
{
}

char pwd[PATHLEN];
char pwd2[PATHLEN];

int copyMode=0;

void do_right(void * data)
{
    char text[MAX_EDIT_CHARS];
    char* ptext = NULL;
    int reload = false;
    int buttonResult = 0;
    struct ls_item * item=(struct ls_item*)data;
    //struct browser_data * bdata=item->bdata;
    
    cops->stop_menu();
    evt_mode=BRW_MODE;

    copy_mode=MODE_COPY;
    
    switch(item->num)
    {
        case MENU_MOVE:
            copy_mode=MODE_MOVE;
        case MENU_COPY:
            if(nbSelected(bdata)==0)
                cops->msgBox("Warning - copy", "Select files first", MSGBOX_TYPE_OK, MSGBOX_ICON_WARNING);
            else
            {
                /* saving current path */
                if(!getcwd(pwd,PATHLEN))
                {
                    cops->msgBox("Warning - copy", "Can't get current path", MSGBOX_TYPE_OK, MSGBOX_ICON_WARNING);
                    break;
                }                
                
                bdata2=&browser2;
                evt_mode=CP_MV_MODE;
                clearBrowser(bdata);
                cops->putS(COLOR_WHITE, COLOR_RED, 5,20, "Select the destination folder");
                cops->drawRect(COLOR_BLACK,bdata2->x_start-1,bdata2->y_start-1,bdata2->width+2,bdata2->height+2);
                if(!viewNewDir(bdata2,pwd))
                    stopLs();
            }
            break;
        case MENU_DELETE:
            buttonResult = cops->msgBox("Delete Warning", "Really delete it ?", MSGBOX_TYPE_OKCANCEL, MSGBOX_ICON_WARNING);
            if(buttonResult == MSGBOX_OK)
            {
                if(bdata->list[bdata->pos+bdata->nselect].type == TYPE_FILE)
                    unlink(bdata->list[bdata->pos+bdata->nselect].name);
                else
                    rmdir(bdata->list[bdata->pos+bdata->nselect].name);    
                reload = true;
            }
            break;
        case MENU_RENAME:
            buttonResult = cops->msgBox("Info", "Rename file not supported", MSGBOX_TYPE_OK, MSGBOX_ICON_INFORMATION);
            break;
        case MENU_NEW_DIR:            
            ptext = cops->editBox("Enter new dir name","", COLOR_BLACK,COLOR_WHITE,COLOR_BLACK,COLOR_RED);
            if(ptext != 0)
            {
                strcpy(text,ptext);
                mkdir(text, S_IRWXU);
                reload = true;
            }
            break;
        case MENU_CF:
            if(cops->CF_mod_is_connected())
            {
                if(cops->CF_is_mounted())
                    cops->umountCF();
                else
                {
                    cops->mountCF();
                    reload=true;
                }                    
            }
            break;
    }
    
    cops->iniHelperMenu(&browserMenu);
    
    if(reload == true)
        if(!viewNewDir(bdata,"./"))
            stopLs();
    
}

void do_F1(void * data)
{
}

void do_F2(void * data)
{
}

void do_F3(void * data)
{
}

void mk_item_str(void * data,char * str)
{
    struct ls_item * item=(struct ls_item*)data;
    switch(item->num)
    {
        case MENU_CF:
            if(!cops->CF_mod_is_connected())
                strcpy(str,"CF not connected");
            else if(cops->CF_is_mounted())
                strcpy(str,"umount CF");
            else
                strcpy(str,"mount CF");            
            break;
        default:
            strcpy(str,item->label);
            break;
    }
}

void cp_mv_evt(int evt)
{
    int pos_list=0;
    char src[PATHLEN];
    struct dir_entry * entry;
    evt=browserEvt(evt,bdata2);
    switch(evt)
    {
        //case BTN_RIGHT: dest has to be dir atm
        case BTN_ON:
            clearBrowser(bdata2);
            evt_mode=BRW_MODE;
            if(!getcwd(&pwd2,PATHLEN))
            {
                cops->msgBox("Warning - Copy/Move", "Can't get dest path", MSGBOX_TYPE_OK, MSGBOX_ICON_WARNING);
                if(!viewNewDir(bdata,pwd))
                    stopLs();
                break;
            }
            
            if(!strcmp(pwd,pwd2))
            {
                cops->msgBox("Warning - Copy/Move", "dest==src", MSGBOX_TYPE_OK, MSGBOX_ICON_WARNING);
                if(!viewNewDir(bdata,pwd))
                    stopLs();
                break;
            }
                        
            printf("copying from %s to %s\n",pwd,pwd2);
            
            while((entry=nxtSelect(bdata,&pos_list))!=NULL)
            {
                fprintf(stderr,"- %s %s",entry->name,entry->type==TYPE_DIR?"folder":"file");
                snprintf(src,PATHLEN,"%s/%s",pwd,entry->name);
                if(copy_mode)
                    if(!cops->do_mv(src,pwd2)) fprintf(stderr," - mv error\n"); else fprintf(stderr," - mv done\n");
                else
                    if(!cops->do_cp(src,pwd2)) fprintf(stderr," - cp error\n"); else fprintf(stderr," - cp done\n");                
            }
            if(!viewNewDir(bdata,pwd2))
                stopLs();
            break;
         case BTN_OFF:
             clearBrowser(bdata2);
             if(!viewNewDir(bdata,pwd))
                 stopLs();
             break;
    }
}

void draw_cp_mv_bottom(struct browser_data *bdata)
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
        snprintf(tmp,100,"%s - Copying/Move",pwd);
        cops->putS(COLOR_BLUE, COLOR_WHITE,2, 220, tmp);  
    }

    snprintf(tmp,100,"%d %s, %d %s, %s",bdata->nbFile,bdata->nbFile>0?"files":"file",
            bdata->nbDir,bdata->nbDir>0?"folders":"folders",tmpS);
    fprintf(stderr,"%s\n",tmp);
    
    cops->putS(COLOR_BLUE, COLOR_WHITE,2, 230, tmp);    
}
