/*
* scrollbar.h
*
* linav - http://linav.sourceforge.net
*
* Copyright (C) 2004 Zakk Roberts
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#define VERTICAL   0
#define HORIZONTAL 1

void scrollbar(int color, int bg_color, int x, int y, int width, int height, int items, int min_shown,
               int max_shown, int orientation);

#endif