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

extern struct plugin edit_box_plugin;

#define EDITBOX_HEIGHT 33
#define EDITBOX_WIDTH  224 // must be a multiple of 32

#define ASCII_DEFAULT 65

needFont(std6x9);
needFont(std7x13);

char* strEdit; // updated in editBoxEvtHandler

int stopEditBoxLoop = 0; /* global variable used to stop the private evt loop*/

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
    switch(evt)
    {
        case BTN_F1:
            break;

        case BTN_F2:
            break;

        case BTN_F3:
            break;

        case BTN_OFF:
            stopEditBoxLoop=1;
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
    int w1 = 0;
    int h1 = 0;
    int posX = 50;
    int posY = 80;

    setFont(std7x13);
    getStringS(caption, &w1, &h1);
    setFont(std6x9);

    setSize(BMAP2,EDITBOX_WIDTH,EDITBOX_HEIGHT, 8);
    setPos(BMAP2,0x14+posX,0x13+posY);

    // show box
    setPlane(BMAP2);

    // fill background
    drawRect(frame_color, 0, 0, EDITBOX_WIDTH, EDITBOX_HEIGHT);
    fillRect(bk_color, 1, 1, EDITBOX_WIDTH-2, EDITBOX_HEIGHT-2);

    setFont(std7x13);
    putS(text_color, bk_color, 2, 2, caption);
    drawLine(COLOR_BLACK,0, h1+2+2, EDITBOX_WIDTH, h1+2+2);

    // write given text
    setFont(std6x9);
    putS(text_color, bk_color, 2, h1+2+5, text);

    showPlane(BMAP2);

    setPlane(BMAP1);
}

/* restore the previous state */
void eraseEditBox(void)
{
    hidePlane(BMAP2);
    setPlane(BMAP1);
    setFont(std6x9);
}

/* main function */
char* editBox(unsigned char* caption, unsigned char* text, int text_color, int bk_color, int frame_color, int cursor_color)
{
    drawEditBox(caption,text,text_color,bk_color,frame_color,cursor_color);
    editEvtLoop();
    eraseEditBox();
    return strEdit;
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

