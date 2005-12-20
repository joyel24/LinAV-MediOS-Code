// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log$
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL library
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id$";

#include "medios.h"

#include "m_swap.h"
#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

char button_to_key[NB_BUTTONS]=
 {KEY_UPARROW,KEY_DOWNARROW,KEY_LEFTARROW,KEY_RIGHTARROW,
  KEY_F11,KEY_F6,KEY_F9,
  KEY_RCTRL,' ',
  KEY_ENTER,KEY_ESCAPE};

// 320px -> 220px clever resize of the HUD (thx to WireDDD for the idea)
int hud_resize_table[REALSCREENWIDTH]={
0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,
56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,
106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,
125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,
163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,181,183,
185,187,189,191,193,195,197,199,201,203,205,207,209,211,213,215,217,219,221,
223,225,227,229,231,233,235,236,237,238,239,240,242,243,244,245,246,247,253,
254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,275,
276,277,278,279,280,281,282,283,284,285,286,287,288,292,293,294,295,296,297,
301,302,303,304,305,306,307,308,309,310,311,312,313,314,319};

static char * offset1;
static char * offset2;

int x_resize_lookup[REALSCREENWIDTH];
char * y_resize_lookup[REALSCREENHEIGHT];

__IRAM_DATA char * hud_resize_lookup[REALSCREENWIDTH];

void InitResizeLookups(){
  int i;

  for(i=0;i<REALSCREENHEIGHT;++i){
    y_resize_lookup[i]=offset1+((i*SCREENHEIGHT)/REALSCREENHEIGHT)*SCREENWIDTH;
  }

  for(i=0;i<REALSCREENWIDTH;++i){
    x_resize_lookup[i]=(i*SCREENWIDTH)/REALSCREENWIDTH;

    if (i>0){
      x_resize_lookup[i-1]=x_resize_lookup[i]-x_resize_lookup[i-1];
    }
  }

  for(i=0;i<REALSCREENWIDTH;++i){
    hud_resize_lookup[i]=offset1+(SCREENHEIGHT-SBARHEIGHT-1)*SCREENWIDTH+hud_resize_table[i];
  }
}

void DoFullScreenResize(){
  int i,j;
  char * ip;
  char * op;

  op=offset2;
  for(j=0;j<REALSCREENHEIGHT;++j){
    ip=y_resize_lookup[j];
    for(i=0;i<REALSCREENWIDTH;++i){
      *(op++)=*(ip+=x_resize_lookup[i]);
    }
    op+=SCREENWIDTH-REALSCREENWIDTH;
  }
}

__IRAM_CODE void DoHUDResize(){
  int i,j;
  char * ip;
  char * op;

  op=offset2+(REALSCREENHEIGHT-SBARHEIGHT-1)*SCREENWIDTH;
  for(i=0;i<REALSCREENWIDTH;++i){
    ip=hud_resize_lookup[i];
    for(j=0;j<SBARHEIGHT;++j){
      *(op+=SCREENWIDTH)=*(ip+=SCREENWIDTH);
    }
    op-=SBARHEIGHT*SCREENWIDTH-1;
  }
}

void DoButtonEvent(int button,bool released){
  event_t event;
  event.type = ev_keydown;
  if (released) event.type=ev_keyup;
  event.data1 = button_to_key[button];
  D_PostEvent(&event);
}

//
// I_StartTic
//
static int oldbt=0;

void I_StartTic (void)
{
  int bt,pressed,released;

  bt=read_btn();
  pressed=bt & ~oldbt;
  released=~bt & oldbt;
  oldbt=bt;


  if (pressed){
    if(pressed & BTMASK_UP)     DoButtonEvent(BUTTON_UP,false);
    if(pressed & BTMASK_DOWN)   DoButtonEvent(BUTTON_DOWN,false);
    if(pressed & BTMASK_LEFT)   DoButtonEvent(BUTTON_LEFT,false);
    if(pressed & BTMASK_RIGHT)  DoButtonEvent(BUTTON_RIGHT,false);
    if(pressed & BTMASK_MENU1)  DoButtonEvent(BUTTON_MENU1,false);
    if(pressed & BTMASK_MENU2)  DoButtonEvent(BUTTON_MENU2,false);
    if(pressed & BTMASK_MENU3)  DoButtonEvent(BUTTON_MENU3,false);
    if(pressed & BTMASK_SQUARE) DoButtonEvent(BUTTON_SQUARE,false);
    if(pressed & BTMASK_CROSS)  DoButtonEvent(BUTTON_CROSS,false);
    if(pressed & BTMASK_ON)     DoButtonEvent(BUTTON_ON,false);
    if(pressed & BTMASK_OFF)    DoButtonEvent(BUTTON_OFF,false);
  }

  if (released){
    if(released & BTMASK_UP)     DoButtonEvent(BUTTON_UP,true);
    if(released & BTMASK_DOWN)   DoButtonEvent(BUTTON_DOWN,true);
    if(released & BTMASK_LEFT)   DoButtonEvent(BUTTON_LEFT,true);
    if(released & BTMASK_RIGHT)  DoButtonEvent(BUTTON_RIGHT,true);
    if(released & BTMASK_MENU1)  DoButtonEvent(BUTTON_MENU1,true);
    if(released & BTMASK_MENU2)  DoButtonEvent(BUTTON_MENU2,true);
    if(released & BTMASK_MENU3)  DoButtonEvent(BUTTON_MENU3,true);
    if(released & BTMASK_SQUARE) DoButtonEvent(BUTTON_SQUARE,true);
    if(released & BTMASK_CROSS)  DoButtonEvent(BUTTON_CROSS,true);
    if(released & BTMASK_ON)     DoButtonEvent(BUTTON_ON,true);
    if(released & BTMASK_OFF)    DoButtonEvent(BUTTON_OFF,true);
  }
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//

int prevt=0;

void I_FinishUpdate (void)
{
  char s[20];
  int t;
  t=GET_TIMER_CNT(TMR1);
  sprintf(s,"%6d %6d\n",t-prevt,35840/(t-prevt));
  prevt=t;

  uartOutString(s,0);

  if(menuactive || (gamestate!=GS_LEVEL)){ // not playing?
    // full screen resize
    DoFullScreenResize();
  }else{
    if (viewheight<REALSCREENHEIGHT){ // not fullscreen?
      // blit the 3d view and do a clever resize of the HUD
      memcpy(offset2,offset1,SCREENWIDTH*(REALSCREENHEIGHT-SBARHEIGHT));
      DoHUDResize();
    }else{
      // blit the 3d view
      memcpy(offset2,offset1,SCREENWIDTH*REALSCREENHEIGHT);
    }
  }
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
  int i,r,g,b;

  for ( i=0; i<256; ++i ) {
    r = gammatable[usegamma][*palette++];
    g = gammatable[usegamma][*palette++];
    b = gammatable[usegamma][*palette++];

    setPalletteRGB(r,g,b,i);
  }
}


void I_InitGraphics(void)
{
  open_graphics();

  offset1=malloc(SCREENWIDTH*SCREENHEIGHT);
  offset2=malloc(SCREENWIDTH*SCREENHEIGHT+256);
  offset2=(char*)(((int)offset2+128)&0xffffffe0); // framebuffer address needs to be multiple of 32

  memset(offset1,0,SCREENWIDTH*SCREENHEIGHT);
  memset(offset2,0,SCREENWIDTH*SCREENHEIGHT);

  screens[0]=offset1;
  setBufferOffset (BMAP1,offset2);

  setSize(BMAP1,SCREENWIDTH,SCREENHEIGHT,8);

  InitResizeLookups();
}


void I_ShutdownGraphics(void)
{
}

void I_StartFrame (void)
{
}

