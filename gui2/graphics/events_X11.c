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

extern Display* display;
extern Window window;	/*variable Event */

int nxtEvent(void)
{
    XEvent event;
    
    XSelectInput(display, window, ExposureMask);
    /* next event */
    XNextEvent(display, &event);
    
    switch (event.type) 
    {
      case Expose :
        lcd_update(); 
      break;
    }
    return 0; 
}  
