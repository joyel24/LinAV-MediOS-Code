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

/****************************************************************************
This is an example of config file reading/writting
to test this example you sould put a config file in /mnt/avwm/plugins/ and
called cfg_tst.cfg. It should contain something like this:
v1=3
v2=7
str=hello
At the end of the programme, the content of this file will be updated
****************************************************************************/


/****************************************************************************
in order to use the config file reader/writer you first need to add 2 
char buffers with a size of MAX_TOKEN+1
you'll also need to code 2 functions, one for reading the other for writting
****************************************************************************/

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

char tmp[250];

int var1,var2;
char str1[MAX_TOKEN+1];
int mode;

/****************************************************************************
this function is used for reading the config file
it's an infinite loop
in the loop it ask the config reader to give the next item/value couple
if the reader returns 0, this means no more couple left in the file
if this happens , we leave the loop
otherwise, we go through several tests to see which item we have
and save the corresponding value in the right variable
****************************************************************************/
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

/****************************************************************************
this function is used to write the config file
3 functions are available for this:
putCfg => it takes a item/value couple and save it in the file, if the value
is a string and has space in it, quotes are added so it can be read by the config reader
putComment => it takes one string and save it to the file, it adds the comment char at the begining
and a newline at the end
newLine => no param it only add a newline in the file
****************************************************************************/
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
/****************************************************************************
the save sequence should be:
call openCfg to open the file
call your writer, here it is called doSave
call closeCfg to close the file
if you forget to close the file, its content might not be saved
****************************************************************************/
            
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

/****************************************************************************
the read sequence should be:
call openCfg to open the file
call your reader, here it is called doParse
call closeCfg to close the file
you need to close the file even you know you'll need it once again 
to write nack the config
****************************************************************************/
    cops->openCfg("/mnt/avwm/plugins/cfg_tst.cfg",CFG_READ);
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,70,"File open...");
    
    doParse();
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,80,"File parsed...");
    
    cops->closeCfg();
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,90,"File closed...");
    
    cops->putS(COLOR_BLACK,COLOR_WHITE,5,100,"Press F1 for nxt step");
    mode=1;

    PACK(cops,NULL)
    

    return 1;
}

