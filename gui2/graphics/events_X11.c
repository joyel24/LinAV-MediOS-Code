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
#include "X11/keysym.h"

extern Display* display;
extern Window window;	/*variable Event */
int key;
XEvent event;

int nxtEvent(void)
{   
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
    /* next event */
    XNextEvent(display, &event);
    KeySym keysym;
    unsigned char c = 0;
    
    
    switch (event.type) 
    {
      case Expose :
        lcd_update(FORCE_REDRAW,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
      break;
      case KeyPress :
	  XLookupString (event.xkey, &c, 1, &keysym, 0);
          key = keysym;
          getKey(key);
      break;
      case KeyRelease :
      break;
    }
    return 0; 
}  

int getKey(int key)
{
    switch(key)
    {
	case XK_KP_Left:
	case XK_Left:
	case XK_KP_4:
	    return BTN_LEFT;
            break;

	case XK_KP_Right:
	case XK_Right:
	case XK_KP_6:
	    return BTN_RIGHT;
            break;

	case XK_KP_Up:
	case XK_Up:
	case XK_KP_8:
	    return BTN_UP;
            break;

	case XK_KP_Down:
	case XK_Down:
	case XK_KP_2:
	    return BTN_DOWN;
            break;

	case XK_KP_Space:
	case XK_KP_5:
	case XK_KP_Begin:
	case XK_space:
	    return BTN_JOY;
            break;

	case XK_KP_Enter:
	case XK_A:
	case XK_a:
	case XK_Return:
	    return BTN_OFF;
            break;

	case XK_KP_Add:
	case XK_plus:
	    return BTN_ON;
            break;

	case XK_KP_Divide:
	case XK_1:
	    return BTN_F1;
            break;

	case XK_KP_Multiply:
	case XK_2:
	    return BTN_F2;
            break;

	case XK_KP_Subtract:
	case XK_3:
	    return BTN_F3;
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
