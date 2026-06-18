#include "doom_gui.h"

#include "medios.h"

#include "doomdef.h"
#include "doomstat.h"
#include "v_video.h"
#include "d_main.h"

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
// AV400 seems to have problems with o/c
#if defined(GMINI402) || defined(GMINI4XX)
    int dspf;

    // mute sound during clock changes, fixes some crashes
#ifdef SOUND_USE_DSP
    while(dspCom->armBusy || dspCom->sndWantBuf) /* wait for ready */;
    dspCom->armBusy=1;
#endif

    if(overclocking){
        dspf=clkc_getClockFrequency(CLK_DSP);

        // dsp freq must be >= arm freq
        if(armFrequency*1000000>dspf){
            clkc_setClockFrequency(CLK_DSP,armFrequency*1000000);
            mdelay(10);
        }

        clkc_setClockFrequency(CLK_ARM,armFrequency*1000000);
    }else{
        // default params
        clkc_setClockParameters(CLK_ARM,15,2,2);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
        clkc_setClockParameters(CLK_DSP,9,1,2);
    }

#ifdef SOUND_USE_DSP
    dspCom->armBusy=0;
#endif

#endif
};

void menu_onClick(MENU m, WIDGETMENU_ITEM mi){
    char * c;
    event_t event;

    if (m->indexOf(m,mi)>m->indexFromCaption(m,"Cheats:")){
        printf("Cheat '%s' %s\n",mi->caption,mi->cfgName);

        c=mi->cfgName;
        while(*c){

            event.type = ev_keydown;
            event.data1 = *c;
            D_PostEvent(&event);

            c++;
        }
        
        gui_wantExit=true;
    }
}

void gui_init(){
    WIDGETMENU_ITEM mi;
    WIDGETMENU_CHECKBOX mic;
    WIDGETMENU_TRACKBAR mit;
    WIDGETMENU_CHOOSER mih;

    gui_eventHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);

    // menu
    menu=widgetMenu_create();
#if defined(GMINI4XX) || defined(GMINI402)
    menu->setRect(menu,0,0,220,176);
#endif
#if defined(AV4XX) || defined(AV3XX) || defined(PMA)
    menu->setRect(menu,0,0,320,200);
#endif
    menu->ownItems=true; // the menu will handle items destroy
    menu->onClick=(MENU_CLICKEVENT)menu_onClick;

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
    mit->trackbar->maximum=100;
    mit->trackbar->value=100;
    menu->addItem(menu,mit);

    mih=widgetMenuChooser_create();
    mih->caption="TV out";
    mih->cfgName="tv_out";
    mih->cfgStored=false;
    mih->chooser->items=tvOut_items;
    mih->chooser->itemCount=3;
    mih->chooser->index=0;
    menu->addItem(menu,mih);
#if defined(AV4XX) || defined(PMA)
    mic=widgetMenuCheckbox_create();
    mic->caption="Int Speacker";
    mic->cfgStored=false;
    mic->checkbox->caption="Enabled";
    menu->addItem(menu,mic);
#endif    
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

    mi=widgetMenuItem_create();
    mi->caption="Cheats:";
    mi->foreColor=GUI_TITLE_COLOR;
    mi->canFocus=false;
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="God mode";
    mi->cfgName="iddqd";
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="No clipping";
    mi->cfgName="idclip";
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Full ammo";
    mi->cfgName="idfa";
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Full ammo & keys";
    mi->cfgName="idkfa";
    mi->widgetWidth=0;
    menu->addItem(menu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Powerups";
    mi->cfgName="idbehold";
    mi->widgetWidth=0;
    menu->addItem(menu,mi);


    printf("Loading settings ...\n");
    menu->cfgLoad(menu,CFG_FILE_PATH);
}


void gui_close(){
    printf("Saving settings ...\n");
    menu->cfgSave(menu,CFG_FILE_PATH);

    evt_freeHandler(gui_eventHandler);
}

void gui_execute(){
    int event;

    // mute sound in gui
#ifdef SOUND_USE_DSP
    while(dspCom->armBusy || dspCom->sndWantBuf) /* wait for ready */;
    dspCom->armBusy=1;
#endif

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

#ifdef SOUND_USE_DSP
    dspCom->armBusy=0;
#endif
}

void gui_applySettings(){
    int bl;

    usegamma=menu->getTrackbar(menu,menu->indexFromCaption(menu,"Gamma correction"))->value;
    bl=menu->getTrackbar(menu,menu->indexFromCaption(menu,"Backlight strength"))->value;
    tvOut=menu->getChooser(menu,menu->indexFromCaption(menu,"TV out"))->index;
    overclocking=menu->getCheckbox(menu,menu->indexFromCaption(menu,"Enable OC"))->checked;
    armFrequency=menu->getTrackbar(menu,menu->indexFromCaption(menu,"CPU frequency(Mhz)"))->value;
    
#if defined(AV4XX) || defined(PMA)
    if(menu->getCheckbox(menu,menu->indexFromCaption(menu,"Int Speacker"))->checked)
    {
        printk("Enable spkr\n");
        SPCKR_ON();
    }
    else
    {
        printk("Disable spkr\n");
        SPCKR_OFF();
    }
#endif

    lcd_setBrightness(bl);

    clk_overclock();
    display_tvOutSet();
}

void gui_welcomeScreen(){
    int y=0;

    gfx_clearScreen(COLOR_WHITE);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y,       "aoDoom v0.3");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "===========");

#if defined(GMINI4XX) || defined(GMINI402)
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:  Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Square: Shoot");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Cross:  Strafe");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On:     Activate");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F1:     Ingame menu");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:     Change weapon");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:     Run");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off:    Doom menu");
#endif

#if defined(AV3XX)
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:    Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Joypress: Run");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F1:       Shoot, Ingame menu (while in doom menu)");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:       Strafe");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:       Change weapon");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On:       Activate");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off:      Doom menu");
#endif

#if defined(AV4XX) || defined(PMA)
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=18,   "Keys:");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  D-Pad:  Move");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  LCD/TV: Shoot");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F1:     Doom menu, Ingame menu (while in doom menu)");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F2:     Strafe");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  F3:     Change weapon");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  On:     Run");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,y+=9,    "  Off:    Activate");
#endif

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
