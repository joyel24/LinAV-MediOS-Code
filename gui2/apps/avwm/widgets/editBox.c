/*
* editbox.c
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

#include "font.h"
#include "graphics.h"
#include "avstring.h"
#include "colordef.h"
#include "avevents.h"
#include "events.h"
#include "osd.h"
#include "editBox.h"
#include "misc.h"

extern struct plugin edit_box_plugin;

#define EDITBOX_HEIGHT 55
#define EDITBOX_WIDTH  220

#define EDITBUTTON_WIDTH 70
#define EDITBUTTON_HEIGHT 15
#define EDITBUTTON_DISTANCE 10

#define ASCII_DEFAULT 65
#define ASCII_SET_BEGIN 32
#define ASCII_SET_END 126

#define true  1
#define false 0

#define MAX_EDITTIMER 2

needFont(std6x9);
needFont(std7x13);

char strEdit[MAX_EDIT_CHARS]; // updated in editBoxEvtHandler
int current_ascii = ASCII_DEFAULT;
int xEditPos = 0;
int yEditPos = 0;
int edit_text_color = 0;
int edit_bk_color = 0;
int edit_frame_color = 0;
int edit_cursor_color = 0;
int fUpKey = false;
int fDownKey = false;
int fValidEntry = false;
int indexChar = 0;
int cntTimer = 0;

int freq = 0;
int repeat = 0;

int stopEditBoxLoop = 0; /* global variable used to stop the private evt loop*/

int SetUnderlinedCursor(int x, int y, int ascii, int fSet);
int PrevAscii(int lastAscii);
int NextAscii(int lastAscii);
void SaveTempEntry(char* text, int indexChar, int ascii);
int GetCurrentAscii(char* text, int index, int oldascii);

//----routine to provide delay-----
void delay(unsigned int time) {
    for (;time>0;time--) ;
}

/*our private loop*/
void editEvtLoop(void)
{
    edit_box_plugin.handle_on=1;
    stopEditBoxLoop=0;
    while(!stopEditBoxLoop)
        procNxtEvent(waitEvent());
    edit_box_plugin.handle_on=0;
}

/* edit_box event handler */
void editBoxEvtHandler(int evt)
{
    int w = 0;
    int h = 0;

    switch(evt)
    {
        case EVT_TIMER:
            cntTimer++;
            break;

        case BTN_ON:
            break;

        case BTN_F3:

            // ToDo: Del Last char
            break;

        case BTN_UP:

            setPlane(BMAP2);
            setFont(std6x9);

            if(fDownKey == true)
            {
                set_mouseParam(6,3);
			    fDownKey = false;
                cntTimer = 0;
            }

            fValidEntry = true;

            current_ascii = NextAscii(current_ascii); // definierte Reihenfolge

            // Faster up and down

			if((fUpKey == true) && (cntTimer >= MAX_EDITTIMER))
                set_mouseParam(6,0);
			else
                set_mouseParam(6,3);

			fUpKey = true;

            SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, true);
            break;

        case BTN_DOWN:

            setPlane(BMAP2);
            setFont(std6x9);

            if(fUpKey == true)
            {
                set_mouseParam(6,3);
			    fUpKey = false;
                cntTimer = 0;
            }
			fValidEntry = true;

			current_ascii = PrevAscii(current_ascii);

            // Faster up and down

			if((fDownKey == true) && (cntTimer >= MAX_EDITTIMER))
                set_mouseParam(6,0);
			else
                set_mouseParam(6,3);

			fDownKey = true;

            SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, true);
            break;

        case BTN_LEFT:
            fUpKey = false;
            fDownKey = false;
            fValidEntry = true;
            cntTimer = 0;

            // ToDo: Add scrolling

            if(indexChar > 0)
            {
                SaveTempEntry(strEdit, indexChar, current_ascii);

                getStringS("M", &w, &h);

                SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, 0);
//                fillRect(edit_bk_color, xEditPos, xEditPos+h, w, 2 );
                xEditPos-=w;
                indexChar--;
//                delay(0x20000);
                current_ascii = GetCurrentAscii(strEdit, indexChar, current_ascii);
                SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, 1);
            }
            break;

        case BTN_RIGHT:
            fUpKey = false;
            fDownKey = false;
            fValidEntry = true;
            cntTimer = 0;

            if(indexChar+1 < MAX_EDIT_CHARS-1)
            {
                // ToDo: Add scrolling

                SaveTempEntry(strEdit, indexChar, current_ascii);

                getStringS("M", &w, &h);

                SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, 0);
//                fillRect(edit_bk_color, xEditPos, xEditPos+h, w, 2 );
                xEditPos+=w;
                indexChar++;
//                delay(0x20000);
                current_ascii = GetCurrentAscii(strEdit, indexChar, current_ascii);
                SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, 1);
            }
            break;

        case BTN_F1:
            stopEditBoxLoop=1;
            break;

        case BTN_F2:
            stopEditBoxLoop=1;
            memset(strEdit, 0, sizeof(strEdit));
            break;

        default:
            break;
    }
}

// called in avwm.c main()
void iniEditBox(void)
{
    doRegisterPlugin(&edit_box_plugin,editBoxEvtHandler,0);
}

/* draw the edit box */
void drawEditBox(unsigned char* caption, unsigned char* text, int text_color, int bk_color, int frame_color, int cursor_color)
{
    char strButtonText[10];

    int i = 0;
    int w1 = 0;
    int h1 = 0;
    int w2 = 0;
    int h2 = 0;
    int posX = 0;
    int posY = 0;
    int buttonPos = 0;
    int buttonOffset = 0;
    int w_buttonText = 0;
    int h_buttonText = 0;
    int buttonTextOffsetX=0;
    int buttonTextOffsetY=0;

    setFont(std7x13);
    getStringS(caption, &w1, &h1);
    setFont(std6x9);
    getStringS(caption, &w2, &h2); // for std6x9

    freq   = get_mouseFreq  ();
    repeat = get_mouseRepeat();

    setSize(BMAP2,EDITBOX_WIDTH,EDITBOX_HEIGHT, 8);

    // center box
    posX = SCREEN_WIDTH-EDITBOX_WIDTH;
    posY = (SCREEN_HEIGHT/2)-(EDITBOX_HEIGHT/2);
    setPos(BMAP2,0x14+posX,0x13+posY);

    // show box
    setPlane(BMAP2);

    // fill background
    drawRect(frame_color, 0, 0, EDITBOX_WIDTH, EDITBOX_HEIGHT);
    fillRect(bk_color, 1, 1, EDITBOX_WIDTH-2, EDITBOX_HEIGHT-2);

    // print caption
    setFont(std7x13);
    putS(text_color, bk_color, 2, 2, caption);
    drawLine(COLOR_BLACK,0, h1+2+2, EDITBOX_WIDTH, h1+2+2);

    // print given text
    setFont(std6x9);
    xEditPos = 2;
    yEditPos = h1+2+5;

    putS(text_color, bk_color, xEditPos, yEditPos, text);

    SetUnderlinedCursor(xEditPos, yEditPos, current_ascii, 1);

    // draw Line under text
    setPlane(BMAP2);
    drawLine(COLOR_BLACK,0, h1+2+5+h2+3, EDITBOX_WIDTH, h1+2+5+h2+3);

    // draw buttons
    buttonPos = (EDITBOX_WIDTH-(2*EDITBUTTON_DISTANCE))/2;
    buttonOffset = (buttonPos - EDITBUTTON_WIDTH)/2;

    for(i = 0; i < 2; i++)
    {
        drawRect(COLOR_BLACK, EDITBUTTON_DISTANCE+i*buttonPos+buttonOffset, EDITBOX_HEIGHT-19, EDITBUTTON_WIDTH, EDITBUTTON_HEIGHT);

        if(i == 0)
            strcpy(strButtonText,"Ok(F1)");
        else
            strcpy(strButtonText,"Cancel(F2)");

        getStringS(strButtonText, &w_buttonText, &h_buttonText);
        buttonTextOffsetX = (EDITBUTTON_WIDTH/2)-(w_buttonText/2);
        buttonTextOffsetY = (EDITBUTTON_HEIGHT/2)-(h_buttonText/2);
        putS(COLOR_BLACK, COLOR_WHITE, EDITBUTTON_DISTANCE+i*buttonPos+buttonOffset+buttonTextOffsetX, EDITBOX_HEIGHT-19+buttonTextOffsetY , strButtonText);
    }

    showPlane(BMAP2);

    setPlane(BMAP1);
    setFont(std6x9);
}

/* restore the previous state */
void eraseEditBox(void)
{
    hidePlane(BMAP2);
    setPlane(BMAP1);
    setFont(std6x9);
    set_mouseParam(freq, repeat);
}

/* main function */
char* editBox(unsigned char* caption, unsigned char* text, int text_color, int bk_color, int frame_color, int cursor_color)
{
    edit_text_color   = text_color;
    edit_bk_color     = bk_color;
    edit_frame_color  = frame_color;
    edit_cursor_color = cursor_color;

    strcpy(strEdit,text);

    if(strlen(strEdit) > 0)
    {
        current_ascii = strEdit[0];
    }
    else
    {
        current_ascii = ASCII_DEFAULT;
    }

    indexChar = 0;

    drawEditBox(caption,text,text_color,bk_color,frame_color,cursor_color);
    editEvtLoop();
    eraseEditBox();
    return strEdit;
}

void SaveTempEntry(char* text, int indexChar, int ascii)
{
    int len = 0;
    char temp[1];
    temp[0] = ascii;
//   temp[1] = '\0';

    len = strlen(text);
    text[indexChar] = ascii;
	if(indexChar >= len)
        text[indexChar+1] = '\0';
}

int GetCurrentAscii(char* text, int index, int oldascii)
{
    int ascii = ASCII_DEFAULT; // A

    if(index >= strlen(text))
        ascii = oldascii;
    else
        ascii = text[index];

    if(ascii < ASCII_SET_BEGIN)
        ascii = ASCII_DEFAULT;

    return ascii;
}

void DeleteChar(char* src, char* dest, int index)
{
   int cnt = 0;
    int dstindex = 0;

    for( cnt = 0; cnt < strlen(src); cnt++)
    {
       if(cnt != index)
        {
          dest[dstindex] = src[cnt];
            dstindex++;
        }
    }

    dest[dstindex] = '\0';
}

// Bereiche in folgender Reihenfolge:
// 33-44 Sonderzeichen
// 58-63 Sonderzeichen
// 91-96 Sonderzeichen
// 123-126 Sonderzeichen
// 45-57 Zahlen
// 32 Space
// 64-90 Grossbuchstaben
// 97-122 Kleinbuchstaben
// 192-255 Erweiterter Zeichensatz
int NextAscii(int lastAscii)
{
   int ascii = ASCII_DEFAULT;

   if(lastAscii == 32) // Space
      ascii = 64; // auf @ und Grossbuchstaben springen
    else if(lastAscii == 90)
      ascii = 97;
    else if(lastAscii == 122)
       ascii = 192;
    else if(lastAscii == 255)
      ascii = 33;
    else if(lastAscii == 44)
       ascii = 58;
    else if(lastAscii == 63)
      ascii = 91;
    else if(lastAscii == 96)
      ascii = 123;
    else if(lastAscii == 126)
      ascii = 45;
    else if(lastAscii == 57)
      ascii = 32;
    else
      ascii = lastAscii+1;

    return ascii;
}

int PrevAscii(int lastAscii)
{
   int ascii = ASCII_DEFAULT;

   if(lastAscii == 33)
       ascii = 255;
    else if(lastAscii == 192)
      ascii = 122;
    else if(lastAscii == 97)
       ascii = 90;
    else if(lastAscii == 64)
      ascii = 32;
    else if(lastAscii == 32)
      ascii = 57;
    else if(lastAscii == 45)
      ascii = 126;
    else if(lastAscii == 123)
      ascii = 96;
    else if(lastAscii == 91)
      ascii = 63;
    else if(lastAscii == 58)
      ascii = 44;
    else
      ascii = lastAscii-1;

    return ascii;
}

int SetUnderlinedCursor(int x, int y, int ascii, int fSet)
{
   int w = 0;
   int h = 0;
   char temp[2];
   temp[0] = ascii;
   temp[1] = '\0';

   setPlane(BMAP2);
   setFont(std6x9);
   getStringS(temp, &w, &h);

   // clear rect of char
   fillRect(edit_bk_color, x, y, w, h+2);
//   sprintf(tmp,"%d %d",w,h);
//   putS(edit_text_color, edit_bk_color, 80, 2, tmp);

   // print char
   putS(edit_text_color, edit_bk_color, x, y, temp);

   // print the underliner
   if(fSet)
       fillRect(edit_cursor_color, x, y+h, w, 2 );

   setPlane(BMAP1);

   return ascii;
}
