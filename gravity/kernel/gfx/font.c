/* 
*   kernel/gfx/font.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/font.h>
#include <kernel/kgraphics.h>

needFont(std4x6);

FONT_ID font_table[NBFONT] ;


void ini_font(void)
{
    font_table[0]=std4x6;
}