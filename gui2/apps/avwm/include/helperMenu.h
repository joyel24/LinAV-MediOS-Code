/*
* helperMenu.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Zakk Roberts
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __HELPERMENU_H
#define __HELPERMENU_H

#include "colordef.h"

#define ALIGN_LEFT    1
#define ALIGN_RIGHT   0

struct helperMenu {

    char * ON_txt;
    char * OFF_txt;
    char * JOY_txt;
    char * F1_txt;
    char * F2_txt;
    char * F3_txt;
    
    int helperDelay; /* not implemented yet */
    int helperSpeed; /* 200 is a good value */
    
    int bg_color;
    int border_color;
    int txt_color;
    
    int align;
};

void  openHelper            (void);
void  closeHelper           (void);
void  hideHelper            (void);
void  helperEvt             (int evt,int action_btn);

void* openHelperMenu        (void* arg);
void  drawhelperMenuBox     (struct helperMenu * menu);

#endif
