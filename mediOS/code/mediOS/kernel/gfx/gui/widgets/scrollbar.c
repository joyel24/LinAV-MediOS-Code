/*
* kernel/gui/widget/scrollbar.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/graphics.h>
#include <gui/scrollbar.h>

/*
 * Print a scroll bar
 */
void draw_scrollBar(struct scroll_bar * scrollBar, int items, int min_shown,int max_shown)
{
    int min;
    int max;
    int start;
    int size;
    
    int x,y,width,height,orientation,color,bg_color,border_color;
    
    x=scrollBar->x;
    y=scrollBar->y;
    width=scrollBar->width;
    height=scrollBar-> height;
    color=scrollBar->fg_color;
    border_color=scrollBar->border_color;
    bg_color=scrollBar->bg_color;
    orientation=scrollBar->orientation;
    
    /* draw box */
    gfx_drawRect(border_color, x, y, width, height);

    /* clear edge pixels */
    gfx_drawPixel(bg_color, x, y);
    gfx_drawPixel(bg_color, (x + width - 1), y);
    gfx_drawPixel(bg_color, x, (y + height - 1));
    gfx_drawPixel(bg_color, (x + width - 1), (y + height - 1));

    /* clear pixels in progress bar */
    gfx_fillRect(bg_color, x + 1, y + 1, width - 2, height - 2);

    /* min should be min */
    if(min_shown < max_shown) {
        min = min_shown;
        max = max_shown;
    }
    else {
        min = max_shown;
        max = min_shown;
    }

    /* limit min and max */
    if(min < 0)
        min = 0;
    if(min > items)
        min = items;

    if(max < 0)
        max = 0;
    if(max > items)
        max = items;

    /* calc start and end of the knob */
    if(items > 0 && items > (max - min)) {
        if(orientation == VERTICAL) {
            size = (height - 2) * (max - min) / items;
            start = (height - 2 - size) * min / (items - (max - min));
        }
        else {
            size = (width - 2) * (max - min) / items;
            start = (width - 2 - size) * min / (items - (max - min));
        }
    }
    else { /* if null draw a full bar */
        start = 0;
        if(orientation == VERTICAL)
            size = (height - 2);
        else
            size = (width - 2);
    }

    /* knob has a width */
    if(size != 0) {
        if(orientation == VERTICAL)
            gfx_fillRect(color, x + 1, y + start + 1, width - 2, size);
        else
            gfx_fillRect(color, x + start + 1, y + 1, size, height - 2);
    }
    else { /* width of knob is null */
        if(orientation == VERTICAL) {
            start = (height - 2 - 1) * min / items;
            gfx_fillRect(color, x + 1, y + start + 1, width - 2, 1);
        }
        else {
            start = (width - 2 - 1) * min / items;
            gfx_fillRect(color, x + start + 1, y + 1, 1, height - 2);
        }
    }
}

