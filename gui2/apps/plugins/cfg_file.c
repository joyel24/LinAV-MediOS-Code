/*
* cfg_file.c
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

#include "stdlib.h"
#include "stdio.h"

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"

struct client_operations * cops;

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

char tmp[250];

int var1,var2;
char str1[MAX_TOKEN+1];
int mode;

int doParse()
{
    char *item=item_buff;
    char *value=value_buff;
    
    while (1)
    {
        /* get next item/value couple, returns 0 if ther is no more item/value in the file */
        if (!cops->getCfg(item,value)) break;
        /* now parse the item using strcmp */
        if(!strcmp(item,"v1"))
        {
            var1=atoi(value);
        }
        else if(!strcmp(item,"v2"))
        {
            var2=atoi(value);    
        }
        else if(!strcmp(item,"str"))
        {
            strcpy(str1,value);
        }
        else
            fprintf(stderr,"unknown item type: %s on line %d\n",item,cops->curLineNum());
    }
}

int doSave(void)
{
    cops->putComment("************** Demo cfg *****************");
    cops->newLine();
    cops->newLine();
    cops->putComment("First variable");
    sprintf(tmp,"%d",var1);
    cops->putCfg("v1",tmp);
    cops->newLine();
    cops->putComment("Second variable");
    sprintf(tmp,"%d",var2);
    cops->putCfg("v2",tmp);
    cops->newLine();
    cops->newLine();
    cops->putComment("A string");
    cops->putCfg("str",str1);
    cops->putComment("Should have quote to protect the string");
    cops->newLine();
    cops->newLine();
    cops->putComment("************** End *****************");
}

int eventHandler(int evt)
{
    switch (evt) {
        case BTN_OFF:
        case EVT_QUIT:
            /* get out of here */
            RELEASE(cops)
            break;
        case BTN_F1:
            if(mode==1)
            {
                cops->fillRect(COLOR_WHITE,5,50,315,120);
                sprintf(tmp,"v1=%d,v2=%d,s=%s",var1,var2,str1);
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,50,tmp);
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,60,"Press F1 for nxt step");
                mode=2;            
            }
            else if(mode==2)
            {
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,70,"Changing vars");
                var1+=5;
                var2+=10;
                strcpy(str1,"hello world");
                sprintf(tmp,"v1=%d,v2=%d,s=%s",var1,var2,str1);
                 cops->putS(COLOR_BLACK,COLOR_WHITE,5,80,tmp);
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,90,"Press F1 for nxt step");
                mode=3;
            }            
            else if(mode==3)
            {
                cops->fillRect(COLOR_WHITE,5,50,315,120);
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,50,"Saving to cfg file");
                cops->openCfg("/mnt/avwm/plugins/cfg_tst.cfg",CFG_WRITE);
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,60,"File open...");
                doSave();
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,70,"Cfg written...");
                cops->closeCfg();
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,80,"File closed...");
                cops->putS(COLOR_BLACK,COLOR_WHITE,5,90,"Press OFF to stop");
                mode=4;
            }
            break;
    }
}

/* called function from outside */
int main(int argc,char * * argv)
{
    REGISTER(cops,eventHandler,0);
    
    cops->clearScreen(COLOR_WHITE);

    cops->putS(COLOR_BLACK,COLOR_WHITE,5,50,"Cfg file demo");    
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,60,"Reading file...");
    
    cops->openCfg("/mnt/avwm/plugins/cfg_tst.cfg",CFG_READ);
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,70,"File open...");
    
    doParse();
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,80,"File parsed...");
    
    cops->closeCfg();
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,90,"File closed...");
    
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,100,"Press F1 for nxt step");
    mode=1;

    PACK(cops,NULL)
    
    STOPME(cops)

    return ;
}

