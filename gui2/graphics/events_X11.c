/*
* events.c
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

#include <stdio.h>
#include <stdlib.h>
#include "events.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "graphics.h"

/*===== define Keyboard // Archos ====== */
#define KEYBOARD_GENERIC \
  "Keyboard   Rockbox\n" \
  "--------   --------------\n" \
  "+          ON\n" \
  "8          UP\n" \
  "2          DOWN\n" \
  "4          LEFT\n" \
  "6          RIGHT\n" \
  "Enter      OFF\n" \
  "5          PLAY\n" \
  "/          F1\n" \
  "*          F2\n" \
  "-          F3\n"
/*=========== End of define ============ */

extern Display* display;
extern Window window;	/*variable Event */
XEvent event;

int nxtEvent(void)
{   
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
    /* next event */
    XNextEvent(display, &event);
    
    
    switch (event.type) 
    {
      case Expose :
        lcd_update(FORCE_REDRAW,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
      break;
      case KeyPress :
        getKey(event.type);
      break;
      case KeyRelease :
      break;
    }
    return 0; 
}  

int getKey(XEvent *event)
{
    switch (event.type)
    {
    case ON :
       return BTN_ON;
    break;
    case OFF :
       return BTN_OFF;
    break;
    case F1 :
       return BTN_F1;
    break;
    case F2 :
       return BTN_F2;
    break;
    case PLAY :
       return BTN_JOY;
    break;
    case F3 :
       return BTN_F3;
    break;
    case UP :
       return BTN_UP;
    break;
    case DOWN :
       return BTN_DOWN;
    break;
    case LEFT :
       return BTN_LEFT;
    break;
    case RIGHT :
       return BTN_RIGHT;
    break;
    }
}

int waitEvent(void)
{
    int evt;
    while(1)
    {
      evt = nxtEvent();
      if(evt != NO_EVENT)
       break;
     }
  return evt;      
}

int iniEvent(void)
{
    return 0;
}

int clearEventQueue(void)
{
    return 0;    
}

int wakeUP(void)
{  
    return 0;    
}

int halt_device(void)
{
    return 0;    
}

int pause_app(void)
{   
    return 0;    
}

int release_app(void)
{    
    return 0; 
}

int setRepeate(int val)
{
    return 0; 
}

int getRepeate(void)
{
    return 0; 
}

int setFreq(int val)
{
    return 0; 
}

int getFreq(void)
{
    return 0; 
}

int setSettings(struct mouseParam * param)
{
    return 0; 
}

int getSettings(struct mouseParam * param)
{
    return 0; 
}

int setTimerFreq(int val)
{
    return 0; 
}

int startTimer()
{
    return 0; 
}

int stopTimer()
{
    return 0; 
}

int timerState()
{
    return 0; 
}
