/*
* gui_fct.c
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

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "browser.h"
#include "graphics.h"
#include "colordef.h"
#include "icons.h"
#include "file_type.h"

NEED_ICON(upBitmap)
NEED_ICON(dwBitmap)
NEED_ICON(dirBitmap)
NEED_ICON(mp3Bitmap)
NEED_ICON(textBitmap)
NEED_ICON(imageBitmap)


//#include "browser_icons.h"

#define    FILE_X_OFFSET 10

int viewNewDir(struct browser_data *bdata,char *name)
{
    if(chdir(name)<0)
    {
        fprintf(stderr,"Error going in: %s\n",name);
        return 0;
    }
        
    cleanList(bdata);
    if(doLs(bdata,"./")<0)
    {
        bdata->listused = 0;
        return 0;
    }
    
    fillRect(COLOR_WHITE,bdata->x_start,bdata->y_start,bdata->width,bdata->height);
    
    bdata->pos=0;
    bdata->nselect=0;
    hideArrow(DOWN_ARROW,bdata);
    hideArrow(UP_ARROW,bdata);
    if(bdata->listused>bdata->nb_disp_entry)
        showArrow(DOWN_ARROW,bdata);
    printAllName(bdata);
    return 1;
}

void showArrow(int type,struct browser_data *bdata)
{
    int H=bdata->entry_height;
    
    switch(type)
    {
        case UP_ARROW:
            drawBITMAP(&upBitmap,bdata->width-9,bdata->y_start);
            break;
        case DOWN_ARROW:
            drawBITMAP(&dwBitmap,bdata->width-9,bdata->y_start+(bdata->nb_disp_entry-1)*H);
            break;
    }
}

void hideArrow(int type,struct browser_data *bdata)
{
    int H=bdata->entry_height;

    switch(type)
    {
        case UP_ARROW:
            fillRect(COLOR_WHITE,bdata->width-9,bdata->y_start,9,9);
            break;
        case DOWN_ARROW:
            fillRect(COLOR_WHITE,bdata->width-9,bdata->y_start+(bdata->nb_disp_entry-1)*H,9,9);
            break;
    }
}

int printName(struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata)
{
    int             color;
    int             select_color;
    char *          cp;
    int             type;
    int             H=bdata->entry_height;
    int             X=bdata->x_start;
    int             Y=bdata->y_start+pos*H;
    int             W=bdata->width-10;

    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        fillRect(COLOR_WHITE, X, Y , W, H);

    switch(dEntry->type)
    {
        case TYPE_BACK:
            color=COLOR_BLUE;
            select_color=COLOR_RED;
            break;
        case TYPE_DIR:    
            color=COLOR_RED;
            select_color=COLOR_BLUE;
            drawBITMAP (&dirBitmap, X+2, Y);
            break;
        case TYPE_FILE:
            color=COLOR_BLACK;
            select_color=COLOR_BLUE;
            type=get_file_type(dEntry->name);
            switch(type)
            {                
                case IMG_TYPE:
                    drawBITMAP (&imageBitmap, X+2, Y);
                    break;
                case MP3_TYPE:
                    drawBITMAP (&mp3Bitmap, X+2, Y);
                    break;
                case TXT_TYPE:
                    drawBITMAP (&textBitmap, X+2, Y);
                    break; 
                case BIN_TYPE:
                case SCRIPT_TYPE:
                case UKN_TYPE:
                default: 
                    fillRect(COLOR_WHITE, X+2, Y, 8, 8); /* no icon */
                    break;               
            }
            break;
    }

    if(selected)
    {
        if(dEntry->selected)
            select_color=COLOR_ORANGE2;            
        putS(color, select_color,X+11, Y, dEntry->name);
        bdata->draw_file_size(dEntry);
    }
    else
    {
        if(dEntry->selected)
            select_color=COLOR_ORANGE;
        else
            select_color= COLOR_WHITE; 
        putS(color, select_color,X+11, Y, dEntry->name);
    }
    return 1;
}

void printAllName(struct browser_data *bdata)
{
    int i;
    int pos=bdata->pos;
    int nselect=bdata->nselect;
    int H=bdata->entry_height;
    int X=bdata->x_start;
    int Y=bdata->y_start+pos*H;
    int W=bdata->width-10;

    for (i = pos; i < bdata->listused && i < pos+bdata->nb_disp_entry; i++)
        printName(&bdata->list[i], i-pos,1,(i-pos)==nselect,bdata);

    /*fillRect(COLOR_WHITE,X, Y+(i-pos)*H, W,H);
    We should replace this with one call to fillRect !! */
    for(;i<pos+bdata->nb_disp_entry;i++)
        fillRect(COLOR_WHITE,X, Y+(i-pos)*H, W,H);
    
    bdata->draw_bottom_status(bdata);
}

void printAName(struct browser_data *bdata,int pos, int nselect, int clear, int selected)
{
    printName(&bdata->list[pos],nselect,clear,selected,bdata);    
}

