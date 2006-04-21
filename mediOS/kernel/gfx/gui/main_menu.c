/*
* kernel/gfx/gui/main-menu.c
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <sys_def/string.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/graphics.h>
#include <kernel/med.h>
#include <kernel/evt.h>
#include <kernel/delay.h>
#include <kernel/disk.h>
#include <kernel/errors.h>
#include <kernel/usb_fw.h>
#include <kernel/lcd.h>

#include <evt.h>

#include <gui/parse_cfg.h>
#include <gui/menu.h>
#include <gui/main_menu.h>
#include <gui/file_browser.h>
#include <gui/gui.h>

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
    font           : STD6X9,
    root           : NULL,
    do_action      : mainMenu_doAction,
    on_action      : mainMenu_doOn,
    off_action     : mainMenu_doOff,
    f1_action      : mainMenu_doF1,
    f2_action      : mainMenu_doF2,
    f3_action      : mainMenu_doF3,
    item_str       : mainMenu_mkItemStr,
    submenu_str    : mainMenu_mkSubmenuStr,
    getSubIcon     : mainMenu_mkSubIcon,
    getItemIcon    : mainMenu_mkItemIcon,
    isTxtMenu      : 0,
    border_color   : COLOR_BLACK,
    has_border     : 0,
    title          : NULL,
};

extern int cfg_line_num;

struct icon_elem * sub_icon=NULL;
struct icon_elem * back_icon=NULL;
struct icon_elem * plugin_icon=NULL;


#define SHOW_ALL        1
#define LISTSIZE        256
#define PATHLEN         256

#define MENU_FILE_NAME "/menu.cfg"

int evt_hand;
struct browser_data * browseData;

void mainMenu_doOff(void * data)
{
    gui_sendEvt(EVT_MENU_UP_LVL); /* send a evt to do an up lvl */
}

void mainMenu_doOn(void * data)
{
    /* nothing to do */
}

void mainMenu_doAction(void * data)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;

    printk("type= %d\n",cfg_data->type);

    if(cfg_data->type==TYPE_INTERNAL)
    {
        printk("launching int app: %s\n",cfg_data->link);
        if(strcmp(cfg_data->link,"browser"))
        {
            printk("Error ukn int app : %s\n",cfg_data->link);
            return;
        }
        if(!browseData)
            return;

        browser_browse(browseData,NULL,NULL);

        evt_purgeHandler(evt_hand);
        mainMenu_start();
    }
    else
    {
        printk("launching ext app: %s\n",cfg_data->link);
        med_load(cfg_data->link);
        gfx_openGraphics();
        evt_purgeHandler(evt_hand);
        mainMenu_start();
    }


}

void mainMenu_doF1(void * data)
{

}

void mainMenu_doF2(void * data)
{
    // Settings ?
}

int usbMode=0;

void mainMenu_doF3(void * data) // switch to usb
{
    int h,w;
    int evt;
    char * msg1 = "In USB mode (F3 to exit)";

    if(usbMode)
    {
        // in usb mode => disable usb
        printk( "Warning should not be here\n");
#warning need some processing to see what to do here
    }
    else
    {
        // not in usb mode => enable usb if cable present
        if(usb_isConnected() || FW_isConnected())
        {
            if(disk_umount(HD_DRIVE,1)!=MED_OK)
            {
                printk("File still open, can't umount\n");
                return;
            }
            gfx_clearScreen(COLOR_WHITE);
            gfx_fontSet(STD8X13);
            gfx_getStringSize(msg1,&w,&h);
            gfx_putS(COLOR_RED,COLOR_WHITE,(SCREEN_WIDTH-w)/2,(SCREEN_HEIGHT-h)/2,msg1);
            enableUsbFw();
            usbMode=1;
            mdelay(10);
            evt_purgeHandler(evt_hand);
            while(1)
            {
                if((evt=evt_getStatus(evt_hand))<0)
                    printk("Bad evt (error:%d)\n",-evt);
                if(evt==BTN_F3)
                    break;
            }
            disableUsbFw();
            usbMode=0;
            //mdelay(5);
            disk_init();
#warning we should also reload menu.cfg or other menu related files
            evt_purgeHandler(evt_hand);
            mainMenu_start();
        }
        else
        {
            printk( "Warning can't go usb as usb cable is absent\n");
        }
    }
}

void mainMenu_mkSubmenuStr(void * data,char * str)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if((cfg_data->type == TYPE_STD || cfg_data->type == TYPE_INTERNAL) && menu_cfg.isTxtMenu)
        sprintf(str,"> %s",cfg_data->name);
    else
        sprintf(str,"%s",cfg_data->name);
}

void mainMenu_mkItemStr(void * data,char * str)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(menu_cfg.isTxtMenu)
        sprintf(str,"x %s",cfg_data->name);
    else
        sprintf(str,"%s",cfg_data->name);
}

BITMAP * mainMenu_mkSubIcon(void * data)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(cfg_data->type & TYPE_STD || cfg_data->type == TYPE_INTERNAL) /* std sub menu */
        return &sub_icon->bmap_data;
    else
        return &back_icon->bmap_data;

}

BITMAP * mainMenu_mkItemIcon(void * data)
{
    struct cfg_menu * cfg_data = (struct cfg_menu *)data;
    if(cfg_data->icon)
        return &cfg_data->icon->bmap_data;
    else
        return &plugin_icon->bmap_data;
}

void mainMenu_start(void)
{
    start_menu(&menu_cfg);
    gui_sendEvt(EVT_REDRAW);
}

void mainMenu_loop(void)
{
    int evt;

    if(evt_hand<0)
    {
        printk("Bad evt Handler\n");
        return;
    }

    while(1)
    {
        if((evt=evt_getStatus(evt_hand))<0)
            printk("Bad evt (error:%d)\n",-evt);
        statusLine_EvtHandler(evt);
        menu_EvtHandler(evt);
    }
}

int mainMenu_ini(void)
{
    int h,w;

    usbMode = 0;

    gfx_clearScreen(COLOR_BLACK);

    gfx_putS(COLOR_WHITE,COLOR_BLACK,5,110,"[ini_menu] reading menu file");
    rootMenu = NULL;
    if(mainMenu_load(MENU_FILE_NAME)<0)
    {
        gfx_putS(COLOR_RED,COLOR_BLACK,5,120,"[ini_menu] Error reading menu => stoping");
        return -1;
    }

    gfx_getStringSize("M", &w, &h);
    menu_cfg.root=rootMenu;
    menu_cfg.dx=5;
    menu_cfg.dy=h+11;

    printk("[ini_menu] menu loaded\n");

    gfx_putS(COLOR_WHITE,COLOR_BLACK,5,120,"[ini_menu] reading icons");

    /* loading icons */
    sub_icon=loadIcon("sub_icon.ico");
    back_icon=loadIcon("back_icon.ico");
    plugin_icon=loadIcon("plugin_icon.ico");

    if((evt_hand=evt_getHandler(ALL_CLASS))<0)
    {
        printk("Can't get evt handler (error:%d)\n",-evt_hand);
        evt_hand=-1;
    }

    browseData=browser_NewBrowse();

    browseData->mode=MODE_NOSELECT;

    gfx_putS(COLOR_WHITE,COLOR_BLACK,5,120,"[ini_menu] finished");

    return 0;
}

void mainMenu_clean(struct menu_item * root)
{
    struct menu_item * ptr;
    while(root!=NULL)
    {
        mainMenu_clean(root->sub);
        ptr=root->nxt;
        free(root);
        root=ptr;
    }
}

struct menu_item * mainMenu_newItem(struct cfg_menu * data)
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
        printk("Not enough space in memory, cant malloc\n");
    return ptr;
}

struct menu_item * mainMenu_findParent(struct menu_item * ptr, char * name)
{
    struct menu_item * ptr2;
    struct cfg_menu * cfg_data;
    while(ptr!=NULL)
    {
        cfg_data=(struct cfg_menu *)ptr->data;
        if(!strcmp(cfg_data->name,name))
            return ptr;
        if((ptr2=mainMenu_findParent(ptr->sub,name))!=NULL)
            return ptr2;
        ptr=ptr->nxt;
    }
    return NULL;

}

int mainMenu_insertItem(struct menu_item * item)
{
    struct menu_item * ptr;
    struct cfg_menu * cfg_data;
    cfg_data=(struct cfg_menu *)item->data;
    if(rootMenu==NULL)
    {
        if(cfg_data->parent[0] != 0)
        {
            printk("no sub defined and data is not in root => error\n");
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
            if((ptr=mainMenu_findParent(rootMenu,cfg_data->parent))!=NULL)
            {
                if(ptr->sub)
                    ptr->sub->prev=item;
                item->nxt=ptr->sub;
                ptr->sub=item;
                item->up=ptr;
            }
            else
            {
                printk("parent not found for %s (parent=%s)\n",cfg_data->name,cfg_data->parent);
                return -1; // parent not found
            }
        }
    }
    return 0;
}

void mainMenu_addItem(struct cfg_menu ** cfg)
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
    current_item->nxt=NULL;
    current_item->icon=NULL;
    current_item->nxt=NULL;
}

void mainMenu_cleanCfg(struct cfg_menu * cfg)
{
    struct cfg_menu * ptr;
    while(cfg!=NULL)
    {
        ptr=cfg->nxt;
        free(cfg);
        cfg=ptr;
    }
}

int mainMenu_parse(struct cfg_menu ** cfg,char * filename)
{
    char *item=item_buff;
    char *value=value_buff;

    if(openFile(filename,CFG_READ)<0)
        return -1;

    while (1)
    {
        if (!nxt_cfg(item,value)) break;
        if(!strcmp(item,"name"))
        {
            mainMenu_addItem(cfg);
            strcpy(current_item->name,value);
        }
        else if(!strcmp(item,"parent"))
        {
            if(current_item==NULL)
            {
                printk("'parent' param before name\n");
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
                printk("'link' param before name\n");
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
                printk("'param' param before name\n");
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
                printk("'icon' param before name\n");
            }
            else
            {
                current_item->icon=loadIcon(value);
            }
        }
        else if(!strcmp(item,"opt"))
        {
            if(current_item==NULL)
            {
                printk("'opt' param before name\n");
            }
            else
            {
                if(!strcmp(value,"is_internal"))
                {
                    current_item->type=TYPE_INTERNAL;
                }
                else
                    printk("Ukn opt for %s: %s\n",current_item->name,value);
            }
        }
        else
            printk("unknown item type: %s on line %d\n",item,cfg_line_num);
    }
    closeFile();
    return 0;
}

int mainMenu_doAddBackEntry(char * name,struct menu_item * up,struct menu_item *cur)
{
        struct menu_item * ptr;
        struct menu_item * start=cur;
        struct cfg_menu * data;

        if(up && name)
        {
            data=(struct cfg_menu *) malloc(sizeof(struct cfg_menu));
            if(!data)
            {
                printk("can't malloc back item\n");
                return 0;
            }
            ptr=mainMenu_newItem(data);
            if(!ptr)
            {
                printk("can't malloc back item\n");
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
        if(!mainMenu_doAddBackEntry(((struct cfg_menu *)ptr->data)->name,ptr,ptr->sub))
            return 0;
    }

    return 1;
}

void mainMenu_dispose(void)
{
    mainMenu_cleanCfg(cfgMenu);
    cfgMenu=NULL;
    mainMenu_clean(rootMenu);
    rootMenu=NULL;
}

int mainMenu_load(char * filename)
{
    struct cfg_menu * data;
    struct menu_item * new_item;
        printk("Reading: %s\n",filename);
    mainMenu_cleanCfg(cfgMenu);
    cfgMenu=NULL;
    mainMenu_clean(rootMenu);
    rootMenu=NULL;
    if(mainMenu_parse(&cfgMenu,filename)<0)
        return -1;
    data=cfgMenu;
    printk("parsing finished, building tree\n");
    while(data!=NULL)
    {
        if(!(new_item=mainMenu_newItem(data)))
            return -1;
        if(mainMenu_insertItem(new_item)<0)
        {
            /*cfgCleanMenu(cfgMenu); !!!!!!!!!!!!! do clean when everything is working
            cleanMenu(rootMenu);*/
            printk("Error building menu tree\n");
            return -1;
        }
        data=data->nxt;
    }

    mainMenu_doAddBackEntry(NULL,NULL,rootMenu);
    return 0;
}

void mainMenu_print(void)
{
    struct cfg_menu * ptr=cfgMenu;
    printk("cfg:\n");
    while(ptr)
    {
        printk("%s %s %s\n",ptr->name,ptr->parent,ptr->link);
        ptr=ptr->nxt;
    }
    printk("Menu:\n");
    doPrint(rootMenu,0);
}
