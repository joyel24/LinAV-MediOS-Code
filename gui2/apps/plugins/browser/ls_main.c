/*
* ls-gui.c
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
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "ls_main.h"
#include "helperMenu.h"

struct client_operations * cops;

struct helperMenu browserMenu = {
    ON_txt        : "Select",
    OFF_txt       : "Quit",
    JOY_txt       : "Nav/Sel",
    F1_txt        : "HD",
    F2_txt        : "CF",
    F3_txt        : "Menu",
    
    helperDelay   :  1,
    helperSpeed   :  300,
    
    bg_color      : COLOR_WHITE,
    border_color  : COLOR_BLUE,
    txt_color     : COLOR_BLACK,
    
    align         : ALIGN_RIGHT
};

struct browser_data * bdata;

struct browser_data realData = {
    path            : NULL,
    
    list            : NULL,
    listused        : 0,
    listsize        : 0,
    
    pos             : 0,
    nselect         : 0,
   
    show_dot_files  : 1,

    nbFile          : 0,
    nbDir           : 0,
    totSize         : 0,
    
    nb_disp_entry   : 20,
    x_start         : 0,
    y_start         : 0,
    
    width           :0
};

int main(int argc,char * * argv)
{
    REGISTER(cops,eventHandler,0);

    /*pos=0;
    nselect=0;*/
    bdata = &realData;

    if(argc>0)
    {
        cops->disableMenu();
        cops->setFont(STD6X9);

        ini_menu_struct();
        drawhelperMenuBox(&browserMenu);

        if(strlen(argv[1]) == 1)
        {
            if(argv[1][0] == '/')
                chdir("./"); // handle spezial case if path only / without dot
            else
                chdir(argv[1]);
        }
        else
            chdir(argv[1]);

        viewNewDir(bdata,"./");       

        PACK(cops,NULL);

        cleanList(bdata);
        return 0;
    }
    STOPME(cops)
    return -1;
}
