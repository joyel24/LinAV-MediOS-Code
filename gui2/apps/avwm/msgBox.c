/*
* avstring.c
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


int msgBox(char * msg)
{
    int w=0,h=0;
    
    getStringS(msg, &w, &h);
    
    w+=10;
    h+=10;
   
    
    fillRect(COLOR_BLACK,(SCREEN_WIDTH-w)/2,(SCREEN_HEIGHT-h)/2,w,h);
    putS(COLOR_WHITE,COLOR_BLACK,(SCREEN_WIDTH-w)/2+5,(SCREEN_HEIGHT-h)/2+5,msg);
    
}
