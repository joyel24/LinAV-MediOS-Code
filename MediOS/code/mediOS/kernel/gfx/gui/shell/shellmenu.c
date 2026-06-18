/*
* kernel/gui/gfx/shell/shellmenu.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/shellmenu.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/stdfs.h>
#include <kernel/cfg_file.h>
#include <kernel/evt.h>
#include <kernel/icons.h>
#include <kernel/iconmenu.h>
#include <kernel/widgetlist.h>
#include <kernel/lcd.h>

#include <sys_def/stddef.h>
#include <sys_def/string.h>

ICON shellMenu_folderIcon = NULL;
ICON shellMenu_backIcon = NULL;
ICON shellMenu_appIcon = NULL;

WIDGETLIST shellMenu_menuList = NULL;
ICONMENU shellMenu_rootMenu = NULL;

typedef struct SHELLMENU_ITEM_STRUCT * SHELLMENU_ITEM;

struct SHELLMENU_ITEM_STRUCT {
    char * name;
    char * param;
    char * command;
    char * icon;

    int event;
    bool global;
    
    bool listView;

    SHELLMENU_ITEM parent;
    SHELLMENU_ITEM next;
};

SHELLMENU_ITEM shellMenu_firstItem = NULL;
SHELLMENU_ITEM shellMenu_lastItem = NULL;

bool rootListView=false;

static void shellMenu_onClick(ICONMENU menu, ICONMENU_ITEM menuItem){
    SHELLMENU_ITEM item;

    item=menuItem->data;

    if(item==NULL){
        printk("[shell menu] critical error: onClick on a menuItem with NULL as shellMenu item\n");
        return;
    }

    if(item->command!=NULL){
        // execute corresponding command
        shell_execute(item->command,item->param);
    }else{
        printk("[shell menu] error: onClick on a menuItem with no associated command\n");
    }
}

static SHELLMENU_ITEM shellMenu_addItem(){
    SHELLMENU_ITEM item;

    // alloc memory
    item=malloc(sizeof(*item));

    // init members
    item->name=NULL;
    item->param=NULL;
    item->command=NULL;
    item->icon=NULL;
    item->event=NO_EVENT;
    item->global=false;
    item->listView=false;
    item->parent=NULL;
    item->next=NULL;

    // handle linked list
    if(shellMenu_firstItem==NULL) shellMenu_firstItem=item;
    if(shellMenu_lastItem!=NULL) shellMenu_lastItem->next=item;
    shellMenu_lastItem=item;

    return item;
}

static bool shellMenu_parse(char * filename){
    char * name;
    char * value;
    SHELLMENU_ITEM item = NULL;

    if(!cfg_readFile(filename)) return false;

    printk("[shell menu] parsing '%s'\n",filename);

    cfg_rewindItems();

    while(cfg_nextItem(&name,&value)){
        if(!strcmp(name,"name")){
            item=shellMenu_addItem();
            item->name=strdup(value);

        }else if (item==NULL){
            if(!strcmp(name,"flags")){
                if(strstr(value,"list_view")){
                    rootListView=true;
                };
            }else{
                printk("[shell menu] parse error: '%s' not allowed before 'name'\n",name);
            }
        }else if(!strcmp(name,"parent")){
            SHELLMENU_ITEM parent;

            // find parent
            parent=shellMenu_firstItem;
            while(parent!=NULL){
                if(!strcmp(parent->name,value)){
                    item->parent=parent;
                    break;
                }

                parent=parent->next;
            }

            if(item->parent==NULL) printk("[shell menu] parse error: parent '%s' not found for '%s'\n",value,item->name);

        }else if(!strcmp(name,"param")){
            item->param=strdup(value);

        }else if(!strcmp(name,"command")){
            item->command=strdup(value);

        }else if(!strcmp(name,"icon")){
            item->icon=strdup(value);

        }else if(!strcmp(name,"event")){
            item->event=atoi(value);

        }else if(!strcmp(name,"flags")){
            if(strstr(value,"global_event")){
                item->global=true;
            };
            if(strstr(value,"list_view")){
                item->listView=true;
            };
        }else{
            printk("[shell menu] parse error: unknown parameter '%s'\n",name);
        }
    }

    cfg_clear();

    return true;
}

static bool shellMenu_build(){
    SHELLMENU_ITEM item;
    ICONMENU menu;
    ICONMENU_ITEM menuItem;
    ICON icon;
    int i;

    printk("[shell menu] building menu\n");

    // create menu list & root menu
    shellMenu_menuList=widgetList_create();
    shellMenu_menuList->setRect(shellMenu_menuList,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
    shellMenu_menuList->repaintAllWidgets=false;

    shellMenu_rootMenu=iconMenu_create();
    shellMenu_rootMenu->setRect(shellMenu_rootMenu,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
    shellMenu_rootMenu->menuList=shellMenu_menuList;
    shellMenu_rootMenu->onClick=(MENU_CLICKEVENT)shellMenu_onClick;

    if(rootListView){
        shellMenu_rootMenu->itemWidth=SHELL_MENU_LISTVIEW_WIDTH;
        shellMenu_rootMenu->itemHeight=SHELL_MENU_LISTVIEW_HEIGHT;
    }else{
        shellMenu_rootMenu->itemWidth=SHELL_MENU_ICONVIEW_WIDTH;
        shellMenu_rootMenu->itemHeight=SHELL_MENU_ICONVIEW_HEIGHT;
    }

    shellMenu_menuList->addWidget(shellMenu_menuList,shellMenu_rootMenu);
    shellMenu_menuList->setFocusedWidget(shellMenu_menuList,shellMenu_rootMenu);

    item=shellMenu_firstItem;

    // create items & submenus
    while(item!=NULL){

        if(item->event==NO_EVENT){ // event items shouldn't appear in the menu

            menu=shellMenu_rootMenu;

            if(item->parent!=NULL){
                menu=NULL;

                // find the menu of the item
                for(i=0;i<shellMenu_menuList->widgetCount;++i){
                    if(shellMenu_menuList->widgets[i]->data==item->parent){
                        menu=(ICONMENU)shellMenu_menuList->widgets[i];
                        break;
                    }
                }

                // if it does not exist, build it and link the corresponding folder item
                if(menu==NULL){
                    MENU parentMenu = NULL;

                    // find the parent menu of the menu
                    for(i=0;i<shellMenu_menuList->widgetCount;++i){
                        if(shellMenu_menuList->widgets[i]->data==item->parent->parent){
                            parentMenu=(MENU)shellMenu_menuList->widgets[i];
                            break;
                        }
                    }

                    // find the folder item in the parent menu of the menu
                    menuItem=NULL;
                    if(parentMenu!=NULL){
                        for(i=0;i<parentMenu->itemCount;++i){
                            if(parentMenu->items[i]->data==item->parent){
                                menuItem=(ICONMENU_ITEM)parentMenu->items[i];
                                break;
                            }
                        }
                    }

                    if(parentMenu==NULL || menuItem==NULL){
                        printk("[shell menu] critical error: attempting to build an orphan menu\n");
                        return false;
                    }

                    // build menu
                    menu=iconMenu_create();
                    menu->setRect(menu,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
                    menu->menuList=shellMenu_menuList;
                    menu->parentMenu=parentMenu;
                    menu->data=item->parent;
                    menu->onClick=(MENU_CLICKEVENT)shellMenu_onClick;

                    if(item->parent->listView){
                        menu->itemWidth=SHELL_MENU_LISTVIEW_WIDTH;
                        menu->itemHeight=SHELL_MENU_LISTVIEW_HEIGHT;
                    }else{
                        menu->itemWidth=SHELL_MENU_ICONVIEW_WIDTH;
                        menu->itemHeight=SHELL_MENU_ICONVIEW_HEIGHT;
                    }

                    shellMenu_menuList->addWidget(shellMenu_menuList,menu);
    
                    // link the folder item to the menu
                    menuItem->subMenu=(MENU)menu;
                }
            }
    
            // build menu item
            menuItem=iconMenuItem_create();
            menuItem->data=item;
            menuItem->caption=item->name;

            if((item->parent!=NULL && item->parent->listView) || (item->parent==NULL && rootListView)){
                menuItem->iconPosition=IMIP_LEFT;
            }else{
                menuItem->iconPosition=IMIP_TOP;
            }

            icon=NULL;
            if(item->icon==NULL){
                if(item->command==NULL){ // no command -> folder
                    icon=shellMenu_folderIcon;
                }else{
                    icon=shellMenu_appIcon;
                }
            }else{
                icon=icon_load(item->icon);
            }
    
            if(icon!=NULL){
                menuItem->icon=icon->bmap_data;
            }
    
            menu->addItem(menu,menuItem);
        }

        item=item->next;
    }

    return true;
}

void shellMenu_handleEvent(int event){
    SHELLMENU_ITEM item;

    shellMenu_menuList->handleEvent(shellMenu_menuList,event);

    // handle event based items
    item=shellMenu_firstItem;
    while(item!=NULL){
        if(item->event==event){

            if(item->global || item->parent==shellMenu_menuList->focusedWidget->data){
                // execute corresponding command
                shell_execute(item->command,item->param);

            }
        }

        item=item->next;
    }
}

void shellMenu_printItems(){
    SHELLMENU_ITEM i;

    i=shellMenu_firstItem;
    while(i){
        printk("* name='%s'",i->name);
        if (i->event) printk(" event=%d%s",i->event,(i->global)?" global":"");
        if (i->parent) printk(" parent='%s'",i->parent->name);
        if (i->param) printk(" param='%s'",i->param);
        if (i->command) printk(" command='%s'",i->command);
        if (i->icon) printk(" icon='%s'",i->icon);

        printk("\n");

        i=i->next;
    }
}

bool shellMenu_init(){
    
    rootListView=false;

    // load icons
    shellMenu_folderIcon = icon_load(SHELL_FOLDER_ICON);
    shellMenu_backIcon = icon_load(SHELL_BACK_ICON);
    shellMenu_appIcon = icon_load(SHELL_APP_ICON);

    // parse menu file
    if(!shellMenu_parse(SHELL_MENU_FILE)){
        printk("[shell menu] error reading menu file, stopping\n");
        return false;
    }

    // build menu
    if(!shellMenu_build()){
        return false;
    };

    return true;
}

void shellMenu_close(){
    SHELLMENU_ITEM item;
    SHELLMENU_ITEM prev;

    // free widgets
    if(shellMenu_menuList!=NULL){
        shellMenu_menuList->destroy(shellMenu_menuList);
    }

    // free item list
    item=shellMenu_firstItem;
    while(item!=NULL){
        // free strings
        if (item->name!=NULL) free(item->name);
        if (item->param!=NULL) free(item->param);
        if (item->command!=NULL) free(item->command);
        if (item->icon!=NULL) free(item->icon);

        // get next item pointer before freeing item
        prev=item;

        item=item->next;

        free(prev);
    }
}
