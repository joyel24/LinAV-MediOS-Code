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
// DESCRIPTION:
//	System specific interface stuff.
//
//-----------------------------------------------------------------------------


#ifndef __D_TICCMD__
#define __D_TICCMD__

#include "doomtype.h"

#ifdef __GNUG__
#pragma interface
#endif

// The data sampled per tick (single player)
// and transmitted to other peers (multiplayer).
// Mainly movements/button commands per game tick,
// plus a checksum for internal state consistency.
typedef __attribute__ ((packed)) struct
{
    signed char	__attribute__ ((packed)) forwardmove;	// *2048 for move
    signed char	__attribute__ ((packed)) sidemove;	// *2048 for move
    short	__attribute__ ((packed)) angleturn;	// <<16 for angle delta
    short	__attribute__ ((packed)) consistancy;	// checks for net game
    byte	__attribute__ ((packed)) chatchar;
    byte	__attribute__ ((packed)) buttons;
} __attribute__ ((packed)) ticcmd_t;



#endif
//-----------------------------------------------------------------------------
//
// $Log$
// Revision 1.2  2006/01/03 20:57:56  sfxgligli
// - Doom: weapon change, fixed backward move bug, HUD resize optimisation
// - Medios: added firmware reload function, implemented exit() user function
//
// Revision 1.1  2005/12/20 19:11:56  sfxgligli
// - added Doom port
// - Gmini400 buttons fix
//
//
//-----------------------------------------------------------------------------
