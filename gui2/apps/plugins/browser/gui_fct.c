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

#include "ls_main.h"

#include "ls-gui-icons.h"

#define    FILE_X_OFFSET 10

/*extern variables */
extern struct client_operations * cops;
extern struct dir_entry * list;
extern int listused;
extern int pos,nselect;
extern int nbFile,nbDir,totSize;
/************************/

int viewNewDir(char *name)
{
    if(chdir(name)<0)
    {
        fprintf(stderr,"Error going in: %s\n",name);
        return 0;
    }
        
    cleanList();
    if(doLs("./")<0)
    {
        listused = 0;
        return 0;
    }
    pos=0;
    nselect=0;
    hideArrow(DOWN_ARROW);
    hideArrow(UP_ARROW);
    if(listused>MAXPOS)
        showArrow(DOWN_ARROW);
    printAllName(pos,nselect);
    return 1;
}

void showArrow(int type)
{
    int h=0,w=0;
    cops->getStringS("M", &w, &h);

    switch(type)
    {
        case UP_ARROW:
            cops->drawBITMAP(&upBitmap,310,h+MENU_SHADOW+1+6);
            break;
        case DOWN_ARROW:
//            cops->drawBITMAP(&dwBitmap,310,SCREEN_HEIGHT-10);
            cops->drawBITMAP(&dwBitmap,310,2+h+6+MENU_SHADOW+(MAXPOS-1)*(h+1));
            break;
    }
}

void hideArrow(int type)
{
    int h=0,w=0;
    cops->getStringS("M", &w, &h);

    switch(type)
    {
        case UP_ARROW:
            cops->fillRect(COLOR_WHITE,310,h+MENU_SHADOW+1+6,9,9);
            break;
        case DOWN_ARROW:
//            cops->fillRect(COLOR_WHITE,310,SCREEN_HEIGHT-10,9,9);
            cops->fillRect(COLOR_WHITE,310,2+h+6+MENU_SHADOW+(MAXPOS-1)*(h+1),9,9);
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

   cops->getStringS("M", &w, &h);

    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        cops->fillRect(COLOR_WHITE, 0, y , 320, h+1);

    switch(dEntry->type)
    {
        case TYPE_BACK:
            color=COLOR_BLUE;
            select_color=COLOR_RED;
            break;
        case TYPE_DIR:    
            color=COLOR_RED;
            select_color=COLOR_BLUE;
            cops->drawBITMAP (&dirBitmap, 2, y);
            break;
        case TYPE_FILE:
            color=COLOR_BLACK;
            select_color=COLOR_BLUE;
            ext = strrchr(dEntry->name, '.');
            if (ext == 0)
            {
                // no extension
                cops->fillRect(COLOR_WHITE, 2, y, 8, 8);
            }
            else if (is_mp3_type(ext))
                cops->drawBITMAP (&mp3Bitmap, 2, y);
            else if(is_text_type(ext))
                cops->drawBITMAP (&textBitmap, 2, y);
            else if(is_image_type(ext))
                cops->drawBITMAP (&imageBitmap, 2, y);
            else
                cops->fillRect(COLOR_WHITE, 2, y, 8, 8);
            break;
    }

    if(selected)
    {
        cops->putS(color, select_color,x, y, dEntry->name);
        draw_file_size(dEntry);
    }
    else
        cops->putS(color, COLOR_WHITE,x, y, dEntry->name);
    return 1;
}

void printAllName(int pos,int nselect)
{
    int w = 0;
    int h = 10;
    int i;

    cops->getStringS("M", &w, &h);

    for (i = pos; i < listused && i < pos+MAXPOS; i++)
    {
        cops->fillRect(COLOR_WHITE,0, 2+(i-pos)*(h+1)+ h+6+MENU_SHADOW , 320,(h+1));
        printName(&list[i],FILE_X_OFFSET, 2 + (i-pos)*(h+1)+ h+6+MENU_SHADOW,0,(i-pos)==nselect);
    }

    for(;i<pos+MAXPOS;i++)
        cops->fillRect(COLOR_WHITE,0, (i-pos)*(h+1)+ h+7+MENU_SHADOW , 320,(h+1));
    
    draw_bottom_status();
}

void printAName(int pos, int nselect, int clear, int selected)
{
    int w = 0;
    int h = 10;

    cops->getStringS("M", &w, &h);

    printName(&list[pos],FILE_X_OFFSET,2 + nselect*(h+1)+ h+6+MENU_SHADOW,clear,selected);    
}

int x=320;

void draw_file_size(struct dir_entry * entry)
{
    int h,w;
    char tmpS[15];
    
    /* erase previsous drawing */

    cops->fillRect(COLOR_WHITE,x, 230,320-x,10);
    if(entry->type == TYPE_FILE)
    {
        createSizeString(tmpS,entry->size);
        cops->getStringS(tmpS,&w,&h);
        x=320-w;
        cops->putS(COLOR_BLUE, COLOR_WHITE,x, 230, tmpS);
    }
}

void draw_bottom_status(void)
{
    char tmp[100];
    char tmpS[15];
    char pwd[PATHLEN];
    int len=0;   
    
    createSizeString(tmpS,totSize);
        
    cops->fillRect(COLOR_WHITE,2, 220,316,20);
        
    if (!getcwd(pwd, PATHLEN))
    {
        fprintf(stderr, "Cannot get current directory\n");        
    }
    else
    {
        len=strlen(pwd);        
        cops->putS(COLOR_BLUE, COLOR_WHITE,2, 220, pwd);  
    }

    snprintf(tmp,100,"%d %s, %d %s, %s",nbFile,nbFile>0?"files":"file",nbDir,nbDir>0?"folders":"folders",tmpS);
    fprintf(stderr,"%s\n",tmp);
    
    cops->putS(COLOR_BLUE, COLOR_WHITE,2, 230, tmp);    
}