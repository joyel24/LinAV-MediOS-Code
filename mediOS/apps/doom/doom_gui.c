#include "doom_gui.h"

#include "medios.h"

#include "doomdef.h"
#include "doomstat.h"
#include "v_video.h"

int gui_eventHandler=0;
bool gui_wantExit=false;

bool overclocking=false;
int armFrequency=0;

extern int realscreenwidth;
extern int realscreenheight;

WIDGETMENU menu;

char * tvOut_items[]={"Off","PAL","NTSC"};

extern int tvOut;
extern void display_tvOutSet();

void clk_overclock(){
// no DSC25/AV300 OC since I don't know the default parame
#if defined(DM270) || defined(DM320)
    int dspf;

    if(overclocking){
        dspf=clkc_getClockFrequency(CLK_DSP);

        // dsp freq must be >= arm freq
        if(armFrequency*1000000>dspf){
            clkc_setClockFrequency(CLK_DSP,armFrequency*1000000);
        }

        clkc_setClockFrequency(CLK_ARM,armFrequency*1000000);
    }else{
        // default params
        clkc_setClockParameters(CLK_ARM,15,2,2);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
        clkc_setClockParameters(CLK_DSP,9,1,2);
    }
#endif
};

void gui_init(){
    int sw,sh;
    WIDGETMENU_ITEM mi;
    WIDGETMENU_CHECKBOX mic;
    WIDGETMENU_TRACKBAR mit;
    WIDGETMENU_CHOOSER mih;

    getResolution(&sw,&sh);

    gui_eventHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);

    // menu
    menu=widgetMenu_create();
    menu->setRect(menu,0,0,sw,sh);
    menu->ownItems=true; // the menu will handle items destroy

    mi=widgetMenuItem_create();
    mi->caption="Picture settings:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mit=widgetMenuTrackbar_create();
    mit->caption="Gamma correction";
    mit->cfgStored=true;
    mit->cfgName="gamma";
    mit->trackbar->maximum=4;
    mit->trackbar->value=0;
    menu->addItem(menu,mit);

    mit=widgetMenuTrackbar_create();
    mit->caption="Backlight strength";
    mit->cfgStored=true;
    mit->cfgName="backlight";
    mit->trackbar->maximum=63;
    mit->trackbar->value=63;
    menu->addItem(menu,mit);

    mih=widgetMenuChooser_create();
    mih->caption="TV out";
    mih->cfgName="tv_out";
    mih->cfgStored=false;
    mih->chooser->items=tvOut_items;
    mih->chooser->itemCount=3;
    mih->chooser->index=0;
    menu->addItem(menu,mih);

    mi=widgetMenuItem_create();
    mi->caption="Overclocking:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mic=widgetMenuCheckbox_create();
    mic->caption="Enable OC";
    mic->cfgStored=true;
    mic->cfgName="overclocking";
    mic->checkbox->caption="Enabled";
    menu->addItem(menu,mic);

    mit=widgetMenuTrackbar_create();
    mit->caption="CPU frequency(Mhz)";
    mit->cfgStored=true;
    mit->cfgName="cpu_frequency";
    mit->trackbar->minimum=50;
    mit->trackbar->maximum=200;
    mit->trackbar->value=clkc_getClockFrequency(CLK_ARM)/1000000;
    menu->addItem(menu,mit);

    menu->cfgLoad(menu,CFG_FILE_PATH);
}


void gui_close(){
    menu->cfgSave(menu,CFG_FILE_PATH);

    evt_freeHandler(gui_eventHandler);
}

void gui_execute(){
    int event;

    gui_wantExit=false;

    // discard waiting events
    evt_purgeHandler(gui_eventHandler);

    // intial paint
    gfx_clearScreen(COLOR_WHITE);
    menu->paint(menu);

    do{
        event=evt_getStatus(gui_eventHandler);
        if (!event) continue; // no new events

        menu->handleEvent(menu,event);
    }while(event!=BTN_OFF && !gui_wantExit);

    while(btn_readState()); // make sure no button is left pressed
}

void gui_applySettings(){
    int bl;

    usegamma=menu->getTrackbar(menu,menu->indexFromCaption(menu,"Gamma correction"))->value;
    bl=menu->getTrackbar(menu,menu->indexFromCaption(menu,"Backlight strength"))->value;
    tvOut=menu->getChooser(menu,menu->indexFromCaption(menu,"TV out"))->index;
    overclocking=menu->getCheckbox(menu,menu->indexFromCaption(menu,"Enable OC"))->checked;
    armFrequency=menu->getTrackbar(menu,menu->indexFromCaption(menu,"CPU frequency(Mhz)"))->value;

#ifdef GMINI402
    outw((bl<<10)|0x03ff,CLKC_PWM0_HIGH); // Gmini402
#endif
#ifdef GMINI4XX
    outw((bl<<10)|0x03ff,CLKC_PWM1_HIGH); // Gmini400
#endif

    clk_overclock();
    display_tvOutSet();
}

void gui_welcomeScreen(){
    int y=0;

    gfx_clearScreen(COLOR_WHITE);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y,       "aoDoom v0.2");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "===========");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:  Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Square: Shoot");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Cross:  Strafe");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On:     Activate");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F1:     Ingame menu");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:     Change weapon");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:     Run");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off:    Doom menu");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Choose the WAD file you want to");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "play using the browser.");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,realscreenheight-10,"Press a key to continue...");

    while(btn_readState());
    while(!btn_readState());
};

void gui_PWADScreen(){
    gfx_clearScreen(COLOR_WHITE);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,0,       "You chose a PWAD (custom WAD)!");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,9,       "Now choose the corresponding");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,18,      "IWAD file (eg:doom2.wad)");

    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,realscreenheight-10,"Press a key to continue...");

    while(btn_readState());
    while(!btn_readState());
};
