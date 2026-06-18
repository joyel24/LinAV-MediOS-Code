/*
* 7seg_clock.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Zakk Roberts
* Most code (c) 2004 by Linus Nielsen Feltzing
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#include "cops.h"
#include "colordef.h"
#include "global.h"

/* Initialization, REQUIRED */
struct client_operations * cops;

/*************************************************************
 * 7-Segment LED/LCD imitation code, by Linus Nielsen Feltzing
 ************************************************************/
/*
       a     0    b
        #########c
       #         #`
       #         #
      1#         #2
       #         #
       #    3    #
      c ######### d
       #         #
       #         #
      4#         #5
       #         #
       #    6    #
      e ######### f
*/
static unsigned int point_coords[6][2] =
{
    {0, 0}, /* a */
    {1, 0}, /* b */
    {0, 1}, /* c */
    {1, 1}, /* d */
    {0, 2}, /* e */
    {1, 2}  /* f */
};

/********************************************
 * The end points (a-f) for each segment line
 *******************************************/
static unsigned int seg_points[7][2] =
{
    {0,1}, /* a to b */
    {0,2}, /* a to c */
    {1,3}, /* b to d */
    {2,3}, /* c to d */
    {2,4}, /* c to e */
    {3,5}, /* d to f */
    {4,5}  /* e to f */
};

/**********************************************************************
 * Lists that tell which segments (0-6) to enable for each digit (0-9),
 * the list is terminated with -1
 *********************************************************************/
static int digit_segs[10][8] =
{
    {0,1,2,4,5,6, -1},   /* 0 */
    {2,5, -1},           /* 1 */
    {0,2,3,4,6, -1},     /* 2 */
    {0,2,3,5,6, -1},     /* 3 */
    {1,2,3,5, -1},       /* 4 */
    {0,1,3,5,6, -1},     /* 5 */
    {0,1,3,4,5,6, -1},   /* 6 */
    {0,2,5, -1},         /* 7 */
    {0,1,2,3,4,5,6, -1}, /* 8 */
    {0,1,2,3,5,6, -1}    /* 9 */
};

/***********************************
 * Draws one segment - LED imitation
 **********************************/
void draw_seg_led(int seg, int x, int y, int width, int height)
{
    int p1 = seg_points[seg][0];
    int p2 = seg_points[seg][1];
    int x1 = point_coords[p1][0];
    int y1 = point_coords[p1][1];
    int x2 = point_coords[p2][0];
    int y2 = point_coords[p2][1];

    /* It draws parallel lines of different lengths for thicker segments */
    if(seg == 0 || seg == 3 || seg == 6)
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1 , y + y2 * height / 2);

        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 - 1,
                         x + x2 * width - 2, y + y2 * height / 2 - 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 + 1,
                         x + x2 * width - 2, y + y2 * height / 2 + 1);

        cops->drawLine(COLOR_WHITE, x + x1 * width + 3, y + y1 * height / 2 - 2,
                         x + x2 * width - 3, y + y2 * height / 2 - 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 3, y + y1 * height / 2 + 2,
                         x + x2 * width - 3, y + y2 * height / 2 + 2);
    }
    else
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width, y + y1 * height / 2 + 1,
                         x + x2 * width , y + y2 * height / 2 - 1);

        cops->drawLine(COLOR_WHITE, x + x1 * width - 1, y + y1 * height / 2 + 2,
                         x + x2 * width - 1, y + y2 * height / 2 - 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2 + 2,
                         x + x2 * width + 1, y + y2 * height / 2 - 2);

        cops->drawLine(COLOR_WHITE, x + x1 * width - 2, y + y1 * height / 2 + 3,
                         x + x2 * width - 2, y + y2 * height / 2 - 3);

        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 + 3,
                         x + x2 * width + 2, y + y2 * height / 2 - 3);
    }
}

/***********************************
 * Draws one segment - LCD imitation
 **********************************/
void draw_seg_lcd(int seg, int x, int y, int width, int height)
{
    int p1 = seg_points[seg][0];
    int p2 = seg_points[seg][1];
    int x1 = point_coords[p1][0];
    int y1 = point_coords[p1][1];
    int x2 = point_coords[p2][0];
    int y2 = point_coords[p2][1];

    if(seg == 0)
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width,     y + y1 * height / 2 - 1,
                         x + x2 * width,     y + y2 * height / 2 - 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 + 1,
                         x + x2 * width - 2, y + y2 * height / 2 + 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 3, y + y1 * height / 2 + 2,
                         x + x2 * width - 3, y + y2 * height / 2 + 2);
    }
    else if(seg == 3)
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 - 1,
                         x + x2 * width - 2, y + y2 * height / 2 - 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 + 1,
                         x + x2 * width - 2, y + y2 * height / 2 + 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 3, y + y1 * height / 2 - 2,
                         x + x2 * width - 3, y + y2 * height / 2 - 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 3, y + y1 * height / 2 + 2,
                         x + x2 * width - 3, y + y2 * height / 2 + 2);
    }
    else if(seg == 6)
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width,     y + y1 * height / 2 + 1,
                         x + x2 * width,     y + y2 * height / 2 + 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 - 1,
                         x + x2 * width - 2, y + y2 * height / 2 - 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 3, y + y1 * height / 2 - 2,
                         x + x2 * width - 3, y + y2 * height / 2 - 2);

    }
    else if(seg == 1 || seg == 4)
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width - 1, y + y1 * height / 2,
                         x + x2 * width - 1, y + y2 * height / 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width,     y + y1 * height / 2 + 1,
                         x + x2 * width,     y + y2 * height / 2 - 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2 + 2,
                         x + x2 * width + 1, y + y2 * height / 2 - 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width + 2, y + y1 * height / 2 + 3,
                         x + x2 * width + 2, y + y2 * height / 2 - 3);
    }
    else if(seg == 2 || seg == 5)
    {
        cops->drawLine(COLOR_WHITE, x + x1 * width + 1, y + y1 * height / 2,
                         x + x2 * width + 1, y + y2 * height / 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width,     y + y1 * height / 2 + 1,
                         x + x2 * width,     y + y2 * height / 2 - 1);
        cops->drawLine(COLOR_WHITE, x + x1 * width - 1, y + y1 * height / 2 + 2,
                         x + x2 * width - 1, y + y2 * height / 2 - 2);
        cops->drawLine(COLOR_WHITE, x + x1 * width - 2, y + y1 * height / 2 + 3,
                         x + x2 * width - 2, y + y2 * height / 2 - 3);
    }
}

/*****************
 * Draws one digit
 ****************/
void draw_7seg_digit(int digit, int x, int y, int width, int height, int lcd_display)
{
    int i;
    int c;

    for(i = 0; digit_segs[digit][i] >= 0; i++)
    {
        c = digit_segs[digit][i];

        if(!lcd_display)
            draw_seg_led(c, x, y, width, height);
        else
            draw_seg_lcd(c, x, y, width, height);
    }
}

/*****************************************************
 * Draws the entire 7-segment hour-minute time display
 ****************************************************/
void draw_7seg_time(int hour, int minute, int x, int y, int width, int height,
int colon, int lcd)
{
    int xpos = x;

    /* Now change to 12H mode if requested */
    if(settings.clock == MODE_DIGITAL)
    {
        if(settings.digital_12h)
        {
            if(hour > 12)
                hour -= 12;
            if(hour == 0)
                hour = 12;
        }
    }
    else
    {
        if(settings.lcd_12h)
        {
            if(hour > 12)
                hour -= 12;
            if(hour == 0)
                hour = 12;
        }
    }

    draw_7seg_digit(hour / 10, xpos, y, width, height, lcd);
    xpos += width + 10;
    draw_7seg_digit(hour % 10, xpos, y, width, height, lcd);
    xpos += width + 10;

    if(colon)
    {
        cops->drawLine(COLOR_WHITE, xpos, y + height/3 + 2,
                         xpos, y + height/3 + 2);
        cops->drawLine(COLOR_WHITE, xpos+1, y + height/3 + 1,
                         xpos+1, y + height/3 + 3);
        cops->drawLine(COLOR_WHITE, xpos+2, y + height/3,
                         xpos+2, y + height/3 + 4);
        cops->drawLine(COLOR_WHITE, xpos+3, y + height/3 + 1,
                         xpos+3, y + height/3 + 3);
        cops->drawLine(COLOR_WHITE, xpos+4, y + height/3 + 2,
                         xpos+4, y + height/3 + 2);

        cops->drawLine(COLOR_WHITE, xpos, y + height-height/3 + 2,
                         xpos, y + height-height/3 + 2);
        cops->drawLine(COLOR_WHITE, xpos+1, y + height-height/3 + 1,
                         xpos+1, y + height-height/3 + 3);
        cops->drawLine(COLOR_WHITE, xpos+2, y + height-height/3,
                         xpos+2, y + height-height/3 + 4);
        cops->drawLine(COLOR_WHITE, xpos+3, y + height-height/3 + 1,
                         xpos+3, y + height-height/3 + 3);
        cops->drawLine(COLOR_WHITE, xpos+4, y + height-height/3 + 2,
                         xpos+4, y + height-height/3 + 2);
    }

    xpos += 12;

    draw_7seg_digit(minute / 10, xpos, y, width, height, lcd);
    xpos += width + 10;
    draw_7seg_digit(minute % 10, xpos, y, width, height, lcd);
    xpos += width + 10;
}