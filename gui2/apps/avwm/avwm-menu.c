/*
* avwm-menu.c
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
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "misc.h"
#include "graphics.h"
#include "plugin.h"
#include "parse_cfg.h"
#include "menu.h"
#include "avwm-menu.h"
#include "avwm.h"
#include "avevents.h"
#include "events.h"
#include "colordef.h"
#include "settings.h"
#include "helperMenu.h"

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

struct plugin * menu_plug;

struct cfg_menu * cfgMenu=NULL;
struct menu_item * rootMenu=NULL;

struct cfg_menu * current_item=NULL;

struct menu_data menu_cfg = {
    useOwnDisp     : 0,
    x:0,y:0,width:320,height:240,
    txt_color      : COLOR_BLACK,
    bg_color       : COLOR_WHITE,
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
    submenu_str    : mk_submenu_str,
    getSubIcon     : mk_sub_icon,
    getItemIcon    : mk_item_icon,
    isTxtMenu      : 0
};

struct helperMenu menuMenu = {
    ON_txt        : "Select",
    OFF_txt       : "UP",
    JOY_txt       : "Nav/Sel",
    F1_txt        : "",
    F2_txt        : "Settings",
    F3_txt        : "USB/FW",
    
    helperDelay   :  1,
    helperSpeed   :  300,
    
    bg_color      : COLOR_WHITE,
    border_color  : COLOR_BLUE,
    txt_color     : COLOR_BLACK,
    
    align         : ALIGN_RIGHT
};

extern int cfg_line_num;

struct icon_elem * sub_icon=NULL;
struct icon_elem * back_icon=NULL;
struct icon_elem * plugin_icon=NULL;


#define SHOW_ALL        1
#define LISTSIZE        256
#define PATHLEN         256

#define MENU_FILE_NAME "menu.cfg"

extern struct plugin menu_plugin;

void enableMenu(void)  {menu_plugin.allowed=1;}
void disableMenu(void) {menu_plugin.allowed=0;}
int  menuStatus(void)  {return menu_plugin.allowed;}

extern int stopWM;
void do_off(void * data)
{
    menuEvtHandler(EVT_MENU_UP_LVL); /* send a evt to do an up lvl */
}

void do_on(void * data)
{
    /* nothing to do */
}

void do_right(void * data)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(cfg_data->link[0]!=0)
    {
        if(cfg_data->param[0]!=0)
        {
            loadPlugin(cfg_data->link,cfg_data->param);     
        }
        else
        {
            loadPlugin(cfg_data->link,NULL);
        }
    }
}

void do_F1(void * data)
{

}

void do_F2(void * data)
{
    SettingsScreen();
}

void do_F3(void * data) // switch to usb
{
    int pid,status; 
    
    if(getUSB() || getFwExt())
    {    
        pid = vfork();
        
        if (pid == 0)
        {
            
            execl("/mnt/avwm/apps/enableUSB","/mnt/avwm/apps/enableUSB",(char *)0);
            
            fprintf(stderr, "exec failed!\n");
            _exit(1);        
        }
        else
        {
            if (pid > 0)
            {
                close_graphics();            
                waitpid(pid, &status, 0);
                ini_graphics();      
            }
            else
            {
                fprintf(stderr, "vfork failed %d\n", pid);
            }
        }
    }
    else
    {
        fprintf(stderr, "Warning can't go usb as usb cable absent\n");
    }
}

void mk_submenu_str(void * data,char * str)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(cfg_data->type == TYPE_STD && menu_cfg.isTxtMenu)
        sprintf(str,"> %s",cfg_data->name);
    else
        sprintf(str,"%s",cfg_data->name);
}

void mk_item_str(void * data,char * str)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(menu_cfg.isTxtMenu)
        sprintf(str,"x %s",cfg_data->name);
    else
        sprintf(str,"%s",cfg_data->name);
}

BITMAP *mk_sub_icon(void * data)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(cfg_data->type == TYPE_STD) /* std sub menu */
        return &sub_icon->bmap_data;
    else
        return &back_icon->bmap_data;
    
}

BITMAP * mk_item_icon(void * data)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(cfg_data->icon)
        return &cfg_data->icon->bmap_data;
    else
        return &plugin_icon->bmap_data;
}

void avwm_menuEvtHandler(int evt)
{
    helperEvt(evt,BTN_JOY);
    if(evt==EVT_REDRAW)
    {
        /* making sure that menu is on */        
        iniHelperMenu(&menuMenu);
        start_menu(&menu_cfg);
    }
    menuEvtHandler(evt);
}

int ini_menu(char * path,struct plugin * plug)
{
    char * tmpC;
    int i,h,w;
    
    menu_plug=plug;
        
    clearScreen(COLOR_BLACK);
    
    putS(COLOR_WHITE,COLOR_BLACK,5,110,"[ini_menu] reading menu file");
    
    tmpC=(char*)malloc(sizeof(char)*(strlen(path)+1+strlen(MENU_FILE_NAME)+1));
    sprintf(tmpC,"%s/%s",path,MENU_FILE_NAME);
    
    if(loadMenu(tmpC)<0)
    {
        putS(COLOR_RED,COLOR_BLACK,5,120,"[ini_menu] Error reading menu => stoping");
        for(i=0;i<10000;i++) /* nothing */
        free(tmpC);
        return 0;
    }
    
    getStringS("M", &w, &h);
    menu_cfg.root=rootMenu;
    menu_cfg.dx=5;
    menu_cfg.dy=h+6+MENU_SHADOW;

    fprintf(stderr,"[ini_menu] menu loaded\n");
    
    iniHelperMenu(&menuMenu);    
    doRegisterPlugin(menu_plug,avwm_menuEvtHandler,0);
    
    fprintf(stderr,"[ini_menu] helper menu added\n");
    
    /* loading icons */
    sub_icon=loadIcon("sub_icon.ico");
    back_icon=loadIcon("back_icon.ico");
    plugin_icon=loadIcon("plugin_icon.ico");
    
    free(tmpC);
    return 1;
}

void cleanMenu(struct menu_item * root)
{
    struct menu_item * ptr;
    while(root!=NULL)
    {
        cleanMenu(root->sub);
        ptr=root->nxt;
        free(root);
        root=ptr;
    }
}

struct menu_item * newItem(struct cfg_menu * data)
{
    struct menu_item * ptr=(struct menu_item *) malloc(sizeof(struct menu_item));
    if(ptr)
    {
        ptr->data=data;
        ptr->nxt=NULL;
        ptr->prev=NULL;
        ptr->sub=NULL;
        ptr->up=NULL;
    }
    else
        fprintf(stderr,"Not enough space in memory, cant malloc\n");
    return ptr;
}

struct menu_item * findParent(struct menu_item * ptr, char * name)
{
    struct menu_item * ptr2;
    struct cfg_menu * cfg_data;
    while(ptr!=NULL)
    {
        cfg_data=(struct cfg_menu *)ptr->data;
        if(!strcmp(cfg_data->name,name))
            return ptr;
        if((ptr2=findParent(ptr->sub,name))!=NULL)
            return ptr2;
        ptr=ptr->nxt;
    }
    return NULL;
    
}

int insertItem(struct menu_item * item)
{
    struct menu_item * ptr;
    struct cfg_menu * cfg_data;
    cfg_data=(struct cfg_menu *)item->data;
    if(rootMenu==NULL)
    {
        if(cfg_data->parent[0] != 0) 
        {
            fprintf(stderr,"no sub defined and data is not in root => error\n");
            return -1; // no sub defined and data is not in root => error
        }
        else
            rootMenu=item; // no root => item is first item
    }
    else
    {
        if(cfg_data->parent[0] == 0) // no parent => add it on top of root
        {
            rootMenu->prev=item;
            item->nxt=rootMenu;
            rootMenu=item;
        }
        else
        {
            if((ptr=findParent(rootMenu,cfg_data->parent))!=NULL)
            {
                if(ptr->sub)
                    ptr->sub->prev=item;
                item->nxt=ptr->sub;
                ptr->sub=item;
                item->up=ptr;
            }
            else
            {
                fprintf(stderr,"parent not found %s\n",cfg_data->name);
                return -1; // parent not found
            }
        }
    }
    return 0;
}

void addItem(struct cfg_menu ** cfg)
{
    struct cfg_menu * ptr =(struct cfg_menu *) malloc(sizeof(struct cfg_menu));
    if(current_item == NULL)
        *cfg=ptr;
    else
        current_item->nxt=ptr;
    current_item=ptr;
    current_item->name[0]=0;
    current_item->link[0]=0;
    current_item->parent[0]=0;
    current_item->param[0]=0;
    current_item->type=TYPE_STD;
    current_item->icon=NULL;
}

void cfgCleanMenu(struct cfg_menu * cfg)
{
    struct cfg_menu * ptr;
    while(cfg!=NULL)
    {
        ptr=cfg->nxt;
        free(cfg);
        cfg=ptr;
    }
}

int do_parse(struct cfg_menu ** cfg,char * filename)
{
    char *item=item_buff;
    char *value=value_buff;

    openFile(filename,CFG_READ);

    while (1)
    {
        if (!nxt_cfg(item,value)) break;
        if(!strcmp(item,"name"))
        {
            addItem(cfg);
            strcpy(current_item->name,value);
        }
        else if(!strcmp(item,"parent"))
        {
            if(current_item==NULL)
            {
                fprintf(stderr,"'parent' param before name\n");
            }
            else
            {
                strcpy(current_item->parent,value);
            }
    
        }
        else if(!strcmp(item,"link"))
        {
            if(current_item==NULL)
            {
                fprintf(stderr,"'link' param before name\n");
            }
            else
            {
                strcpy(current_item->link,value);
            }
        }
        else if(!strcmp(item,"param"))
        {
            if(current_item==NULL)
            {
                fprintf(stderr,"'param' param before name\n");
            }
            else
            {
                strcpy(current_item->param,value);
            }
        }
        else if(!strcmp(item,"icon"))
        {
            if(current_item==NULL)
            {
                fprintf(stderr,"'icon' param before name\n");
            }
            else
            {
                fprintf(stderr,"icon:|%s|\n",value);
                current_item->icon=loadIcon(value);
            }
        }
        else
            fprintf(stderr,"unknown item type: %s on line %d\n",item,cfg_line_num);
    }
    closeFile();
    return 0;
}

int doAddBackEntry(char * name,struct menu_item * up,struct menu_item *cur)
{
        struct menu_item * ptr;
        struct menu_item * start=cur;
        struct cfg_menu * data;
        
        if(up && name)
        {  
            data=(struct cfg_menu *) malloc(sizeof(struct cfg_menu));
            if(!data)
            {
                fprintf(stderr,"can't malloc back item\n");
                return 0;
            }
            ptr=newItem(data);
            if(!ptr)
            {
                fprintf(stderr,"can't malloc back item\n");
                return 0;
            }
            if(menu_cfg.isTxtMenu)
                sprintf(data->name,"<-  %s",name);
            else
                sprintf(data->name,"%s",name);
            data->type=TYPE_BACK;
            if(cur)
                cur->prev=ptr;            
            ptr->nxt=cur;
            cur->up->sub=ptr;
            ptr->up=cur->up;
            if(ptr->up->up)
                ptr->sub=ptr->up->up;
            else
                ptr->sub=rootMenu;
    }
    
    for(ptr=start;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!doAddBackEntry(((struct cfg_menu *)ptr->data)->name,ptr,ptr->sub))
            return 0;
    }

    return 1;
}

int loadMenu(char * filename)
{
    struct cfg_menu * data;
    struct menu_item * new_item;
        fprintf(stderr,"Reading: %s\n",filename);
    cfgCleanMenu(cfgMenu);
    cfgMenu=NULL;
    cleanMenu(rootMenu);
    rootMenu=NULL;
    if(do_parse(&cfgMenu,filename)<0)
        return -1;
    data=cfgMenu;
    while(data!=NULL)
    {
        if(!(new_item=newItem(data)))
            return -1;
        if(insertItem(new_item)<0)
        {
            /*cfgCleanMenu(cfgMenu); !!!!!!!!!!!!! do clean when everything is working
            cleanMenu(rootMenu);*/
            fprintf(stderr,"Error building menu tree\n");
            return -1;
        }
        data=data->nxt;
    }
    
    doAddBackEntry(NULL,NULL,rootMenu);
    
    return 0;
}

void printMenu(void)
{
    struct cfg_menu * ptr=cfgMenu;
    printf("cfg:\n");
    while(ptr)
    {
        printf("%s %s %s\n",ptr->name,ptr->parent,ptr->link);
        ptr=ptr->nxt;
    }
    printf("Menu:\n");
    doPrint(rootMenu,0);
}
