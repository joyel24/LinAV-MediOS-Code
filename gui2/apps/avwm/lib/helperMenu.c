/*
* helperMenu.c
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
#include <stdio.h>
#include <pthread.h>

#include "colordef.h"
#include "misc.h"
#include "cops.h"
#include "events.h"

#include "helperMenu.h"

int endPosX;
int stopHelper=0;
int isOpening=0;
int inAction=0;
int posX=320;

pthread_t drawHelper_thread;

//extern struct client_operations * cops;

int helperDelay=0;
int helperSpeed=200;

void helperEvt(int evt,const int action_btn)
{
    //fprintf(stderr,"[helperEvt] get:%x wait:%x\n",evt,action_btn);
    
    if(evt==action_btn)
    {
        chgState();
        return;
    }
    
    switch(evt)
    {
        case BTN_UP:
        case BTN_LEFT:
        case BTN_RIGHT:
        case BTN_DOWN:
        case BTN_F3:
        case BTN_F1:
        case BTN_F2:
        case BTN_JOY:
        case BTN_ON:
        case BTN_OFF:
            closeHelper();
            break;
    }    
}

void hideHelper(void)
{
    stopHelper=1;    
    /*fprintf(stderr,"wait join after kill\n");
    if (pthread_join(drawHelper_thread, NULL)!=0)
        printf("Error, can't join\n");
    fprintf(stderr,"kill done\n");*/
    hidePlane(BMAP2);
    posX=320;
}

void chgState(void)
{
    isOpening=~isOpening;
    fprintf(stderr,"chg opening\n");
    /*if(!inAction)
    {
        fprintf(stderr,"wait join\n");
        if (pthread_join(drawHelper_thread, NULL)!=0)
            printf("Error, can't join\n");
        fprintf(stderr,"launch\n");
        if (pthread_create(&drawHelper_thread, NULL, drawHelperMenu, 0) != 0)
            printf("Error, can't create draw thread\n");
    }*/
    drawHelperMenu(NULL);
}

void openHelper(void)
{
    if(!isOpening)
        chgState();
}

void closeHelper(void)
{
    if(isOpening)
        chgState();
}

void * drawHelperMenu(void* arg)
{    
    int i;    
    inAction=1;
    fprintf(stderr,"IN thread\n");
    showPlane(BMAP2);
    
    while(1)
    {
        setPos(BMAP2,0x14+posX*2,0x13);
        
        if(isOpening)
        {
            posX--;
            if(posX<endPosX || stopHelper)
                break;
        }
        else
        {
            posX++;
            if(posX>320 || stopHelper)
                break;
        }
        for(i=0;i<helperSpeed;i++) /* NOTHING */;
    }
    
    if(posX>=320)
        hidePlane(BMAP2);
   
    inAction=0;    
    fprintf(stderr,"OUT thread\n");
    return NULL;
}

#define prepString(str)        {if(str){ getStringS(str,&w,&h); if(w>maxW) maxW=w;}}
#define drawString(str,y)      {int _x; if(str){ getStringS(str,&w,&h); _x=menu->align?4:maxW-w-1; \
                                 putS(menu->txt_color,menu->bg_color,_x,y,str);}}

/* draw the edit box */
void iniHelperMenu(struct helperMenu * menu)
{
    
    int h,w;
    int maxW=0;   
    
    helperDelay=menu->helperDelay;
    helperSpeed=menu->helperSpeed;
    
    prepString(menu->ON_txt);
    prepString(menu->OFF_txt);
    prepString(menu->JOY_txt);
    prepString(menu->F1_txt);
    prepString(menu->F2_txt);
    prepString(menu->F3_txt);
    
    maxW+=4;
    
    if(maxW>320)
        maxW=320;    
    
        
    setSize(BMAP2,maxW,240, 8);
    
    setPlane(BMAP2);
    
    fillRect(menu->bg_color,0,0,maxW,240);
    
    drawRect(menu->border_color,0,0,maxW,2);
    drawRect(menu->border_color,0,0,2,240);
    drawRect(menu->border_color,0,238,maxW,2);
    
    drawString(menu->ON_txt,13);
    drawString(menu->OFF_txt,47);
    drawString(menu->JOY_txt,93);
    drawString(menu->F1_txt,150);
    drawString(menu->F2_txt,180);
    drawString(menu->F3_txt,210);
    
    setPlane(BMAP1);
    
    // hide Box
    endPosX = 320-maxW;
    posX=320;
    stopHelper=0;
    isOpening=0;
    inAction=0;
        
    setPos(BMAP2,0x14+320*2,0x13);
    showPlane(BMAP2);
    
}

