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

extern struct client_operations * cops;

int helperState=0;
int helperDelay=0;
int nbTout=0;
int helperSpeed=200;

void helperEvt(int evt,const int action_btn)
{
    if(evt==action_btn)
    {
        helperState=~helperState;
        if(helperState)
            openHelper();
        else
            closeHelper();
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
            helperState=0;
            closeHelper();
            break;
        /*case EVT_TIMER:
            nbTout++;
            if(nbTout==helperDelay && helperState)
                closeHelper();*/
    }    
}

void hideHelper(void)
{
    stopHelper=1;    
    if(inAction)
        pthread_join(drawHelper_thread, NULL);
    cops->hidePlane(BMAP2);
    posX=320;
}

void openHelper(void)
{
    if(posX>endPosX)
    {
        cops->showPlane(BMAP2);
        isOpening=1;
        if(!inAction)
        {
            if (pthread_create(&drawHelper_thread, NULL, openHelperMenu, 0) != 0)
            {            
                printf("Error, can't create read thread\n");
            }
        }
    }
}

void closeHelper(void)
{
    if(posX<320)
    {
        isOpening=0;
        if(!inAction)
        {
            if (pthread_create(&drawHelper_thread, NULL, openHelperMenu, 0) != 0)
            {            
                printf("Error, can't create read thread\n");
            }
        }
   }
}

void * openHelperMenu(void* arg)
{    
    int i;    
    inAction=1;
    
    while(1)
    {
        cops->setPos(BMAP2,0x14+posX*2,0x13);
        
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
        cops->hidePlane(BMAP2);
   
    /*if(posX<=endPosX && helperDelay == 0)
    {
       nbTout=0;
    }*/
    
    
    inAction=0;    
    
    return NULL;
}

#define prepString(str)        {if(str){ cops->getStringS(str,&w,&h); if(w>maxW) maxW=w;}}
#define drawString(str,y)      {int _x; if(str){ cops->getStringS(str,&w,&h); _x=menu->align?4:maxW-w-1; \
                                 cops->putS(menu->txt_color,menu->bg_color,_x,y,str);}}

/* draw the edit box */
void drawhelperMenuBox(struct helperMenu * menu)
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
    
        
    cops->setSize(BMAP2,maxW,240, 8);
    
    cops->setPlane(BMAP2);
    
    cops->fillRect(menu->bg_color,0,0,maxW,240);
    
    cops->drawRect(menu->border_color,0,0,maxW,2);
    cops->drawRect(menu->border_color,0,0,2,240);
    cops->drawRect(menu->border_color,0,238,maxW,2);
    
    drawString(menu->ON_txt,13);
    drawString(menu->OFF_txt,47);
    drawString(menu->JOY_txt,93);
    drawString(menu->F1_txt,150);
    drawString(menu->F2_txt,180);
    drawString(menu->F3_txt,210);
    
    /*if(menu->ON_txt)  cops->putS(menu->txt_color,menu->bg_color,4,13,menu->ON_txt);
    if(menu->OFF_txt) cops->putS(menu->txt_color,menu->bg_color,4,47,menu->OFF_txt);
    if(menu->JOY_txt) cops->putS(menu->txt_color,menu->bg_color,4,97,menu->JOY_txt);
    if(menu->F1_txt)  cops->putS(menu->txt_color,menu->bg_color,4,150,menu->F1_txt);
    if(menu->F2_txt)  cops->putS(menu->txt_color,menu->bg_color,4,180,menu->F2_txt);
    if(menu->F3_txt)  cops->putS(menu->txt_color,menu->bg_color,4,210,menu->F3_txt);*/
    cops->setPlane(BMAP1);
    // hide Box
    endPosX = 320-maxW;
    posX=320;
    helperState=0;
    stopHelper=0;
    isOpening=0;
    inAction=0;
        
    cops->setPos(BMAP2,0x14+320*2,0x13);
    cops->showPlane(BMAP2);
    
}

