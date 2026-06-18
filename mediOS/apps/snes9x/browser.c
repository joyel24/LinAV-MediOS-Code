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

#include <kernel/lcd.h>
#include <kernel/graphics.h>
#include <kernel/evt.h>

#include <sys_def/colordef.h>
#include <gui/file_browser.h>
#include <file_type.h>

#define LCD_WIDTH SCREEN_REAL_WIDTH
#define LCD_HEIGHT (SCREEN_HEIGHT)

static struct browser_data realData = {
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

static struct browser_data * bdata;
int evt_handler;

void spc_ini_file_browser(char * path,int mode)
{
    bdata = &realData;
    evt_handler = evt_getHandler(ALL_CLASS);
    iniBrowser();

    if(!viewNewDir(bdata,path))
        return NULL;
    
    bdata->mode = mode;
        
}

// dirty
static char res[PATHLEN];

char * spc_browse()
{    
    int pos=0;

    gfx_fillRect(COLOR_WHITE, 0, 10, LCD_WIDTH, LCD_HEIGHT - 10);
    redrawBrowser(bdata);
    bdata->draw_bottom_status(bdata);
    browserEvt(bdata);
    
    if(bdata->mode)
    {    
        sprintf(res,"%s/%s",bdata->path,nxtSelect(bdata,&pos)->name);
	chgSelect(bdata, pos-1);
        return res;
    }
    
    return NULL;
}

