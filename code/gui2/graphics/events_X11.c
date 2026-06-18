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
#include "misc.h"
#include <time.h>

/* Define for battery */

#define BATMAX 1600
#define BATMIN 1000
#define INCBAT 200

/* Global variables */
extern Display* display;
extern Window window;	/*variable Event */
int key;
XEvent event;
int stateUSB = 0;
int statePWR =0;
int stateBAT = BATMAX;
long oldtimes;

int nxtEvent(void)
{   
 
    int pending;
    pending = XPending(display);
    KeySym keysym;
    unsigned char c = 0;
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
    
  if(pending != 0)
  {
    /* next event */
    XNextEvent(display, &event);    
    
    switch (event.type) 
    {
      case Expose :
        lcd_update(FORCE_REDRAW,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
      break;
      case KeyPress :
	  XLookupString (&event.xkey, &c, 1, &keysym, 0);
          key = keysym;
          return getKey(key);
      break;
      case KeyRelease :
      break;
    }
   }  
    
    return EvTimer();
}


int EvTimer(void)
{
  time_t times;
  times = time(NULL);
     
  if(times > oldtimes)
   {
    return EVT_TIMER;
   }
  else
   {
     return NO_EVENT;
   }
   oldtimes = times;
}

int getKey(int key)
{
    switch(key)
    {
    /* Fonctions principales (boutons) */
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
	    
	/* Autres evenements */
	/*======USB======*/
	    
	case XK_u:
	case XK_U:
	  if(stateUSB == 0)
	  {
	    stateUSB = 1;
	    return EVT_USB;
	  }
	  else
	  {
	    stateUSB = 0;
	    return EVT_USB;
	  }
	break;
	/*=======POWER=======*/
	case XK_p:
	case XK_P:
	  if(statePWR == 0)
	  {
	    statePWR = 1;
	    return EVT_PWR;
	  }
	  else
	  {
	    statePWR = 0;
	    return EVT_PWR;
	  }
	break;
	/*=============BATTERY +==========*/
	case XK_w:
	       stateBAT = stateBAT+INCBAT;
	       if(stateBAT > BATMAX)
	         stateBAT = BATMAX;
	break;
	/*=============BATTERY -==========*/
	case XK_x:
	       stateBAT = stateBAT-INCBAT;
	       if(stateBAT < BATMIN)
	         stateBAT = BATMIN;
	break;
        }
  return NO_EVENT;
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
