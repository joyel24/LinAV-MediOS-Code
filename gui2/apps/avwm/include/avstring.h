/*
* avstring.h
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

#ifndef __AVSTRING_H
#define __AVSTRING_H

void   ini_font         (int font);
void   wmPutS           (int color, int bg_color,int x, int y, char *s);
void   wmPutC           (int color, int bg_color,int x, int y, char s);
void   wmSetFont        (int font);
void   wmgetStringS     (const unsigned char *str, int *w, int *h);
void   defaultFont      (void);
void   debug            (char *fmt, ...);

#endif