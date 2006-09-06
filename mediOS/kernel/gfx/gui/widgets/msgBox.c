/*
* kernel/gfx/gui/widget/msgbox.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/msgBox.h>

#include <kernel/graphics.h>
#include <kernel/evt.h>
#include <kernel/lcd.h>
#include <kernel/icons.h>
#include <kernel/widget.h>

#include <sys_def/colordef.h>
#include <sys_def/font.h>
#include <sys_def/string.h>


BITMAP * MsgBox_ExclamationBitmap;
BITMAP * MsgBox_QuestionBitmap;
BITMAP * MsgBox_WarningBitmap;
BITMAP * MsgBox_InformationBitmap;
BITMAP * MsgBox_ErrorBitmap;

#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 15
#define BUTTON_DISTANCE 10


int cntButtons  = 0; /* Buttons in MSgBox */
int g_type = MSGBOX_TYPE_OK;

int fontVal;

/* msg_box event handler */
int msgBox_evtHandler(int evt_hanlder)
{
    int stopBoxLoop = 0;
    int res=MSGBOX_NOTHING;
    char evt=0;
    while(!stopBoxLoop)
    {
        evt = evt_getStatus(evt_hanlder);
        if(evt==NO_EVENT)
            continue;

        switch(evt)
        {
            case WIDGET_ACTION_BTN:
            case BTN_F1:
                if(cntButtons>0)
                {
                    if(g_type == MSGBOX_TYPE_OK)
                        res = MSGBOX_OK;
                    else if(g_type == MSGBOX_TYPE_OKCANCEL)
                        res =  MSGBOX_OK;
                    else if(g_type == MSGBOX_TYPE_YESNO)
                        res =  MSGBOX_YES;
                    else if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_YES;
                    stopBoxLoop=1;
                }
                break;

            case WIDGET_BACK_BTN:
            case BTN_F2:
                if(cntButtons>1)
                {
                    if(g_type == MSGBOX_TYPE_OKCANCEL)
                        res =  MSGBOX_CANCEL;
                    else if(g_type == MSGBOX_TYPE_YESNO)
                        res =  MSGBOX_NO;
                    else if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_NO;
                    stopBoxLoop=1;
                }
                break;

            case BTN_F3:
                if(cntButtons>2)
                {
                    if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_CANCEL;
                    stopBoxLoop=1;
                }
                break;
/*
            case BTN_OFF:
                res = MSGBOX_NOTHING;
                stopBoxLoop=1;
                break;
*/
        }
    }

    return res;
}

void msgBox_init(void)
{
    MsgBox_ExclamationBitmap=&icon_get("MsgBoxExclamationBitmap")->bmap_data;
    MsgBox_QuestionBitmap=&icon_get("MsgBoxQuestionBitmap")->bmap_data;
    MsgBox_WarningBitmap=&icon_get("MsgBoxWarningBitmap")->bmap_data;
    MsgBox_InformationBitmap=&icon_get("MsgBoxInformationBitmap")->bmap_data;
    MsgBox_ErrorBitmap=&icon_get("MsgBoxErrorBitmap")->bmap_data;
}

#define POS_X(X) ((X)+posX)
#define POS_Y(Y) ((Y)+posY)

/* draw the msg box */
void msgBox_draw(unsigned char* caption, unsigned char* msg, int type, int icon,int useOwnPlane)
{
    int w1 = 0;
    int h1 = 0;
    int w2 = 0;
    int h2 = 0;
    int w_buttonText = 0;
    int h_buttonText = 0;
    int width = 0;
    int posX = 0;
    int posY = 0;
    int value = 0;
    int buttonPos = 0;
    int buttonOffset = 0;
    int buttonTextOffsetX=0;
    int buttonTextOffsetY=0;
    int i = 0;
    char strButtonText[10];
    int x;
    int prevFont;

    //save font
    prevFont=gfx_fontGet();

    // calculate width
    gfx_fontSet(STD7X13);
    gfx_getStringSize(caption, &w1, &h1);
    gfx_fontSet(STD5X8);
    gfx_getStringSize(msg, &w2, &h2);
    
    // calculate width of box for text
    if(w1>w2)
        width = w1 + 20 + 20;
    else
        width = w2 + 20 + 20;

    // calculate width of box for buttons
    switch(type)
    {
        case MSGBOX_TYPE_OK:
            cntButtons = 1; //only one button
            if(width < MSGBOX_1BUTTON_MIN_WIDTH) width = MSGBOX_1BUTTON_MIN_WIDTH;
            break;
        case MSGBOX_TYPE_YESNOCANCEL:
            cntButtons = 3; //three buttons
            if(width < MSGBOX_3BUTTON_MIN_WIDTH) width = MSGBOX_3BUTTON_MIN_WIDTH;
            break;
        case MSGBOX_TYPE_OKCANCEL:
        case MSGBOX_TYPE_YESNO:
            cntButtons = 2; //two buttons
            if(width < MSGBOX_2BUTTON_MIN_WIDTH) width = MSGBOX_2BUTTON_MIN_WIDTH;
            break;
        case MSGBOX_TYPE_INFO:
            cntButtons = 0;
            break;
    }

    // the width must be a multiple of 32 !
    value = width/8;
    width = value*8;

    buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
    buttonOffset = (buttonPos - BUTTON_WIDTH)/2;

    if(useOwnPlane);
        gfx_planeSetSize(BMAP2,width,MSGBOX_HEIGHT,8);

    // center box
    if(useOwnPlane)
        posX = SCREEN_REAL_WIDTH-width;
    else
        posX = (SCREEN_REAL_WIDTH-width)/2;
    posY = (SCREEN_HEIGHT/2)-(MSGBOX_HEIGHT/2);
    if(useOwnPlane)
        gfx_planeSetPos(BMAP2,SCREEN_ORIGIN_X+posX,SCREEN_ORIGIN_Y+posY);

    // show box
    if(useOwnPlane)
        gfx_setPlane(BMAP2);

    // fill background
    gfx_drawRect(COLOR_BLACK, POS_X(0), POS_Y(0), width, MSGBOX_HEIGHT);
    gfx_fillRect(COLOR_WHITE, POS_X(1), POS_Y(1), width-2, MSGBOX_HEIGHT-2);
    // write caption
    gfx_fontSet(STD7X13);
    
    x = (width/2)-(w1/2);
    
    gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(x), POS_Y(2), caption);
    gfx_drawLine(COLOR_BLACK,POS_X(0), POS_Y(h1+2+2), POS_X(width), POS_Y(h1+2+2));
    // draw bitmap and buttons
    if(icon == MSGBOX_ICON_EXCLAMATION)
        gfx_drawBitmap (MsgBox_ExclamationBitmap, POS_X(2),POS_Y(h1+2+2+5));
    else if(icon == MSGBOX_ICON_WARNING)
        gfx_drawBitmap (MsgBox_WarningBitmap, POS_X(2),POS_Y(h1+2+2+5));
    else if(icon == MSGBOX_ICON_INFORMATION)
        gfx_drawBitmap (MsgBox_InformationBitmap, POS_X(2),POS_Y(h1+2+2+5));
    else if(icon == MSGBOX_ICON_QUESTION)
        gfx_drawBitmap (MsgBox_QuestionBitmap, POS_X(2),POS_Y(h1+2+2+5));
    else if(icon == MSGBOX_ICON_ERROR)
        gfx_drawBitmap (MsgBox_ErrorBitmap, POS_X(2),POS_Y(h1+2+2+5));
    // print message

    gfx_fontSet(STD5X8);
    if(icon == MSGBOX_ICON_NO_ICON) // if no icon, print the text on the left side
        gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(2), POS_Y(h1+2+2+10), msg);
    else
        gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(25), POS_Y(h1+2+2+10), msg); // xPos with offset for icon

    // print buttons
    switch(type)
    {
        case MSGBOX_TYPE_OK:
            cntButtons = 1; //only one button
    
            buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
            buttonOffset = (buttonPos - BUTTON_WIDTH)/2;
    
            for(i = 0; i < cntButtons; i++)
            {
                gfx_drawRect(COLOR_BLACK, POS_X(BUTTON_DISTANCE+i*buttonPos+buttonOffset), POS_Y(MSGBOX_HEIGHT-20),
                    BUTTON_WIDTH, BUTTON_HEIGHT);
    
                gfx_getStringSize("Ok(F1)", &w_buttonText, &h_buttonText);
                buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
                buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
                gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX),
                    POS_Y(MSGBOX_HEIGHT-20+buttonTextOffsetY) , "Ok(F1)");
            }
            break;
        case MSGBOX_TYPE_YESNOCANCEL:
            cntButtons = 3; //three buttons
    
            buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
            buttonOffset = (buttonPos - BUTTON_WIDTH)/2;
    
            for(i = 0; i < cntButtons; i++)
            {
                gfx_drawRect(COLOR_BLACK, POS_X(BUTTON_DISTANCE+i*buttonPos+buttonOffset), 
                    POS_Y(MSGBOX_HEIGHT-20), BUTTON_WIDTH, BUTTON_HEIGHT);
    
                if(i == 0)
                    strcpy(strButtonText,"Yes(F1)");
                else if (i == 1)
                    strcpy(strButtonText,"No(F2)");
                else
                    strcpy(strButtonText,"Cancel(F3)");
    
                gfx_getStringSize(strButtonText, &w_buttonText, &h_buttonText);
                buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
                buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
                gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX),
                    POS_Y(MSGBOX_HEIGHT-20+buttonTextOffsetY) , strButtonText);
            }
            break;
        case MSGBOX_TYPE_OKCANCEL:
        case MSGBOX_TYPE_YESNO:
            cntButtons = 2; //two buttons
    
            buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
            buttonOffset = (buttonPos - BUTTON_WIDTH)/2;
    
            for(i = 0; i < cntButtons; i++)
            {
                gfx_drawRect(COLOR_BLACK, POS_X(BUTTON_DISTANCE+i*buttonPos+buttonOffset), POS_Y(MSGBOX_HEIGHT-20),
                    BUTTON_WIDTH, BUTTON_HEIGHT);
    
                if(i == 0)
                {
                    if(type == MSGBOX_TYPE_YESNO)
                        strcpy(strButtonText,"Yes(F1)");
                    else
                        strcpy(strButtonText,"Ok(F1)");
                }
                else
                {
                    if(type == MSGBOX_TYPE_YESNO)
                        strcpy(strButtonText,"No(F2)");
                    else
                        strcpy(strButtonText,"Cancel(F2)");
                }
    
                gfx_getStringSize(strButtonText, &w_buttonText, &h_buttonText);
                buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
                buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
                gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX),
                    POS_Y(MSGBOX_HEIGHT-20+buttonTextOffsetY) , strButtonText);
            }
            break;
        case MSGBOX_TYPE_INFO:
            /* no button */
            break;
    }

    //restore font
    gfx_fontSet(prevFont);

    if(useOwnPlane)
    {
        gfx_planeShow(BMAP2);
        gfx_setPlane(BMAP1);
    }
}



/* restore the previous state */
void msgBox_erase(void)
{
    gfx_planeHide(BMAP2);
    gfx_setPlane(BMAP1);
    gfx_fontSet(fontVal);
}

/* main function */
int msgBox_show(unsigned char* caption, unsigned char* msg, int type, int icon, int evt_hanlder)
{
    int res=MSGBOX_NOTHING;

    g_type = type;
    fontVal=gfx_fontGet();

    //gli: use default plane for now
    msgBox_draw(caption,msg,type,icon,0);

    //info message boxes are display only
    if(type!=MSGBOX_TYPE_INFO){
        res=msgBox_evtHandler(evt_hanlder);
    }

    //msgBox_erase();

    return res;
}

void msgBox_info(unsigned char* msg)
{
    fontVal=gfx_fontGet();
    msgBox_draw("Info",msg,MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,0);
}
