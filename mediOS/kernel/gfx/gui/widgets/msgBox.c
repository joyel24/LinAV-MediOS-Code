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

#include <graphics.h>
#include <api.h>
#include <evt.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>
#include <sys_def/string.h>
#include <gui/msgBox.h>
#include <gui/icons.h>

BITMAP * MsgBox_ExclamationBitmap;
BITMAP * MsgBox_QuestionBitmap;
BITMAP * MsgBox_WarningBitmap;
BITMAP * MsgBox_InformationBitmap;
BITMAP * MsgBox_ErrorBitmap;

#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 15
#define BUTTON_DISTANCE 10

int res; // updated in msgBoxEvtHandler
// we should add some define in msgBox.h such as RES_OK RES_YES RES_NO RES_CANCEL ...
// maybe a param to msgBox to set the type of box: alert, choice yes/no, input text


int stopBoxLoop = 0; /* global variable used to stop the private evt loop*/
int cntButtons  = 0; /* Buttons in MSgBox */
int g_type = MSGBOX_TYPE_OK;

/* msg_box event handler */
void msgBoxEvtHandler(EVT_PIPE evt_buffer)
{
    char evt=0;
    while(!stopBoxLoop)
    {
        evt = get_evt(evt_buffer);
        if(!evt)
            continue;
        
        switch(evt)
        {
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
                }
                stopBoxLoop=1;
                break;
    
            case BTN_F2:
                if(cntButtons>1)
                {
                    if(g_type == MSGBOX_TYPE_OKCANCEL)
                        res =  MSGBOX_CANCEL;
                    else if(g_type == MSGBOX_TYPE_YESNO)
                        res =  MSGBOX_NO;
                    else if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_NO;
                }
                stopBoxLoop=1;
                break;
    
            case BTN_F3:
                if(cntButtons>2)
                {
                    if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_CANCEL;
                }
                stopBoxLoop=1;
                break;
    
            case BTN_OFF:
                res = MSGBOX_NOTHING;
                stopBoxLoop=1;
                break;
        }
    }
}

void iniMsgBox(void)
{
    MsgBox_ExclamationBitmap=&getIcon("MsgBoxExclamationBitmap")->bmap_data;
    MsgBox_QuestionBitmap=&getIcon("MsgBoxQuestionBitmap")->bmap_data;
    MsgBox_WarningBitmap=&getIcon("MsgBoxWarningBitmap")->bmap_data;
    MsgBox_InformationBitmap=&getIcon("MsgBoxInformationBitmap")->bmap_data;
    MsgBox_ErrorBitmap=&getIcon("MsgBoxErrorBitmap")->bmap_data;
}

/* draw the msg box */
void drawMsgBox(unsigned char* caption, unsigned char* msg, int type, int icon)
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

    // calculate width
    setFont(STD7X13);
    getStringS(caption, &w1, &h1);
    setFont(STD5X8);
    getStringS(msg, &w2, &h2);
    
    // calculate width of box for text
    if(w1>w2)
        width = w1 + 20;
    else
        width = w2 + 20;

    // calculate width of box for buttons
    if(type == MSGBOX_TYPE_OK)
    {
        cntButtons = 1; //only one button
        if(width < MSGBOX_1BUTTON_MIN_WIDTH) width = MSGBOX_1BUTTON_MIN_WIDTH;
    }
    else if(type == MSGBOX_TYPE_YESNOCANCEL)
    {
        cntButtons = 3; //three buttons
        if(width < MSGBOX_3BUTTON_MIN_WIDTH) width = MSGBOX_3BUTTON_MIN_WIDTH;
    }
    else
    {
        cntButtons = 2; //two buttons
        if(width < MSGBOX_2BUTTON_MIN_WIDTH) width = MSGBOX_2BUTTON_MIN_WIDTH;
    }

    // the width must be a multiple of 32 !
    value = width/8;
    width = value*8;

    buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
    buttonOffset = (buttonPos - BUTTON_WIDTH)/2;

    setSize(BMAP2,width,MSGBOX_HEIGHT,8);

    // center box
    posX = SCREEN_WIDTH-width;
    posY = (SCREEN_HEIGHT/2)-(MSGBOX_HEIGHT/2);

    setPos(BMAP2,0x14+posX,0x13+posY);

    // show box
    setPlane(BMAP2);
    
    // fill background
    drawRect(COLOR_BLACK, 0, 0, width, MSGBOX_HEIGHT);
    fillRect(COLOR_WHITE, 1, 1, width-2, MSGBOX_HEIGHT-2);
    // write caption
    setFont(STD7X13);
    posX = (width/2)-(w1/2);
    putS(COLOR_BLACK, COLOR_WHITE, posX, 2, caption);
    drawLine(COLOR_BLACK,0, h1+2+2, width, h1+2+2);
    // draw bitmap and buttons
    if(icon == MSGBOX_ICON_EXCLAMATION)
        drawBITMAP (MsgBox_ExclamationBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_WARNING)
        drawBITMAP (MsgBox_WarningBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_INFORMATION)
        drawBITMAP (MsgBox_InformationBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_QUESTION)
        drawBITMAP (MsgBox_QuestionBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_ERROR)
        drawBITMAP (MsgBox_ErrorBitmap, 2,h1+2+2+5);
    // print message
    setFont(STD5X8);
    if(icon == MSGBOX_ICON_NO_ICON)
        putS(COLOR_BLACK, COLOR_WHITE, 2, h1+2+2+10, msg); // if no icon, print the text on the left side
    else
        putS(COLOR_BLACK, COLOR_WHITE, 25, h1+2+2+10, msg); // xPos with offset for icon
    // print buttons
    if(type == MSGBOX_TYPE_OK)
    {
        cntButtons = 1; //only one button

        buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
        buttonOffset = (buttonPos - BUTTON_WIDTH)/2;

        for(i = 0; i < cntButtons; i++)
        {
            drawRect(COLOR_BLACK, BUTTON_DISTANCE+i*buttonPos+buttonOffset, MSGBOX_HEIGHT-20, BUTTON_WIDTH, BUTTON_HEIGHT);

            getStringS("Ok(F1)", &w_buttonText, &h_buttonText);
            buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
            buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
            putS(COLOR_BLACK, COLOR_WHITE, BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX, MSGBOX_HEIGHT-20+buttonTextOffsetY , "Ok(F1)");
        }
    }
    else if(type == MSGBOX_TYPE_YESNOCANCEL)
    {
        cntButtons = 3; //three buttons

        buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
        buttonOffset = (buttonPos - BUTTON_WIDTH)/2;

        for(i = 0; i < cntButtons; i++)
        {
            drawRect(COLOR_BLACK, BUTTON_DISTANCE+i*buttonPos+buttonOffset, MSGBOX_HEIGHT-20, BUTTON_WIDTH, BUTTON_HEIGHT);

            if(i == 0)
                strcpy(strButtonText,"Yes(F1)");
            else if (i == 1)
                strcpy(strButtonText,"No(F2)");
            else
                strcpy(strButtonText,"Cancel(F3)");

            getStringS(strButtonText, &w_buttonText, &h_buttonText);
            buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
            buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
            putS(COLOR_BLACK, COLOR_WHITE, BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX, MSGBOX_HEIGHT-20+buttonTextOffsetY , strButtonText);
        }
    }
    else
    {
        cntButtons = 2; //two buttons

        buttonPos = (width-(2*BUTTON_DISTANCE))/cntButtons;
        buttonOffset = (buttonPos - BUTTON_WIDTH)/2;

        for(i = 0; i < cntButtons; i++)
        {
            drawRect(COLOR_BLACK, BUTTON_DISTANCE+i*buttonPos+buttonOffset, MSGBOX_HEIGHT-20, BUTTON_WIDTH, BUTTON_HEIGHT);

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

            getStringS(strButtonText, &w_buttonText, &h_buttonText);
            buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
            buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
            putS(COLOR_BLACK, COLOR_WHITE, BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX, MSGBOX_HEIGHT-20+buttonTextOffsetY , strButtonText);
        }
    }
    
    showPlane(BMAP2);

    setPlane(BMAP1);
}

int ini_font;

/* restore the previous state */
void eraseMsgBox(void)
{
    hidePlane(BMAP2);
    setPlane(BMAP1);
    setFont(ini_font);
}

/* main function */
int msgBox(unsigned char* caption, unsigned char* msg, int type, int icon)
{
    g_type = type;
    ini_font=getFont();
    drawMsgBox(caption,msg,type,icon);
    msgBoxEvtHandler(get_evt_pipe());
    //msgEvtLoop();
    eraseMsgBox();
    return res;
}
