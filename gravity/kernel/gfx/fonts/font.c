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

#include <kernel/kfont.h>
#include <kernel/kgraphics.h>

FONT_ID font_table[NBFONT] ;

#define font_ini(name)  extern FONT_ID name; font_table[name->num]=name;

void ini_font(void)
{
    int i;
    for(i=0;i<NB_FONT;i++)
        font_table[i]=NULL;

#ifdef _std4x6
font_ini(std4x6)
#endif
#ifdef _std4x8
    font_ini(std4x8)
#endif
#ifdef _std5x7
    font_ini(std5x7)
#endif
#ifdef _std5x8
    font_ini(std5x8)
#endif
#ifdef _std6x9
    font_ini(std6x9)
#endif
#ifdef _std6x10
    font_ini(std6x10)
#endif
#ifdef _std6x12
    font_ini(std6x12)
#endif
#ifdef _std6x13
    font_ini(std6x13)
#endif
#ifdef _std7x13
    font_ini(std7x13)
#endif
#ifdef _std7x14
    font_ini(std7x14)
#endif
#ifdef _std8x13
    font_ini(std7x13)
#endif
#ifdef _cursive_9x15
    font_ini(cursive)
#endif
#ifdef _dagger_9x14
    font_ini(dagger)
#endif
#ifdef _inkblot_9x13
    font_ini(inkblot)
#endif
#ifdef _radon_8x12
    font_ini(radon)
#endif
#ifdef _radonWide_9x12
    font_ini(radonWide)
#endif
#ifdef _shadow_12x18
    font_ini(shadow)
#endif
#ifdef _shadowBold_12x18
    font_ini(shadowBold)
#endif
}
