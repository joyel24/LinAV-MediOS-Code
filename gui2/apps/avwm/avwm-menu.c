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
#include <sys/stat.h>
#include <dirent.h>

#include "plugin.h"
#include "parse_cfg.h"
#include "menu.h"
#include "avwm-menu.h"
#include "avwm.h"
#include "avevents.h"
#include "colordef.h"

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
    submenu_str    : mk_submenu_str
};

extern int cfg_line_num;

#define SHOW_ALL        1
#define LISTSIZE        256
#define PATHLEN         256

#define MENU_FILE_NAME "menu.cfg"

extern int stopWM;
void do_off(void * data)
{
    /* nothing to do */
    //stopWM=1; /* use this if you want to be able to quit avwm without hakting the device*/
    /* to be done: call resume evt on plugin if it exists */
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
                int status;
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
    sprintf(str,"> %s",cfg_data->name);
}

void mk_item_str(void * data,char * str)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    sprintf(str,"x %s",cfg_data->name);
}

void avwm_menuEvtHandler(int evt)
{
    if(evt==EVT_REDRAW)
    {
        /* making sure that menu is on */
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
    
    putS(COLOR_WHITE,COLOR_BLACK,5,110,"Reading menu file ....");
    
    tmpC=(char*)malloc(sizeof(char)*(strlen(path)+1+strlen(MENU_FILE_NAME)));
    sprintf(tmpC,"%s/%s",path,MENU_FILE_NAME);
    
    if(loadMenu(tmpC)<0)
    {
        putS(COLOR_RED,COLOR_BLACK,5,120,"Error reading menu => stoping");
        for(i=0;i<10000;i++) /* nothing */
        free(tmpC);
        return 0;
    }
    
    getStringS("M", &w, &h);
    menu_cfg.root=rootMenu;
    menu_cfg.dx=5;
    menu_cfg.dy=h+6+MENU_SHADOW;
        
#ifdef DO_DEBUG
    printMenu();
#endif

    doRegisterPlugin(menu_plug,avwm_menuEvtHandler,0);
    
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
            return -1; // no sub defined and data is not in root => error
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
                return -1; // parent not found
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
                fprintf(stderr,"'label' param before image\n");
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
                fprintf(stderr,"'link' param before image\n");
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
                fprintf(stderr,"'param' param before image\n");
            }
            else
            {
                strcpy(current_item->param,value);
            }
        }
        else
            fprintf(stderr,"unknown item type: %s on line %d\n",item,cfg_line_num);
    }
    closeFile();
    return 0;
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
