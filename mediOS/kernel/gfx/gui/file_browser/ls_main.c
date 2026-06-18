/*
* kernel/gfx/gui/file_browser/ls_main.c
*
*   mediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>


#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/lcd.h>
#include <kernel/graphics.h>
#include <kernel/evt.h>


#include <gui/file_browser.h>

#include <file_type.h>

struct browser_data realData = {
    path            : "/",

    list            : NULL,
    listused        : 0,
    listsize        : 0,
    
    scroll_pos      : LEFT_SCROLL,
    
    pos             : 0,
    nselect         : 0,
   
    show_dot_files  : 1,

    nbFile          : 0,
    nbDir           : 0,
    totSize         : 0,
    nb_disp_entry   : -1, // will be computed
    max_entry_length: -1,


    x_start         : 0,
    y_start         : 0,
    
    font            : STD6X9,

    width           : LCD_WIDTH,
    height          : LCD_HEIGHT,


    entry_height    : 10,

    draw_bottom_status : draw_bottom_status,
    draw_file_size     : draw_file_size,
    clear_status       : clear_status
};

void printList(struct browser_data * bdata,int val)
{
    int i;
    printk("%d: Files:%d Dir:%d Size:%d ListUsed:%d\n",val,bdata->nbFile,bdata->nbDir,bdata->totSize,bdata->listused);
    printk("liste @:%x\n",(int)bdata->list);
    for(i=0;i<bdata->listused;i++)
        printk("%d: (%s) %s\n",i,bdata->list[i].type==TYPE_FILE?"F":"D",bdata->list[i].name);
    printk("---------------------------- %d\n",val);
}

struct browser_data * browser_NewBrowse(void)
{
    struct browser_data * ptr = (struct browser_data *)malloc(sizeof(struct browser_data));
    if(!ptr)
        return NULL;
    memcpy(ptr,&realData,sizeof(struct browser_data));
    
    return ptr;
}

void browser_setPath(char * path,struct browser_data *bdata)
{
    strcpy(bdata->path,path);
}

void browser_disposeBrowse(struct browser_data * bdata)
{
    cleanList(bdata);
    free(bdata);
}

int browser_oldFileSizeX=LCD_WIDTH;

void draw_file_size(struct browser_data *bdata, struct dir_entry * entry)
{
    int sh,sw;
    char tmpS[15];
    int bx,by;

    by=bdata->y_start+bdata->height;
    bx=bdata->x_start+bdata->width;

    /* erase previous drawing */

    gfx_fillRect(COLOR_WHITE,browser_oldFileSizeX, by-10,bx-browser_oldFileSizeX,10);
    if(entry->type == TYPE_FILE)
    {
        createSizeString(tmpS,entry->size);
        gfx_getStringSize(tmpS,&sw,&sh);
        browser_oldFileSizeX=bx-sw;
        gfx_putS(COLOR_BLUE, COLOR_WHITE,browser_oldFileSizeX, by-10, tmpS);
    }
}

void draw_bottom_status(struct browser_data *bdata)
{
    char tmp[100];
    char tmpS[15];
    int bx,by;

    by=bdata->y_start+bdata->height;
    bx=bdata->x_start+bdata->width;

    createSizeString(tmpS,bdata->totSize);

    gfx_fillRect(COLOR_WHITE,2, by-20,bx-4,20);

    gfx_putS(COLOR_BLUE, COLOR_WHITE,2, by-20,bdata->path);


#if defined(GMINI4XX) || defined(AV1XX) || defined(JBMM) || defined(GMINI402)
    snprintf(tmp,100,"%d %s, %s",bdata->nbFile,bdata->nbFile>1?"files":"file",tmpS);
#else
    snprintf(tmp,100,"%d %s, %d %s, %s",bdata->nbFile,bdata->nbFile>1?"files":"file",
            bdata->nbDir,bdata->nbDir>1?"folders":"folder",tmpS);
#endif

    gfx_putS(COLOR_BLUE, COLOR_WHITE,2, by-10, tmp);
}

void clear_status(struct browser_data *bdata)
{
    int bx,by;

    by=bdata->y_start+bdata->height;
    bx=bdata->x_start+bdata->width;

    gfx_fillRect(COLOR_WHITE,2, by-20,bx-24,20);
}

void createSizeString(char * str,int Isize)
{
    char * unit;
    int a,b;
    int tmp=Isize;
    if(str!=NULL)
    {
        if((tmp=(tmp>>10))!=0)
        {
            if((tmp=(tmp>>10))!=0)
            {
                Isize=Isize>>10;                
                if((tmp=(tmp>>10))!=0)
                {
                    Isize=Isize>>10;
                    tmp=(Isize*100)>>10;
                    a=Isize>>10;
                    b=tmp-a*100;
                    unit="Gb";
                }
                else
                {
                    tmp=(Isize*100)>>10;
                    a=Isize>>10;
                    b=tmp-a*100;
                    unit="Mb";
                }
            }
            else
            {
                tmp=(Isize*100)>>10;
                a=Isize>>10;
                b=tmp-a*100;           
                unit="Kb";            
            }
        }
        else
        {
            a=Isize;
            b=0;
            unit = "b";
        }
     
        sprintf(str,"%d.%d %s",a,b,unit);   
        
    }
}
