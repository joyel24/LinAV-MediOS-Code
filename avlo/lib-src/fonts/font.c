/*
*   lib/fonts/font.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stddef.h>

#include <font.h>
#include <graphics.h>

FONT_ID font_table[NBFONT] ;

#define font_ini(name)  extern FONT_ID name; font_table[name->num]=name;

void ini_font(void)
{
    int i;
    for(i=0;i<NBFONT;i++)
        font_table[i]=NULL;

    font_ini(std6x9)
    font_ini(std8x13)
}
