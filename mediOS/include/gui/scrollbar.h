/*
* include/gui/scrollbar.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_SCROLLBAR_H
#define __GUI_SCROLLBAR_H

struct scroll_bar {
    int border_color;
    int bg_color;
    int fg_color;
    int x;
    int y;
    int width;
    int height;
    int orientation;
};

#define VERTICAL   0
#define HORIZONTAL 1

void draw_scrollBar(struct scroll_bar * scrollBar, int items, int min_shown,int max_shown);

#endif
