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
    bdata->pos=0;
    bdata->nselect=0;
    hideArrow(DOWN_ARROW,bdata->nb_disp_entry);
    hideArrow(UP_ARROW,bdata->nb_disp_entry);
    if(bdata->listused>bdata->nb_disp_entry)
        showArrow(DOWN_ARROW,bdata->nb_disp_entry);
    printAllName(bdata);
    return 1;
}

void showArrow(int type,int max)
{
    int h=0,w=0;
    getStringS("M", &w, &h);

    switch(type)
    {
        case UP_ARROW:
            drawBITMAP(&upBitmap,310,h+MENU_SHADOW+1+6);
            break;
        case DOWN_ARROW:
            drawBITMAP(&dwBitmap,310,2+h+6+MENU_SHADOW+(max-1)*(h+1));
            break;
    }
}

void hideArrow(int type,int max)
{
    int h=0,w=0;
    getStringS("M", &w, &h);

    switch(type)
    {
        case UP_ARROW:
            fillRect(COLOR_WHITE,310,h+MENU_SHADOW+1+6,9,9);
            break;
        case DOWN_ARROW:
            fillRect(COLOR_WHITE,310,2+h+6+MENU_SHADOW+(max-1)*(h+1),9,9);
            break;
    }
}



int printName(struct dir_entry * dEntry,int x,int y,int clear,int selected)
{
    int             color;
    int             select_color;
    char *          cp;
    int             w = 0;
    int             h = 10;
    char *ext;

    getStringS("M", &w, &h);

    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        fillRect(COLOR_WHITE, 0, y , 320, h+1);

    switch(dEntry->type)
    {
        case TYPE_BACK:
            color=COLOR_BLUE;
            select_color=COLOR_RED;
            break;
        case TYPE_DIR:    
            color=COLOR_RED;
            select_color=COLOR_BLUE;
            drawBITMAP (&dirBitmap, 2, y);
            break;
        case TYPE_FILE:
            color=COLOR_BLACK;
            select_color=COLOR_BLUE;
            ext = strrchr(dEntry->name, '.');
            if (ext == 0)
            {
                // no extension
                fillRect(COLOR_WHITE, 2, y, 8, 8);
            }
            else if (is_mp3_type(ext))
                drawBITMAP (&mp3Bitmap, 2, y);
            else if(is_text_type(ext))
                drawBITMAP (&textBitmap, 2, y);
            else if(is_image_type(ext))
                drawBITMAP (&imageBitmap, 2, y);
            else
                fillRect(COLOR_WHITE, 2, y, 8, 8);
            break;
    }

    if(selected)
    {
        putS(color, select_color,x, y, dEntry->name);
        draw_file_size(dEntry);
    }
    else
        putS(color, COLOR_WHITE,x, y, dEntry->name);
    return 1;
}

void printAllName(struct browser_data *bdata)
{
    int w = 0;
    int h = 10;
    int i;
    
    int pos=bdata->pos;
    int nselect=bdata->nselect;

    getStringS("M", &w, &h);

    for (i = pos; i < bdata->listused && i < pos+bdata->nb_disp_entry; i++)
    {
        fillRect(COLOR_WHITE,0, 2+(i-pos)*(h+1)+ h+6+MENU_SHADOW , 320,(h+1));
        printName(&bdata->list[i],FILE_X_OFFSET, 2 + (i-pos)*(h+1)+ h+6+MENU_SHADOW,0,(i-pos)==nselect);
    }

    for(;i<pos+bdata->nb_disp_entry;i++)
        fillRect(COLOR_WHITE,0, (i-pos)*(h+1)+ h+7+MENU_SHADOW , 320,(h+1));
    
    draw_bottom_status(bdata);
}

void printAName(struct browser_data *bdata,int pos, int nselect, int clear, int selected)
{
    int w = 0;
    int h = 10;

    getStringS("M", &w, &h);

    printName(&bdata->list[pos],FILE_X_OFFSET,2 + nselect*(h+1)+ h+6+MENU_SHADOW,clear,selected);    
}

int x=320;

void draw_file_size(struct dir_entry * entry)
{
    int h,w;
    char tmpS[15];
    
    /* erase previsous drawing */

    fillRect(COLOR_WHITE,x, 230,320-x,10);
    if(entry->type == TYPE_FILE)
    {
        createSizeString(tmpS,entry->size);
        getStringS(tmpS,&w,&h);
        x=320-w;
        putS(COLOR_BLUE, COLOR_WHITE,x, 230, tmpS);
    }
}

void draw_bottom_status(struct browser_data *bdata)
{
    char tmp[100];
    char tmpS[15];
    char pwd[PATHLEN];
    int len=0;   
    
    createSizeString(tmpS,bdata->totSize);
        
    fillRect(COLOR_WHITE,2, 220,316,20);
        
    if (!getcwd(pwd, PATHLEN))
    {
        fprintf(stderr, "Cannot get current directory\n");        
    }
    else
    {
        len=strlen(pwd);        
        putS(COLOR_BLUE, COLOR_WHITE,2, 220, pwd);  
    }

    snprintf(tmp,100,"%d %s, %d %s, %s",bdata->nbFile,bdata->nbFile>0?"files":"file",
            bdata->nbDir,bdata->nbDir>0?"folders":"folders",tmpS);
    fprintf(stderr,"%s\n",tmp);
    
    putS(COLOR_BLUE, COLOR_WHITE,2, 230, tmp);    
}

void createSizeString(char * str,int Isize)
{
    char * unit;
    float size=Isize;
    if(str!=NULL)
    {
        if(size/1024>1)
        {
            size/=1024;
            unit="Kb";
            if(size/1024>1)
            {
                size/=1024;
                unit="Mb";
                if(size/1024>1)
                {
                    size/=1024;
                    unit="Gb";
                }
            }
        }
        else 
            unit = "b";
        sprintf(str,"%.02f %s",size,unit);
    }
}