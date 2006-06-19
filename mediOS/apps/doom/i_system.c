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
// Revision 1.4  2006/05/28 17:08:45  sfxgligli
// aoDoom update (adding browser, PWADs support, optimisations,...)
//
// Revision 1.3  2006/02/06 22:45:48  oxygen77
// make doom work with new api, we lack of exit() now
//
// Revision 1.2  2006/01/19 08:51:51  sfxgligli
// cleanup & name standardisation in wdt/irq/timers/uart/cpld/gio
//
// Revision 1.1  2005/12/20 19:11:56  sfxgligli
// - added Doom port
// - Gmini400 buttons fix
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id$";


#include "medios.h"

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"




int	mb_used = 8;


int I_strncasecmp(char *str1, char *str2, int len)
{
	char c1, c2;

	while ( *str1 && *str2 && len-- ) {
		c1 = *str1++;
		c2 = *str2++;
		if ( toupper(c1) != toupper(c2) )
			return(1);
	}
	return(0);
}

void
I_Tactile
( int	on,
  int	off,
  int	total )
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t	emptycmd;
ticcmd_t*	I_BaseTiccmd(void)
{
    return &emptycmd;
}


int  I_GetHeapSize (void)
{
    return mb_used*1024*1024;
}

byte* I_ZoneBase (int*	size)
{
    byte * z;
    *size = mb_used*1024*1024;
    z=malloc (*size);
    memset(z,0,*size);
    return z;
}



//
// I_GetTime
// returns time in 1/35 second tics
//

int numoverflows=0;
int prevtime=0;
int  I_GetTime (void)
{
  int time;
  time=TMR_GET_CNT(TMR1);

  if (time<prevtime) numoverflows++;

  prevtime=time;

  return (numoverflows<<6)+(time>>10);
}



//
// I_Init
//
void I_Init (void)
{
  // init the timer
  TMR_SET_MODE(TMR_MODE_FREERUN,TMR1);
  TMR_SET_SCAL(753,TMR1);
  TMR_SET_DIV(65535,TMR1);
  TMR_SET_SEL(TMR_SEL_EXT,TMR1)

  I_InitSound();
  //  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit (void)
{
    D_QuitNetGame ();
    I_ShutdownSound();
    I_ShutdownMusic();
    M_SaveDefaults ();
    I_ShutdownGraphics();

    app_exit(false);
}

void I_WaitVBL(int count)
{
//    SDL_Delay((count*1000)/70);
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

byte*	I_AllocLow(int length)
{
    byte*	mem;
        
    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}


//
// I_Error
//
extern boolean demorecording;

void I_Error (char *error, ...)
{
    va_list	argptr;
    char buf[255];

    // Message first.
    va_start (argptr,error);

	  vsprintf(buf,error,argptr);
	  printf("\nI_Error: %s\n",buf);

    va_end (argptr);

    // Shutdown. Here might be other errors.
    if (demorecording)
	G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();

    app_exit(true);
}
