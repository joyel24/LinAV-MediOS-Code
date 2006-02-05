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
#include <kernel/kernel.h>
#include <sys_def/string.h>

#include <kernel/graphics.h>
#include <kernel/evt.h>

#include <sys_def/colordef.h>
#include <gui/file_browser.h>
#include <file_type.h>

#ifdef GMINI4XX
#define LCD_WIDTH 220
#define LCD_HEIGHT 176
#endif

#ifdef AV3XX
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#endif

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
    
#ifdef GMINI4XX
    nb_disp_entry   : 14,
#endif
#ifdef AV3XX
    nb_disp_entry   : 20,
#endif

    x_start         : 0,
    y_start         : 18,
    
    width           : LCD_WIDTH,
    height          : LCD_HEIGHT-38,
    entry_height    : 10,
    
    draw_bottom_status : draw_bottom_status,
    draw_file_size     : draw_file_size,
    clear_status       : clear_status
};

struct browser_data * bdata;
int evt_handler;

void printList(struct browser_data * bdata,int val)
{
    int i;
    printk("%d: Files:%d Dir:%d Size:%d ListUsed:%d\n",val,bdata->nbFile,bdata->nbDir,bdata->totSize,bdata->listused);
    printk("liste @:%x\n",(int)bdata->list);
    for(i=0;i<bdata->listused;i++)
        printk("%d: (%s) %s\n",i,bdata->list[i].type==TYPE_FILE?"F":"D",bdata->list[i].name);
    printk("---------------------------- %d\n",val);
}

void ini_file_browser(void)
{
    bdata = &realData;
    evt_handler = evt_getHandler(ALL_CLASS);
    iniBrowser();
}

char res[PATHLEN];

char * browse(char * path,int mode)
{    
    int pos=0;
      
    if(!viewNewDir(bdata,path))
        return NULL;
    
    bdata->mode = mode;
        
    browserEvt(bdata);
    
    if(bdata->mode)
    {    
        sprintf(res,"%s/%s",bdata->path,nxtSelect(bdata,&pos)->name);
        return res;
    }
    
    return NULL;
}

int x=LCD_WIDTH;

void draw_file_size(struct dir_entry * entry)
{
    int h,w;
    char tmpS[15];
    
    /* erase previsous drawing */

    gfx_fillRect(COLOR_WHITE,x, LCD_HEIGHT-10,LCD_WIDTH-x,10);
    if(entry->type == TYPE_FILE)
    {
        createSizeString(tmpS,entry->size);
        gfx_getStringSize(tmpS,&w,&h);
        x=LCD_WIDTH-w;
        gfx_putS(COLOR_BLUE, COLOR_WHITE,x, LCD_HEIGHT-10, tmpS);
    }
}

void draw_bottom_status(struct browser_data *bdata)
{
    char tmp[100];
    char tmpS[15];

    //int len=0;   
    
    createSizeString(tmpS,bdata->totSize);
        
    gfx_fillRect(COLOR_WHITE,2, LCD_HEIGHT-20,LCD_WIDTH-4,20);
          
    gfx_putS(COLOR_BLUE, COLOR_WHITE,2, LCD_HEIGHT-20,bdata->path);  
    

    snprintf(tmp,100,"%d %s, %d %s, %s",bdata->nbFile,bdata->nbFile>0?"files":"file",
            bdata->nbDir,bdata->nbDir>0?"folders":"folders",tmpS);
    printk("%s\n",tmp);
    
    gfx_putS(COLOR_BLUE, COLOR_WHITE,2, LCD_HEIGHT-10, tmp);    
}

void clear_status(struct browser_data *bdata)
{
    gfx_fillRect(COLOR_WHITE,2, LCD_HEIGHT-20,LCD_WIDTH-24,20);
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
