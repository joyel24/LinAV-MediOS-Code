/*
*   loader/target/arch_AV1XX/gui.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <kernel.h>
#include <graphics.h>
#include <avlo.h>
#include <avlo_cfg.h>
#include <hardware.h>

extern struct avlo_cfg * ptr_cfg;
#include <colordef.h>
#include <avlo_colors.h>

#include <font.h>

/* load step from 0 to 3 */
void printIniLevel(int level)
{
    char * str="";
    switch(level)
    {
        case 0:
            str = "A";
            break;
        case 1:
            str = "AV";
            break;
        case 2:
            str = "AVL";
            break;
        case 3:
            str = "AVLO";
            break;    
    }
    printf("%s\n",str);
    setFont(TXT_FONT);
    getStringS(str,&w,&h);
    putS(COLOR_TXT,COLOR_TSP,0,0,str);
    putS(COLOR_TXT,COLOR_TSP,0,h+2,VERSION);
}

void HD_init_string(void)
{
    int x,y,h,w;
    char * str = "HD Init";
    printf("%s\n",str);
    setFont(BOX_FONT);
    getStringS(str,&w,&h);  
    drawBox(w,h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,str);
}

void readingConfString(void)
{
    int x,y,h,w;
    char * str = "Reading config";
    printf("%s\n",str);
    setFont(BOX_FONT);
    getStringS(str,&w,&h);
    drawBox(w,h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,str);
}

/*void USBEnableString(void)
{
    getStringS("USB MODE (F3)",&w,&h);
    drawBox(w,h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,"USB Enable, PRESS F3 to resume");
}*/

void USBEnableString(void)
{
    int x,y,h,w;
    char * str = "USB MODE (F3)";
    printf("%s\n",str);
    setFont(BOX_FONT);
    getStringS(str,&w,&h);
    drawBox(w,h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,str);
}

void drawProgress(int offset,int length,int mode)
{
    if(mode)
        fillRect(COLOR_LOAD,BAR_X+(BAR_W*offset)/length, BAR_Y, 1 , BAR_H);
    else
        fillRect(COLOR_LOAD,BAR_X,BAR_Y, (BAR_W*offset)/length+1 , BAR_H);
}

/*void drawProgress(int offset,int length,int mode)
{
    if(mode)
        fillRect(COLOR_LOAD,BAR_X+(BAR_W*offset)/length, BAR_Y, 1 , BAR_H);
    else
        fillRect(COLOR_LOAD,BAR_X,BAR_Y-(BAR_W*offset)/length+1 , BAR_H,(BAR_W*offset)/length+1);
}*/

char * errorMsg[]={
"Error with avlo.cfg",            /*doFault(0)*/
"Bad avlo.cfg file",             /*doFault(1)*/
};

void printFault(int faultNum)
{
    int x,y,h,w;
    setFont(BOX_FONT);
    getStringS("Press a key to retry",&w,&h);
    drawBox(w,3*h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,errorMsg[faultNum]);
    printf("%s\n",errorMsg[faultNum]);
    //putS(COLOR_TXT,COLOR_BOX,x,y+h,"USB is enable, you can access the HD");
    putS(COLOR_TXT,COLOR_BOX,x,y+h,"USB is ON");
    putS(COLOR_TXT,COLOR_BOX,x,y+2*h,"Press a key to retry");    
}

void shutDownString(void)
{
    int x,y,h,w;
    char * str="Shutting down";
    printf("%s\n",str);
    setFont(BOX_FONT);
    getStringS(str,&w,&h);
    drawBox(w,h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,str);
}

void USB_connDisp(void)
{
    setFont(TXT_FONT);
    putS(COLOR_TXT,0x0,USB_X,USB_Y,"USB(F3)");
}

void USB_connHide(void)
{
    int w,h;
    setFont(TXT_FONT);
    getStringS("USB(F3)",&w,&h);
    fillRect(COLOR_TSP,USB_X,USB_Y, w, h);
}
