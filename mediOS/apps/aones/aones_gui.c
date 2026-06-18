#include "medios.h"
#include "aones.h"
#include "aones_gui.h"
#include "unes.h"
#include "intro_gmini4.h"

int gui_eventHandler=0;
bool gui_wantExit=false;
bool gui_browserNeedInit=true;

struct browser_data * browser;

WIDGETLIST menuList;
WIDGETMENU standardMenu;
WIDGETMENU advancedMenu;

char * autoFire_items[]={"Off","A Button","B Button","Both A and B"};
char * f3Use_items[]={"A+B","Sticky A","Sticky B"};
char * tvOut_items[]={"Off","PAL","Stretched PAL","NTSC"};
char * sndFilter_items[]={"Off","Lowpass","Weighted lp."};

void menu_onClick(MENU m, WIDGETMENU_ITEM mi){
    int slot=standardMenu->getTrackbar(standardMenu,standardMenu->indexFromCaption(standardMenu,"Slot"))->value;
    char * s=malloc(100);

    if(!strcmp(mi->caption,"Load state")){
        sprintf(s,"Loading state, slot %d...",slot);
        gui_showText(s);

        LoadStateSnss(slot-1);
        gui_wantExit=true;
    }

    if(!strcmp(mi->caption,"Save state")){
        sprintf(s,"Saving state, slot %d...",slot);
        gui_showText(s);

        SaveStateSnss(slot-1);
        gui_wantExit=true;
    }

    if(!strcmp(mi->caption,"Reset NES")){
        Reset_NES();
        gui_wantExit=true;
    }

    free(s);
}

void gui_init(){
    int sw,sh;
    WIDGETMENU_ITEM mi;
    WIDGETMENU_CHECKBOX mic;
    WIDGETMENU_TRACKBAR mit;
    WIDGETMENU_CHOOSER mih;

    getResolution(&sw,&sh);

    gui_eventHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);

    // browser init
    icon_init();
    iniBrowser();
    browser=browser_NewBrowse();
    browser->mode=MODE_STRING;
    gui_browserNeedInit=true;

    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    // standardMenu
    standardMenu=widgetMenu_create();
    standardMenu->setRect(standardMenu,0,0,sw,sh);
    standardMenu->ownItems=true; // the menu will handle items destroy
    standardMenu->onClick=(MENU_CLICKEVENT)menu_onClick;
    standardMenu->menuList=menuList;
    menuList->addWidget(menuList,standardMenu);

    mi=widgetMenuItem_create();
    mi->caption="General settings:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mit=widgetMenuTrackbar_create();
    mit->caption="Volume";
    mit->cfgStored=true;
    mit->cfgName="volume";
    mit->trackbar->maximum=100;
    mit->trackbar->value=75;
    standardMenu->addItem(standardMenu,mit);

    mit=widgetMenuTrackbar_create();
    mit->caption="Backlight strength";
    mit->cfgStored=true;
    mit->cfgName="backlight";
    mit->trackbar->maximum=100;
    mit->trackbar->value=100;
    standardMenu->addItem(standardMenu,mit);

    mih=widgetMenuChooser_create();
    mih->caption="TV out";
    mih->cfgName="tv_out";
    mih->cfgStored=false;
    mih->chooser->items=tvOut_items;
    mih->chooser->itemCount=4;
    mih->chooser->index=0;
    standardMenu->addItem(standardMenu,mih);

    mi=widgetMenuItem_create();
    mi->caption="Buttons:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mic=widgetMenuCheckbox_create();
    mic->caption="Swap buttons";
    mic->cfgStored=true;
    mic->cfgName="buttons_swap";
    mic->checkbox->caption="Swap";
    standardMenu->addItem(standardMenu,mic);

    mih=widgetMenuChooser_create();
    mih->caption="Autofire";
    mih->cfgName="autofire";
    mih->cfgStored=true;
    mih->chooser->items=autoFire_items;
    mih->chooser->itemCount=4;
    mih->chooser->index=0;
    standardMenu->addItem(standardMenu,mih);

    mih=widgetMenuChooser_create();
    mih->caption="F3 button use";
    mih->cfgName="f3_button";
    mih->cfgStored=true;
    mih->chooser->items=f3Use_items;
    mih->chooser->itemCount=3;
    mih->chooser->index=0;
    standardMenu->addItem(standardMenu,mih);

    mi=widgetMenuItem_create();
    mi->caption="State saving:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mit=widgetMenuTrackbar_create();
    mit->caption="Slot";
    mit->trackbar->minimum=1;
    mit->trackbar->maximum=10;
    mit->trackbar->numTicks=8;
    mit->trackbar->value=1;
    standardMenu->addItem(standardMenu,mit);

    mi=widgetMenuItem_create();
    mi->caption="Load state";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Save state";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);
/*
    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    standardMenu->addItem(standardMenu,mi);
*/
    mi=widgetMenuItem_create();
    mi->caption="Advanced menu";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    // advancedMenu
    advancedMenu=widgetMenu_create();
    advancedMenu->setRect(advancedMenu,0,0,sw,sh);
    advancedMenu->ownItems=true; // the menu will handle items destroy
    advancedMenu->onClick=(MENU_CLICKEVENT)menu_onClick;
    advancedMenu->menuList=menuList;
    menuList->addWidget(menuList,advancedMenu);

    mi=widgetMenuItem_create();
    mi->caption="Emulation options:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    advancedMenu->addItem(advancedMenu,mi);

    mit=widgetMenuTrackbar_create();
    mit->caption="Frameskip";
    mit->cfgStored=true;
    mit->cfgName="frameskip";
    mit->trackbar->maximum=9;
    mit->trackbar->numTicks=8;
    mit->trackbar->value=1;
    advancedMenu->addItem(advancedMenu,mit);

    mit=widgetMenuTrackbar_create();
    mit->caption="CPU cycles/line";
    mit->cfgStored=true;
    mit->cfgName="cpu_cycles";
    mit->trackbar->maximum=255;
    mit->trackbar->value=113;
    advancedMenu->addItem(advancedMenu,mit);

    mih=widgetMenuChooser_create();
    mih->caption="Sound filter";
    mih->cfgName="sound_filter";
    mih->cfgStored=true;
    mih->chooser->items=sndFilter_items;
    mih->chooser->itemCount=3;
    mih->chooser->index=0;
    standardMenu->addItem(advancedMenu,mih);

    mi=widgetMenuItem_create();
    mi->caption="Overclocking:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    advancedMenu->addItem(advancedMenu,mi);

    mic=widgetMenuCheckbox_create();
    mic->caption="Enable OC";
    mic->cfgStored=true;
    mic->cfgName="overclocking";
    mic->checkbox->caption="Enabled";
    advancedMenu->addItem(advancedMenu,mic);

    mit=widgetMenuTrackbar_create();
    mit->caption="CPU frequency(Mhz)";
    mit->cfgStored=true;
    mit->cfgName="cpu_frequency";
    mit->trackbar->minimum=50;
    mit->trackbar->maximum=200;
    mit->trackbar->value=clkc_getClockFrequency(CLK_ARM)/1000000;
    advancedMenu->addItem(advancedMenu,mit);

    mit=widgetMenuTrackbar_create();
    mit->caption="DSP frequency(Mhz)";
    mit->cfgStored=true;
    mit->cfgName="dsp_frequency";
    mit->trackbar->minimum=50;
    mit->trackbar->maximum=200;
    mit->trackbar->value=clkc_getClockFrequency(CLK_DSP)/1000000;
    advancedMenu->addItem(advancedMenu,mit);

    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    advancedMenu->addItem(advancedMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Reset NES";
    mi->widgetWidth=0;
    advancedMenu->addItem(advancedMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="";
    mi->canFocus=false;
    advancedMenu->addItem(advancedMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Standard menu";
    mi->widgetWidth=0;
    advancedMenu->addItem(advancedMenu,mi);

    standardMenu->items[standardMenu->indexFromCaption(standardMenu,"Advanced menu")]->subMenu=(MENU)advancedMenu;
    advancedMenu->items[advancedMenu->indexFromCaption(advancedMenu,"Standard menu")]->subMenu=(MENU)standardMenu;

    standardMenu->cfgLoad(standardMenu,CFG_FILE_PATH);
    advancedMenu->cfgLoad(advancedMenu,CFG_FILE_PATH);
}


void gui_close(){
    standardMenu->cfgSave(standardMenu,CFG_FILE_PATH);
    advancedMenu->cfgSave(advancedMenu,CFG_FILE_PATH);

    evt_freeHandler(gui_eventHandler);
    menuList->destroy(menuList);
    
    browser_disposeBrowse(browser);
}

void gui_execute(){
    int event;

    gui_wantExit=false;

    gui_showGuiPlane();

    // discard waiting events
    evt_purgeHandler(gui_eventHandler);

    // intial paint
    if(menuList->focusedWidget==NULL) menuList->setFocusedWidget(menuList,standardMenu);
    menuList->focusedWidget->paint(menuList->focusedWidget);

    do{
        event=evt_getStatus(gui_eventHandler);
        if (!event) continue; // no new events

        menuList->handleEvent(menuList,event);
    }while(event!=BTN_OFF && !gui_wantExit);

    while(btn_readState()); // make sure no button is left pressed

    gui_showEmuPlane();
}

void gui_applySettings(){
    int vol,bl,cc,sf;

    vol=standardMenu->getTrackbar(standardMenu,standardMenu->indexFromCaption(standardMenu,"Volume"))->value;
    bl=standardMenu->getTrackbar(standardMenu,standardMenu->indexFromCaption(standardMenu,"Backlight strength"))->value;
    tvOut=standardMenu->getChooser(standardMenu,standardMenu->indexFromCaption(standardMenu,"TV out"))->index;
    buttonsSwap=standardMenu->getCheckbox(standardMenu,standardMenu->indexFromCaption(standardMenu,"Swap buttons"))->checked;
    autoFire=standardMenu->getChooser(standardMenu,standardMenu->indexFromCaption(standardMenu,"Autofire"))->index;
    f3Use=standardMenu->getChooser(standardMenu,standardMenu->indexFromCaption(standardMenu,"F3 button use"))->index;
    frameSkip=advancedMenu->getTrackbar(advancedMenu,advancedMenu->indexFromCaption(advancedMenu,"Frameskip"))->value;
    cc=advancedMenu->getTrackbar(advancedMenu,advancedMenu->indexFromCaption(advancedMenu,"CPU cycles/line"))->value;
    sf=standardMenu->getChooser(advancedMenu,advancedMenu->indexFromCaption(advancedMenu,"Sound filter"))->index;
    overclocking=advancedMenu->getCheckbox(advancedMenu,advancedMenu->indexFromCaption(advancedMenu,"Enable OC"))->checked;
    armFrequency=advancedMenu->getTrackbar(advancedMenu,advancedMenu->indexFromCaption(advancedMenu,"CPU frequency(Mhz)"))->value;
    dspFrequency=advancedMenu->getTrackbar(advancedMenu,advancedMenu->indexFromCaption(advancedMenu,"DSP frequency(Mhz)"))->value;

    display_tvOutSet();
#ifdef SOUND_USE_AIC23
    aic23_setOutputVolume(vol+27,AIC23_CHANNEL_BOTH);
#endif

#ifdef SOUND_USE_DSP
    dspCom->sndFilter=sf;
#endif

    clk_overclock(true);

    lcd_setBrightness(bl);

    if(!Vnes.var.CustomCpuCycle) Vnes.var.cpucycle=cc;

}

bool gui_browse(){

    gfx_clearScreen(COLOR_WHITE);

    if (gui_browserNeedInit){
        viewNewDir(browser,NULL);
        gui_browserNeedInit=false;
    }

    redrawBrowser(browser);

    if(browserEvt(browser)==MED_OK){
        sprintf(CurrentROMFile,"%s/%s",browser->path,browser->list[browser->pos+browser->nselect].name);
        return true;
    }else{
        return false;
    }
}

void gui_welcomeScreen(){

#if 0
    int y=0;

    gfx_planeHide(VID1);
    gfx_setPlane(BMAP1);
    gfx_planeShow(BMAP1);

    gfx_clearScreen(COLOR_WHITE);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y,       "aoNES v0.3");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "==========");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "Port of LittleJohnGP by yoyo.");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Ingame keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:  Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Square: A button");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Cross:  B button");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On:     Start button");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F1:     Open menu");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:     Select button");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:     Customizable button");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off:    Go to browser");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,166,     "Press a key to continue...");
#else
    unsigned long *ip,*op;
    int i;

    gfx_planeHide(VID1);
    gfx_planeHide(BMAP1);
    gfx_setPlane(VID2);

    ip=intro_gmini4_data;
    op=gfx_planeGetBufferOffset(VID2);
    for(i=0;i<intro_gmini4_X*intro_gmini4_Y;++i){
        *(op++)=(*ip)|((*ip>>8)<<24);
        ip++;
    }

    gfx_planeShow(VID2);
#endif

    while(btn_readState());
    while(!btn_readState());

    gfx_planeHide(VID2);
}

bool gui_confirmQuit(){
    int bt;

    gui_showText("Really quit? (OFF=yes, any other=no)");

    while(btn_readState());
    while(!(bt=btn_readState()));
    while(btn_readState());

    gui_showEmuPlane();

    return bt==NES_BTN_HALT;
}

void gui_showGuiPlane(){
    gfx_planeHide(VID1);
    gfx_setPlane(BMAP1);
    gfx_clearScreen(COLOR_WHITE);
    gfx_planeShow(BMAP1);
}

void gui_showEmuPlane(){
    gfx_planeHide(BMAP1);
    gfx_setPlane(VID1);
    gfx_clearScreen(COLOR32_BLACK);
    gfx_planeShow(VID1);
}

void gui_showText(char * text){
    gui_showGuiPlane();
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,0,text);
}
