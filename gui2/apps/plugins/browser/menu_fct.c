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
#include <sys/stat.h>

#include "ls_main.h"
#include "msgBox.h"
#include "editBox.h"

struct menu_data menu_cfg = {
    useOwnDisp     : 1,
    x:200,y:120,width:8*16,height:100,
    dx:2,dy:2,
    txt_color      : COLOR_WHITE,
    bg_color       : COLOR_GREEN,
    select_color   : COLOR_BLUE,
    sub_color      : COLOR_RED,
    root           : NULL,
    right_action   : do_right,
    on_action      : do_on,
    off_action     : do_off,
    f1_action      : do_F1,
    f2_action      : do_F2,
    f3_action      : do_F3,
    item_str       : mk_item_str,
    submenu_str    : mk_item_str
};

struct ls_item {
    char * label;
    int num;
};

struct menu_item menu1;
struct menu_item menu2;
struct menu_item menu3;
struct menu_item menu4;
struct menu_item menu5;

#define MENU_COPY      1
#define MENU_DELETE    2
#define MENU_RENAME    3
#define MENU_NEW_DIR   4
#define MENU_CF        5

struct ls_item ls_item1 = { "New Dir" , MENU_NEW_DIR};
struct ls_item ls_item2 = { "Delete"  , MENU_DELETE};
struct ls_item ls_item3 = { "Rename"  , MENU_RENAME};
struct ls_item ls_item4 = { "Copy"    , MENU_COPY};
struct ls_item ls_item5 = { "CF"      , MENU_CF};



int menuOff=1;

/*extern variables */
extern struct client_operations * cops;
extern int pos,nselect;
extern struct dir_entry * list;
extern int listused;
/************************/

void ini_menu_struct(void)
{
    menu1.data=(void*)&ls_item1;
    menu2.data=(void*)&ls_item2;
    menu3.data=(void*)&ls_item3;
    menu4.data=(void*)&ls_item4;
    menu4.data=(void*)&ls_item5;

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
    menu4.nxt=NULL;
    menu4.sub=NULL;
    menu4.up=NULL;
    
    menu5.prev=&menu4;
    menu5.nxt=NULL;
    menu5.sub=NULL;
    menu5.up=NULL;
    
}

void do_off(void * data)
{
    cops->stop_menu();
    menuOff=1;
}

void do_on(void * data)
{
}

void do_right(void * data)
{
    char tmp[200];
    char text[MAX_EDIT_CHARS];
    char* ptext = NULL;
    int reload = false;
    int buttonResult = 0;
    struct ls_item * item=(struct ls_item*)data;
    
    cops->stop_menu();
    menuOff=1;

    switch(item->num)
    {
        case MENU_COPY:
            buttonResult = cops->msgBox("Info", "Copy file not supported", MSGBOX_TYPE_OK, MSGBOX_ICON_INFORMATION);
            break;
        case MENU_DELETE:
            buttonResult = cops->msgBox("Delete Warning", "Really delete it ?", MSGBOX_TYPE_OKCANCEL, MSGBOX_ICON_WARNING);
            if(buttonResult == MSGBOX_OK)
            {
                if(list[pos+nselect].type == TYPE_FILE)
                    unlink(list[pos+nselect].name);
                else
                    rmdir(list[pos+nselect].name);    
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
            do_mount("/dev/avcf1","/cf");
            break;
    }

    if(reload == true)
    {
        doLs("./");
        printAllName(pos,nselect);

        if(listused>MAXPOS)
            showArrow(DOWN_ARROW);
    }
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
            strcpy(str,item->label);
            break;
        default:
            strcpy(str,item->label);
            break;
    }
}


