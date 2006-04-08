/*
* kernel/gui/internal_plugin/gui_fct.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/graphics.h>
#include <kernel/evt.h>
#include <kernel/errors.h>

#include <gui/icons.h>
#include <gui/file_browser.h>
#include <gui/scrollbar.h>
#include <file_type.h>

BITMAP * gui_ls_upBitmap;
BITMAP * gui_ls_dwBitmap;
BITMAP * gui_ls_dirBitmap;
BITMAP * gui_ls_mp3Bitmap;
BITMAP * gui_ls_textBitmap;
BITMAP * gui_ls_imageBitmap;

//#include "browser_icons.h"

#define    FILE_X_OFFSET 10

struct scroll_bar browser_scroll = {
    border_color : COLOR_BLACK,
    fg_color     : COLOR_BLUE,
    bg_color     : COLOR_WHITE,
    orientation  : VERTICAL
};

int evt_handler;

void iniBrowser(void)
{   
    gui_ls_upBitmap=&getIcon("upBitmap")->bmap_data;
    gui_ls_dwBitmap=&getIcon("dwBitmap")->bmap_data;
    gui_ls_dirBitmap=&getIcon("dirBitmap")->bmap_data;
    
    if((evt_handler=evt_getHandler(ALL_CLASS))<0)
    {
        printk("Can't get evt handler (error:%d)\n",-evt_handler);
        evt_handler=-1;        
    }
    gui_ls_mp3Bitmap=&getIcon("mp3Bitmap")->bmap_data;
    gui_ls_textBitmap=&getIcon("textBitmap")->bmap_data;
    gui_ls_imageBitmap=&getIcon("imageBitmap")->bmap_data;
}

int viewNewDir(struct browser_data *bdata,char *name)
{
    
    if(name!=NULL)
    {
        if(strlen(name)>PATHLEN)
        {
           // msgBox("ERROR - Browser", "Can't load dir, path too long", MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR);
            return 0;
        }
        strcpy(bdata->path,name);
    }
        

    cleanList(bdata);
    printk("before doLS\n");
    if(!doLs(bdata))
    {
        cleanList(bdata);
        //msgBox("ERROR - Browser", "Can't load dir", MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR);
        return 0;
    }

    gfx_fontSet(bdata->font);
    
    gfx_fillRect(COLOR_WHITE,bdata->x_start,bdata->y_start,bdata->width,bdata->height);

    browser_scroll.x=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?1:bdata->width-10);
    browser_scroll.y=bdata->y_start;
    browser_scroll.width=8;    
    browser_scroll.height=bdata->height;
    
    bdata->pos=0;
    bdata->nselect=0;
    redrawBrowser(bdata);
    
    return 1;
}

int browser_browse(struct browser_data *bdata,char * path,char * res)
{
    if(viewNewDir(bdata,path))
    {        
        if(browserEvt(bdata)==MED_OK)
        {
            if(bdata->mode==MODE_STRING && res)
            {
                if(bdata->path[0]=='/' && bdata->path[1]=='\0')
                    sprintf(res,"/%s",bdata->list[bdata->pos+bdata->nselect].name);
                else
                    sprintf(res,"%s/%s",bdata->path,bdata->list[bdata->pos+bdata->nselect].name);
            }
            return MED_OK;
        }
        else
        {
            return -MED_ERROR;
        }
    }
    return -MED_ERROR;
}

int browser_simpleBrowse(char * path,char * res)
{
    int ret_val;
    struct browser_data * browseData=browser_NewBrowse();
    if(!browseData)
        return -MED_ENOMEM;
    browseData->mode=MODE_STRING;
    ret_val=browser_browse(browseData,path,res);
    browser_disposeBrowse(browseData);
    return ret_val;
}

void redrawBrowser(struct browser_data *bdata)
{
    printAllName(bdata);
    draw_scrollBar(&browser_scroll, bdata->listused, bdata->pos,bdata->nb_disp_entry+bdata->pos);
}

void clearBrowser(struct browser_data *bdata)
{
    cleanList(bdata);
    gfx_fillRect(COLOR_WHITE,bdata->x_start,bdata->y_start,bdata->width,bdata->height);
    if(bdata->clear_status)
        bdata->clear_status(bdata);
}

void printName(struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata)
{
    int             color=COLOR_BLACK;
    int             select_color=COLOR_BLUE;
    char *          cp;
#ifdef GMINI4XX
    char						trimmed_filename[34];
    int							i;
#endif
    int             type;
    int             H=bdata->entry_height;
    int             X=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?10:0);
    int             Y=bdata->y_start+pos*H;
    int             W=bdata->width-10;

    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        gfx_fillRect(COLOR_WHITE, X, Y , W, H);

    switch(dEntry->type)
    {
        case TYPE_BACK:
            color=COLOR_BLUE;
            select_color=COLOR_RED;
            break;
        case TYPE_DIR:
            color=COLOR_RED;
            select_color=COLOR_BLUE;
            gfx_drawBitmap(gui_ls_dirBitmap, X+2, Y);
            break;
        case TYPE_FILE:
            color=COLOR_BLACK;
            select_color=COLOR_BLUE;
            
            type=get_file_type(dEntry->name);
            switch(type)
            {
                case IMG_TYPE:
                    gfx_drawBitmap(gui_ls_imageBitmap, X+2, Y);
                    break;
                case MP3_TYPE:
                    gfx_drawBitmap(gui_ls_mp3Bitmap, X+2, Y);
                    break;
                case TXT_TYPE:
                    gfx_drawBitmap(gui_ls_textBitmap, X+2, Y);
                    break;
                case BIN_TYPE:
                case UKN_TYPE:
                default:
                    gfx_fillRect(COLOR_WHITE, X+2, Y, 8, 8); /* no icon */
                    break;
            }
            break;
    }

    if(selected)
    {
        if(dEntry->selected)
            select_color=COLOR_ORANGE2;
        if(bdata->draw_file_size)
            bdata->draw_file_size(dEntry);
    }
    else
    {
        if(dEntry->selected)
            select_color=COLOR_ORANGE;
        else
            select_color= COLOR_WHITE;
    }
#ifdef GMINI4XX
    for(i=0;i<=32;i++)
    {
    	trimmed_filename[i]=dEntry->name[i];
    }
    trimmed_filename[33]='\0';
    gfx_putS(color, select_color,X+11, Y, trimmed_filename);
#endif
#ifdef AV3XX
    gfx_putS(color, select_color,X+11, Y, dEntry->name);
#endif
}

void printAllName(struct browser_data *bdata)
{
    int i;
    int pos=bdata->pos;
    int nselect=bdata->nselect;
    int H=bdata->entry_height;
    int X=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?10:0);
    int Y=bdata->y_start+pos*H;
    int W=bdata->width-10;

    for (i = pos; i < bdata->listused && i < pos+bdata->nb_disp_entry; i++)
        printName(&bdata->list[i], i-pos,1,(i-pos)==nselect,bdata);

    /*gfx_fillRect(COLOR_WHITE,X, Y+(i-pos)*H, W,H);
    We should replace this with one call to gfx_fillRect !! */
    for(;i<pos+bdata->nb_disp_entry;i++)
        gfx_fillRect(COLOR_WHITE,X, Y+(i-pos)*H, W,H);
    if(bdata->draw_bottom_status)
        bdata->draw_bottom_status(bdata);
}

void printAName(struct browser_data *bdata,int pos, int nselect, int clear, int selected)
{
    printName(&bdata->list[pos],nselect,clear,selected,bdata);
}

