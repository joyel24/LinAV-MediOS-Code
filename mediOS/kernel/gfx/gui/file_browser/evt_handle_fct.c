/*
* kernel/gui/internal_plugin/evt_handle_fct.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <graphics.h>
#include <evt.h>
#include <api.h>
#include <sys_def/colordef.h>

#include <gui/file_browser.h>
#include <gui/scrollbar.h>

extern struct scroll_bar browser_scroll;
extern int evt_handler;

void browserEvt(struct browser_data * bdata)
{
    int w = 0;
    int h = 10;
    
    int stop=0;;
    char evt=0;
    getStringS("M", &w, &h);

    while(!stop)
    {
        evt = evt_get(evt_handler);
        if(!evt)
            continue;
            
        switch(evt)
        {
            case BTN_ON:                
                chgSelect(bdata,bdata->pos+bdata->nselect);
                if(bdata->mode)
                    stop=1;
                break;
            case BTN_UP:
                bdata->nselect--;
    
                if(bdata->nselect<0)
                {
                    bdata->nselect=0;
                    bdata->pos--;
                    if(bdata->pos<0) // we are at the beg => do wrapping
                    {
                        bdata->pos=bdata->listused-bdata->nb_disp_entry;//-1;
                        if(bdata->pos<0)
                        {
                            bdata->pos=0;
                            printAName(bdata,0,0,0,0);
                            bdata->nselect=bdata->listused-1;
                            printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                        }
                        else
                        {
                            bdata->nselect=bdata->listused-bdata->pos-1;
                            printAllName(bdata);
                        }
                    }
                    else // not going up, scrolling
                    {
                        scrollWindowVert(COLOR_WHITE, bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?10:0), bdata->y_start,
                                    bdata->width-10, (h+1)*bdata->nb_disp_entry, h+1,0);
                        printAName(bdata,bdata->pos+bdata->nselect+1,bdata->nselect+1,1,0);
                        printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,1,1);
                    }
    
                    draw_scrollBar(&browser_scroll, bdata->listused, bdata->pos,bdata->nb_disp_entry+bdata->pos);
                }
                else // just going up
                {
                    printAName(bdata,bdata->pos+bdata->nselect+1,bdata->nselect+1,0,0);
                    printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                }
    
                break;
    
            case BTN_DOWN:
                bdata->nselect++;
    
                if(bdata->nselect+bdata->pos>=bdata->listused)       // jump to beginning
                {
                    if(bdata->listused<=bdata->nb_disp_entry)
                    {
                        printAName(bdata,bdata->pos+bdata->nselect-1,bdata->nselect-1,0,0);
                        bdata->pos=0;
                        bdata->nselect=0;
                        printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                    }
                    else
                    {
                        bdata->pos=0;
                        bdata->nselect=0;
                        printAllName(bdata);
                    }
    
                    draw_scrollBar(&browser_scroll, bdata->listused, bdata->pos,bdata->nb_disp_entry+bdata->pos);
                }
                else
                {
                    if(bdata->nselect>=bdata->nb_disp_entry)
                    {
                        bdata->nselect=bdata->nb_disp_entry-1;
                        bdata->pos++;
                        if(bdata->pos+bdata->nb_disp_entry>bdata->listused) // we are at the end => do wrapping
                        {
                            bdata->pos=0;
                            bdata->nselect=0;
                            printAllName(bdata);
                        }
                        else // not going down, scrolling
                        {
                            scrollWindowVert(COLOR_WHITE, bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?10:0), bdata->y_start,
                                    bdata->width-10, (h+1)*bdata->nb_disp_entry, h+1,1);
                            printAName(bdata,bdata->pos+bdata->nselect-1,bdata->nselect-1,1,0);
                            printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,1,1);
                        }
    
                        draw_scrollBar(&browser_scroll, bdata->listused, bdata->pos,bdata->nb_disp_entry+bdata->pos);
                    }
                    else
                    {
                        printAName(bdata,bdata->pos+bdata->nselect-1,bdata->nselect-1,0,0);
                        printAName(bdata,bdata->pos+bdata->nselect,bdata->nselect,0,1);
                    }
                }
                break;
    
            case BTN_RIGHT:
                switch(bdata->list[bdata->pos+bdata->nselect].type)
                {
                    case TYPE_BACK:
                        upDir(bdata);
                        if(!viewNewDir(bdata,NULL))
                        {
                            stop=1;
                            break;
                        }
                        break;
                    case TYPE_DIR:
                        inDir(bdata,bdata->list[bdata->pos+bdata->nselect].name);
                        if(!viewNewDir(bdata,NULL))
                        {
                            stop=1;
                            break;
                        }
                        break;
                    case TYPE_FILE:
                        /*sprintf(path,"%s%s",bdata->path,bdata->list[bdata->pos+bdata->nselect].name);*/
                        /*handle_type_other(path);                        */
                        //printk("launching %s\n",path);
                        break;
                }
                break;
            case BTN_LEFT:
                if(upDir(bdata))
                {              
                    if(!viewNewDir(bdata,NULL))
                    {
                        stop=1;
                        break;
                    }
                }
                break;
#ifdef GMINI4XX
            case BTN_1:
                switch(bdata->list[bdata->pos+bdata->nselect].type)
                {
                    case TYPE_BACK:
                        upDir(bdata);
                        if(!viewNewDir(bdata,NULL))
                        {
                            stop=1;
                            break;
                        }
                        break;
                    case TYPE_DIR:
                        inDir(bdata,bdata->list[bdata->pos+bdata->nselect].name);
                        if(!viewNewDir(bdata,NULL))
                        {
                            stop=1;
                            break;
                        }
                        break;
                    case TYPE_FILE:
												chgSelect(bdata,bdata->pos+bdata->nselect);
												if(bdata->mode)
														stop=1;
                        break;
                }
                break;
            case BTN_2:
                if(upDir(bdata))
                {              
                    if(!viewNewDir(bdata,NULL))
                    {
                        stop=1;
                        break;
                    }
                }
                break;
#endif
        }
    }
}
