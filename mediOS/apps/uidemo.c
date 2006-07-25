/*
* uidemo.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gui/widget.h>
#include <gui/checkbox.h>
#include <gui/trackbar.h>
#include <gui/chooser.h>
#include <gui/button.h>
#include <gui/textmenu.h>
#include <gui/widgetmenu.h>
#include <gui/iconmenu.h>
#include <gui/icons.h>
#include <gui/widgetlist.h>
#include <gui/label.h>

#include <gui/cfg_file.h>

char * chooser_items[]={"Choice 1","Choice 2","Choice 3","Choice 4","Choice 5","Pouet!"};

int eventHandler;

WIDGETLIST widgetList;

WIDGETLIST menuList;

TEXTMENU mainMenu;

TEXTMENU textMenu;
WIDGETMENU widgetMenu;
ICONMENU iconMenu;

bool useMenuHandler=false;
bool wantQuit=false;

void mainMenuHandleEvents();

void menu_onClick(MENU m, MENU_ITEM mi){
    char s[256];

    sprintf(s,"item #%d '%s' clicked!",m->indexOf(m,mi),mi->caption);

    gfx_fillRect(COLOR_GREY,0,0,220,10);
    gfx_putS(COLOR_RED,COLOR_WHITE,0,0,s);
}

void exampleButton_onClick(BUTTON b){
    gfx_putS(COLOR_RED,COLOR_WHITE,120,120,"Button click!");
}

void menuDemoButton_onClick(BUTTON b){
    useMenuHandler=true;
}

void widgetMenu_onClick(WIDGETMENU m, WIDGETMENU_ITEM mi){

    gfx_fillRect(COLOR_GREY,0,0,220,10);

    if(!strcmp(mi->caption,"Load config")){

        if (!m->cfgLoad(m,"/uidemo.cfg")){

            gfx_putS(COLOR_RED,COLOR_WHITE,0,0,"failed loading /uidemo.cfg");

            return;
        }

        gfx_putS(COLOR_RED,COLOR_WHITE,0,0,"/uidemo.cfg loaded!");

        m->handleEvent(m,EVT_REDRAW);
    }

    if(!strcmp(mi->caption,"Save config")){

        if(!m->cfgSave(m,"/uidemo.cfg")){

            gfx_putS(COLOR_RED,COLOR_WHITE,0,0,"failed saving /uidemo.cfg");

            return;
        }

        gfx_putS(COLOR_RED,COLOR_WHITE,0,0,"/uidemo.cfg saved!");
    }
}

void buildMenuList(){
    menuList=widgetList_create();
    menuList->ownWidgets=true; // the list will handle items destroy
}

void buildMainMenu(){
    TEXTMENU_ITEM mi;

    mainMenu=textMenu_create();
    mainMenu->setRect(mainMenu,0,20,220,156);
    mainMenu->ownItems=true; // the menu will handle items destroy

    mi=textMenuItem_create();
    mi->caption="Text menu demo";
    mainMenu->addItem(mainMenu,mi);

    mi=textMenuItem_create();
    mi->caption="Icon menu demo";
    mainMenu->addItem(mainMenu,mi);

    mi=textMenuItem_create();
    mi->caption="Widget menu demo";
    mainMenu->addItem(mainMenu,mi);

    // add to menu list
    menuList->addWidget(menuList,mainMenu);
    mainMenu->menuList=menuList;
    
    // set as active menu
    menuList->setFocusedWidget(menuList,mainMenu);
}

void buildTextMenu(){
    TEXTMENU_ITEM mi;
    int i;
    char * s;

    textMenu=textMenu_create();
    textMenu->setRect(textMenu,0,20,220,156);
    textMenu->ownItems=true; // the menu will handle items destroy
    textMenu->onClick=(MENU_CLICKEVENT)menu_onClick;

    mi=textMenuItem_create();
    mi->caption="<- back";
    mi->foreColor=COLOR_RED;
    mi->subMenu=(MENU)mainMenu; // link to main menu
    textMenu->addItem(textMenu,mi);

    mi=textMenuItem_create();
    mi->caption="Left aligned item (default)";
    textMenu->addItem(textMenu,mi);

    mi=textMenuItem_create();
    mi->caption="Centered item";
    mi->alignment=TMA_CENTER;
    textMenu->addItem(textMenu,mi);

    mi=textMenuItem_create();
    mi->caption="Right aligned item";
    mi->alignment=TMA_RIGHT;
    textMenu->addItem(textMenu,mi);

    for(i=1;i<51;++i){
        s=malloc(20);
        sprintf(s,"Item %d",i);

        mi=textMenuItem_create();
        mi->caption=s;
        textMenu->addItem(textMenu,mi);
    }

    // add to menu list
    menuList->addWidget(menuList,textMenu);
    textMenu->menuList=menuList;

    // set parent menu
    textMenu->parentMenu=(MENU)mainMenu;

    // link mainMenu item to this menu
    mainMenu->items[0]->subMenu=(MENU)textMenu;
}

void buildWidgetMenu(){
    WIDGETMENU_ITEM mi;
    WIDGETMENU_CHECKBOX mic;
    WIDGETMENU_TRACKBAR mit;
    WIDGETMENU_CHOOSER mih;
    int i;
    char * s;

    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,0,20,220,156);
    widgetMenu->ownItems=true; // the menu will handle items destroy
    widgetMenu->onClick=(MENU_CLICKEVENT)widgetMenu_onClick;


    mi=widgetMenuItem_create();
    mi->caption="Load config";
    mi->widgetWidth=0;
    widgetMenu->addItem(widgetMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Save config";
    mi->widgetWidth=0;
    widgetMenu->addItem(widgetMenu,mi);

    mic=widgetMenuCheckbox_create();
    mic->caption="Checkbox example";
    mic->cfgName="checkbox_example";
    mic->cfgStored=true;
    widgetMenu->addItem(widgetMenu,mic);

    mit=widgetMenuTrackbar_create();
    mit->caption="Trackbar example";
    mit->cfgName="trackbar_example";
    mit->cfgStored=true;
    widgetMenu->addItem(widgetMenu,mit);

    mih=widgetMenuChooser_create();
    mih->caption="Chooser example";
    mih->cfgName="chooser_example";
    mih->cfgStored=true;
    mih->chooser->items=chooser_items;
    mih->chooser->itemCount=6;
    mih->chooser->index=0;
    widgetMenu->addItem(widgetMenu,mih);

    mi=widgetMenuItem_create();
    mi->caption="Simple widgetMenuItem example";
    mi->widgetWidth=0;
    widgetMenu->addItem(widgetMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="(can be used for onClick evts)";
    mi->widgetWidth=0;
    widgetMenu->addItem(widgetMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="This item can't be selected";
    mi->canFocus=false;
    mi->widgetWidth=0;
    widgetMenu->addItem(widgetMenu,mi);

    for(i=1;i<101;++i){
        s=malloc(20);
        sprintf(s,"Item %d",i);

        mit=widgetMenuTrackbar_create();
        mit->caption=s;
        mit->trackbar->value=i;
        widgetMenu->addItem(widgetMenu,mit);
    }

    // add to menu list
    menuList->addWidget(menuList,widgetMenu);
    widgetMenu->menuList=menuList;

    // set parent menu
    widgetMenu->parentMenu=(MENU)mainMenu;

    // link mainMenu item to this menu
    mainMenu->items[2]->subMenu=(MENU)widgetMenu;
}

void buildIconMenu(){
    ICONMENU_ITEM mi;
    int i;
    char * s;
    BITMAP icons[]={icon_load("clock_icon.ico")->bmap_data,
                    icon_load("calc.ico")->bmap_data,
                    icon_load("mines.ico")->bmap_data,
                    icon_load("snake.ico")->bmap_data,
                    icon_load("othello.ico")->bmap_data,
                    icon_load("browser_icon.ico")->bmap_data};

    iconMenu=iconMenu_create();
    iconMenu->setRect(iconMenu,0,20,220,156);
    iconMenu->itemWidth=52;
    iconMenu->itemHeight=64;
    iconMenu->ownItems=true; // the menu will handle items destroy
    iconMenu->onClick=(MENU_CLICKEVENT)menu_onClick;

    mi=iconMenuItem_create();
    mi->caption="Example";
    mi->icon=icon_load("plugin_icon.ico")->bmap_data;
    iconMenu->addItem(iconMenu,mi);

    for(i=1;i<101;++i){
        s=malloc(20);
        sprintf(s,"Item %d",i);

        mi=iconMenuItem_create();
        mi->caption=s;
        mi->icon=icons[i%6];

        iconMenu->addItem(iconMenu,mi);
    }

    // add to menu list
    menuList->addWidget(menuList,iconMenu);
    iconMenu->menuList=menuList;

    // set parent menu
    iconMenu->parentMenu=(MENU)mainMenu;

    // link mainMenu item to this menu
    mainMenu->items[1]->subMenu=(MENU)iconMenu;
}

void buildWidgetList(){
    CHECKBOX c;
    TRACKBAR t;
    CHOOSER h;
    BUTTON b;
    LABEL l;

    widgetList=widgetList_create();
    widgetList->setRect(widgetList,0,20,220,156);
    widgetList->ownWidgets=true; // the list will handle items destroy

    l=label_create();
    l->setRect(l,0,25,220,13);
    l->font=STD8X13;
    l->alignment=LA_CENTER;
    l->foreColor=COLOR_RED;
    l->caption="widgetList demo";
    widgetList->addWidget(widgetList,l);

    l=label_create();
    l->setRect(l,0,40,220,6);
    l->font=STD4X6;
    l->foreColor=COLOR_GREEN;
    l->caption="handles widgets focus, F1/F2: change focus, F3: quit";
    widgetList->addWidget(widgetList,l);

    b=button_create();
    b->setRect(b,10,155,100,15);
    b->focusPosition=5;
    b->onClick=(BUTTON_CLICKEVENT)menuDemoButton_onClick;
    b->caption="Go to menu demo";
    widgetList->addWidget(widgetList,b);

    c=checkbox_create();
    c->setRect(c,10,55,150,15);
    c->focusPosition=1;
    c->caption="Example checkbox";
    widgetList->addWidget(widgetList,c);

    t=trackbar_create();
    t->setRect(t,10,75,150,15);
    t->focusPosition=2;
    t->numTicks=7;
    t->minimum=-10;
    widgetList->addWidget(widgetList,t);

    h=chooser_create();
    h->setRect(h,10,95,150,15);
    h->focusPosition=3;
    h->items=chooser_items;
    h->itemCount=6;
    h->index=0;
    widgetList->addWidget(widgetList,h);

    b=button_create();
    b->setRect(b,10,115,100,15);
    b->focusPosition=4;
    b->onClick=(BUTTON_CLICKEVENT)exampleButton_onClick;
    b->caption="Example button";
    widgetList->addWidget(widgetList,b);

    // focus first widget
    widgetList->changeFocus(widgetList,WLD_NEXT);
}

void widgetListHandleEvents(){
    int event;

    // initial paint
    widgetList->paint(widgetList);

    do{
        event=evt_getStatus(eventHandler);
        if (!event) continue; // no new events

        if (event==BTN_F1){
            widgetList->changeFocus(widgetList,WLD_PREVIOUS); // focus previous widget
            continue;
        }

        if (event==BTN_F2){
            widgetList->changeFocus(widgetList,WLD_NEXT); // focus next widget
            continue;
        }

        widgetList->handleEvent(widgetList,event);

    }while(event!=BTN_F3 && !useMenuHandler);

    wantQuit=!useMenuHandler;
}

void mainMenuHandleEvents(){
    int event;

    // intial paint
    menuList->focusedWidget->paint(menuList->focusedWidget);

    do{
        event=evt_getStatus(eventHandler);
        if (!event) continue; // no new events

        menuList->handleEvent(menuList,event);

    }while(event!=BTN_F3 && useMenuHandler);

    useMenuHandler=false;
}


int app_main(void)
{
    gfx_openGraphics();
    gfx_clearScreen(COLOR_GREY);

    eventHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);

    // build menus & all

    buildWidgetList();

    buildMenuList();

    buildMainMenu();

    buildTextMenu();
    buildWidgetMenu();
    buildIconMenu();

    // handle events

    while(!wantQuit){
        if(useMenuHandler){
            mainMenuHandleEvents();
        }else{
            widgetListHandleEvents();
        }
    }

    gfx_clearScreen(COLOR_GREY);

    evt_freeHandler(eventHandler);

    menuList->destroy(menuList);
    widgetList->destroy(widgetList);

    return 0;
}
