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

/*extern variables */
extern int pos,nselect;
extern int listused;
/************************/

int main(int argc,char * * argv)
{
    REGISTER(cops,eventHandler,0);

    pos=0;
    nselect=0;

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

        if(doLs("./")<0)
        {
            cleanList();
            return -1;
        }        

        printAllName(pos,nselect);

        if(listused>MAXPOS)
            showArrow(DOWN_ARROW);

        PACK(cops,NULL);

        cleanList();
        return 0;
    }
    STOPME(cops)
    return -1;
}
