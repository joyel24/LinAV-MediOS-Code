/*
* msgbox.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2124 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include "font.h"
#include "graphics.h"
#include "avstring.h"
#include "colordef.h"
#include "avevents.h"
#include "events.h"
#include "osd.h"
#include "msgBox.h"

extern struct plugin msg_box_plugin;

needFont(std5x8);
needFont(std6x9);
needFont(std7x13);

#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT 15
#define BUTTON_DISTANCE 10


unsigned char MsgBoxExclamation[18][18] =
{   {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,12,12,12,12,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,12,12,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
};
BITMAP MsgBoxExclamationBitmap = {(unsigned int) MsgBoxExclamation, 18, 18, 0, 0};

unsigned char MsgBoxQuestion[18][18] =
{   {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,00,00,15,00,00,00,15,15,15,15,15,15},
    {15,15,15,15,15,15,00,00,15,15,00,00,15,15,15,15,15,15},
    {15,15,15,15,15,15,00,15,15,15,15,00,00,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,00,00,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,00,00,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
};
BITMAP MsgBoxQuestionBitmap = {(unsigned int) MsgBoxQuestion, 18, 18, 0, 0};

unsigned char MsgBoxWarning[18][18] =
{   {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
};
BITMAP MsgBoxWarningBitmap = {(unsigned int) MsgBoxWarning, 18, 18, 0, 0};

unsigned char MsgBoxInformation[18][18] =
{   {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
};
BITMAP MsgBoxInformationBitmap = {(unsigned int) MsgBoxInformation, 18, 18, 0, 0};

unsigned char MsgBoxError[18][18] =
{   {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,00,00,00,00,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,00,00,15,15,15,15,15,15,15,15},
    {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15}
};
BITMAP MsgBoxErrorBitmap = {(unsigned int) MsgBoxError, 18, 18, 0, 0};


int res; // updated in msgBoxEvtHandler
// we should add some define in msgBox.h such as RES_OK RES_YES RES_NO RES_CANCEL ...
// maybe a param to msgBox to set the type of box: alert, choice yes/no, input text


int stopBoxLoop = 0; /* global variable used to stop the private evt loop*/
int cntButtons  = 0; /* Buttons in MSgBox */
int g_type = MSGBOX_TYPE_OK;

/*our private loop*/
void msgEvtLoop(void)
{
    msg_box_plugin.handle_on=1;
    stopBoxLoop=0;
    while(!stopBoxLoop)
        procNxtEvent(waitEvent());
    msg_box_plugin.handle_on=0;
}

/* msg_box event handler */
void msgBoxEvtHandler(int evt)
{
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

// called in avwm.c main()
void iniMsgBox(void)
{
    doRegisterPlugin(&msg_box_plugin,msgBoxEvtHandler,0);
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
    setFont(std7x13);
    getStringS(caption, &w1, &h1);
    setFont(std5x8);
    getStringS(msg, &w2, &h2);

    if(w1>w2)
        width = w1 + 20;
    else
        width = w2 + 20;

    if(width < MSGBOX_MIN_WIDTH)
        width = MSGBOX_MIN_WIDTH;

    // the width must be a multiple of 32 !
    value = width/32;
    width = value*32;

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
    setFont(std7x13);
    posX = (width/2)-(w1/2);
    putS(COLOR_BLACK, COLOR_WHITE, posX, 2, caption);
    drawLine(COLOR_BLACK,0, h1+2+2, width, h1+2+2);

    // draw bitmap and buttons
    if(icon == MSGBOX_ICON_EXCLAMATION)
        drawBITMAP (&MsgBoxExclamationBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_WARNING)
        drawBITMAP (&MsgBoxWarningBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_INFORMATION)
        drawBITMAP (&MsgBoxInformationBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_QUESTION)
        drawBITMAP (&MsgBoxQuestionBitmap, 2,h1+2+2+5);
    else if(icon == MSGBOX_ICON_ERROR)
        drawBITMAP (&MsgBoxErrorBitmap, 2,h1+2+2+5);

    // print message
    setFont(std5x8);
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

            getStringS("Ok", &w_buttonText, &h_buttonText);
            buttonTextOffsetX = (BUTTON_WIDTH/2)-(w_buttonText/2);
            buttonTextOffsetY = (BUTTON_HEIGHT/2)-(h_buttonText/2);
            putS(COLOR_BLACK, COLOR_WHITE, BUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX, MSGBOX_HEIGHT-20+buttonTextOffsetY , "Ok");
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
                strcpy(strButtonText,"Yes");
            else if (i == 1)
                strcpy(strButtonText,"No");
            else
                strcpy(strButtonText,"Cancel");

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
                    strcpy(strButtonText,"Yes");
                else
                    strcpy(strButtonText,"Ok");
            }
            else
            {
                if(type == MSGBOX_TYPE_YESNO)
                    strcpy(strButtonText,"No");
                else
                    strcpy(strButtonText,"Cancel");
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

/* restore the previous state */
void eraseMsgBox(void)
{
    hidePlane(BMAP2);
    setPlane(BMAP1);
    setFont(std6x9);
}

/* main function */
int msgBox(unsigned char* caption, unsigned char* msg, int type, int icon)
{
    g_type = type;
    drawMsgBox(caption,msg,type,icon);
    msgEvtLoop();
    eraseMsgBox();
    return res;
}
